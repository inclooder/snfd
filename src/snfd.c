#include "snfd.h"
#include <stdlib.h>
#include <string.h>
#include "snfd_internal.h"

/*
 * 1. Initialize blocks which are broken or not initialized.
 * 2. Store state of each block
 */
SNFD_ERROR snfd_startup(SNFD * snfd)
{
    SNFD_UINT32 i;
    SNFD_BLOCK_STATE block_state;
    SNFD_BLOCK_HEADER header;
    for(i = 0; i < SNFD_BLOCKS_COUNT; ++i)
    {
        snfd_read_block_header(snfd, i, &header);
        block_state = header.state;
        if(!snfd_is_block_initialized(snfd, &header) ||
                snfd_is_block_broken(snfd, &header))
        {
            if(snfd_check_block(snfd, i))
            {
                snfd_erase_and_initialize_block(snfd, i);
                block_state = SNFD_BLOCK_FREE;
            } else {
                block_state = SNFD_BLOCK_BROKEN;
            }
        }
        snfd->blocks[i].state = block_state;
    }
    return SNFD_ERROR_NO_ERROR;
}

void snfd_cleanup(SNFD * snfd)
{
}

/*
 * Finds first free log in the block.
 * Block must be initialized.
 * Returns 0 if block was not found.
 */
SNFD_UINT32 snfd_find_free_log_in_block(SNFD * snfd,
                                        SNFD_UINT16 block_number)
{
    SNFD_UINT32 next_log_loc = (block_number * SNFD_BLOCK_SIZE) + sizeof(SNFD_BLOCK_HEADER);
    SNFD_LOG log;
    while(next_log_loc < SNFD_BLOCK_SIZE)
    {
        snfd_direct_read(snfd, next_log_loc, &log, sizeof(log));
        if(snfd_log_is_invalid(&log))
        {
            return next_log_loc;
        }
        next_log_loc += log.data_size + sizeof(log);
    }
    return 0;
}

/*
 * Returns 0 if log was not found.
 */
SNFD_UINT32 snfd_find_last_log_for_file(SNFD * snfd, SNFD_FILE_NUMBER file_nr)
{
    //TODO Add cache
    
    SNFD_UINT32 last_log_location = 0;
    SNFD_UINT32 block;
    SNFD_UINT32 offset;
    SNFD_UINT32 memory_location;
    SNFD_BLOCK_STATE block_state;
    SNFD_LOG log;
    for(block = 0; block < SNFD_BLOCKS_COUNT; ++block)
    {
        block_state = snfd->blocks[block].state;
        if(block_state != SNFD_BLOCK_CLEAN || block_state != SNFD_BLOCK_DIRTY) break;

        for(offset = sizeof(SNFD_BLOCK_HEADER); offset < SNFD_BLOCK_SIZE; ++offset)
        {
            memory_location = (block * SNFD_BLOCK_SIZE) + offset;
            snfd_direct_read(snfd, memory_location, &log, sizeof(log));
            if(snfd_log_is_invalid(&log) || log.file_number != file_nr)
            {
                break;
            }

            if(log.next_log == SNFD_LOG_NO_NEXT)
            {
                last_log_location = memory_location;
                block = SNFD_BLOCKS_COUNT;
                break;
            }
        }
    }
    return last_log_location;
}

/*
 * Returns 0 if log was not found.
 */
SNFD_UINT32 snfd_find_first_log_for_file(SNFD * snfd, SNFD_FILE_NUMBER file_nr)
{
    //TODO Add cache

    SNFD_UINT32 first_log_location = 0;
    SNFD_UINT32 block;
    SNFD_UINT32 offset;
    SNFD_UINT32 memory_location;
    SNFD_BLOCK_STATE block_state;
    SNFD_LOG log;
    for(block = 0; block < SNFD_BLOCKS_COUNT; ++block)
    {
        block_state = snfd->blocks[block].state;
        if(block_state != SNFD_BLOCK_CLEAN || block_state != SNFD_BLOCK_DIRTY) break;

        for(offset = sizeof(SNFD_BLOCK_HEADER); offset < SNFD_BLOCK_SIZE; ++offset)
        {
            memory_location = (block * SNFD_BLOCK_SIZE) + offset;
            snfd_direct_read(snfd, memory_location, &log, sizeof(log));
            if(snfd_log_is_invalid(&log) || log.file_number != file_nr)
            {
                //Skip invalid logs or those with different file_number.
                break;
            }

            if(log.prev_log == SNFD_LOG_NO_PREV)
            {
                first_log_location = memory_location;
                block = SNFD_BLOCKS_COUNT;
                break;
            }
        }
    }
    return first_log_location;
}

SNFD_UINT32 snfd_find_space_for_new_log(SNFD * snfd, SNFD_UINT32 size)
{
    SNFD_UINT32 i;
    SNFD_BLOCK_STATE state;
    SNFD_UINT32 write_loc = 0;
    SNFD_UINT32 last_free_size = 0;
    SNFD_UINT32 free_size = 0;
    SNFD_UINT32 tmp_write_loc = 0;
    for(i = 0; i < SNFD_BLOCKS_COUNT; ++i) //For each block
    {
        state = snfd->blocks[i].state;
        /*
         * Check only clean and free blocks, skip others.
         */
        if(state != SNFD_BLOCK_CLEAN && state != SNFD_BLOCK_FREE) continue;
        tmp_write_loc = find_free_log_in_block(snfd, i);
        free_size = SNFD_BLOCK_SIZE - tmp_write_loc - sizeof(SNFD_LOG);
        /*
         * Check if this location is better or it's the first location we found.
         */
        if(last_free_size == 0 || 
           (free_size >= size && last_free_size >= free_size))
        {
            last_free_size = free_size;
            write_loc = tmp_write_loc;
            if(last_free_size - size <= 10)
            {
                break;
            }
        }
    }
    return write_loc;
}

/*
 * 1. Find free space to write.
 * 2. Write log entry.
 * 3. Change block state if needed.
 */
SNFD_ERROR snfd_write_file(SNFD * snfd, 
                           SNFD_FILE_NUMBER file_nr, 
                           SNFD_UINT32 destination, 
                           void * source, SNFD_UINT32 size)
{
    // TODO: Split into two write operations if size > SNFD_BLOCK_SIZE - sizeof(SNFD_BLOCK_HEADER)
    SNFD_UINT32 write_loc = snfd_find_space_for_new_log(snfd, size);

    if(write_loc == 0){
        return SNFD_ERROR_NO_SPACE_LEFT;
    }
    
    SNFD_LOG log;
    log.file_number = file_nr;
    log.file_operation = SNFD_LOG_OPERATION_SET;
    log.state = SNFD_LOG_ACTIVE;
    log.start_loc = destination;
    log.data_size = size;
    SNFD_UINT32 prev_log = snfd_find_last_log_for_file(snfd, file_nr);
    log.prev_log = prev_log ? prev_log : SNFD_LOG_NO_PREV;
    log.next_log = SNFD_LOG_NO_NEXT;

    snfd_direct_write(snfd, write_loc, &log, sizeof(log));
    snfd_direct_write(snfd, write_loc + sizeof(log), source, size);

    // Set next log of the parent to write_loc
    snfd_direct_read(snfd, prev_log, &log, sizeof(log));
    log.next_log = write_loc;
    snfd_direct_write(snfd, prev_log, &log, sizeof(log));

    // TODO: check if write succeded

    SNFD_UINT16 block_nr = snfd_calc_block_number_from_physical_addr(write_loc);
    if(snfd->blocks[block_nr].state == SNFD_BLOCK_FREE){
        //change state to SNFD_CLEAN
        SNFD_BLOCK_HEADER block_header;
        snfd_read_block_header(snfd, block_nr, &block_header);
        block_header.state = SNFD_BLOCK_CLEAN;
        snfd_direct_write(snfd, block_nr * SNFD_BLOCK_SIZE,
                          &block_header,
                          sizeof(block_header));
        snfd->blocks[block_nr].state = SNFD_BLOCK_CLEAN;
    }

    // TODO: garbage collect
}

SNFD_ERROR snfd_read_file(SNFD * snfd,
                          SNFD_FILE_NUMBER file_nr,
                          SNFD_UINT32 source,
                          void * destination,
                          SNFD_UINT32 size)
{
    SNFD_UINT32 log_loc = snfd_find_first_log_for_file(snfd, file_nr);
    if(log_loc == 0)
    {
        return SNFD_ERROR_FILE_NOT_FOUND;
    }

    SNFD_LOG log;
    SNFD_UINT32 data_end;
    SNFD_UINT32 read_size;
    SNFD_UINT32 read_loc;
    SNFD_UINT32 read_offset;
    SNFD_UINT32 source_end = source + size;
    while(1)
    {
        snfd_direct_read(snfd, log_loc, &log, sizeof(log));
        if(log.next_log == SNFD_LOG_NO_NEXT) break;

        //Read data and write to destination
        data_end = log.start_loc + log.data_size;
        if(source >= log.start_loc && source < data_end)
        {
            //  log.start_loc    data_end
            //     |=================|
            //                  |=================|
            //               source            source_end
            
            read_offset = source - log.start_loc;
            read_loc = log_loc + sizeof(log) + read_offset;

            if(data_end < (source_end)) read_size = data_end - source;
            else read_size = size;

            snfd_direct_read(snfd, read_loc, ((SNFD_UINT8 *)destination) + read_offset, read_size);
        } 
        else if(log.start_loc >= source && log.start_loc < (source_end)) 
        {
            //               log.start_loc    data_end
            //                  |=================|
            //     |=================|
            //  source            source_end

            read_offset = 0;
            read_loc = log_loc + sizeof(log) + read_offset;

            if(data_end > source_end) read_size = source_end - log.start_loc;
            else read_size = log.data_size;

            snfd_direct_read(snfd, read_loc, ((SNFD_UINT8 *)destination) + read_offset, read_size);
        }

        log_loc = log.next_log;
    }

    return SNFD_ERROR_NO_ERROR;
}
