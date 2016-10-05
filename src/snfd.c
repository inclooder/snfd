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
 * Searches for next log based on order_number.
 * Returns first log if order_number == 0
 * Returns 0 if log was not found.
 */
SNFD_UINT32 snfd_find_next_log_for_file(SNFD * snfd, SNFD_FILE_NUMBER file_nr, SNFD_UINT32 order_number)
{
    SNFD_UINT32 next_log_number = order_number + 1;
    SNFD_BLOCK_NUMBER block_number;
    SNFD_UINT32 offset;
    SNFD_LOG log;
    SNFD_BLOCK_STATE block_state;
    for(block_number = 0; block_number < SNFD_BLOCKS_COUNT; block_number++)
    {
        if(!snfd_block_has_logs(snfd, block_number)) break;
        offset = sizeof(SNFD_BLOCK_HEADER); //Skip block header
        while(offset < SNFD_BLOCK_SIZE)
        {
            snfd_direct_read(snfd, (block_number * SNFD_BLOCK_SIZE) + offset, &log, sizeof(log));
            if(snfd_log_is_invalid(&log)) break;
            if(log.file_number == file_nr && log.order_number == next_log_number)
            {
                return (block_number * SNFD_BLOCK_SIZE) + offset;
            }

            offset += sizeof(log) + log.data_size;
        }
    }
    return 0;
}

/*
 * Returns 0 if not found.
 */
SNFD_UINT32 snfd_find_last_order_number_for_file(SNFD * snfd, SNFD_FILE_NUMBER file_nr)
{
    SNFD_UINT32 order_number = 0;
    while(snfd_find_next_log_for_file(snfd, file_nr, order_number) != 0)
    {
        order_number++;
    }
    return order_number;
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
    log.order_number = snfd_find_last_order_number_for_file(snfd, file_nr) + 1;

    snfd_direct_write(snfd, write_loc, &log, sizeof(log));
    snfd_direct_write(snfd, write_loc + sizeof(log), source, size);

    // TODO: check if write succeded

    SNFD_BLOCK_NUMBER block_nr = snfd_calc_block_number_from_physical_addr(write_loc);
    if(snfd->blocks[block_nr].state == SNFD_BLOCK_FREE){
        //change state to SNFD_CLEAN
        snfd_block_state_change(snfd, block_nr, SNFD_BLOCK_CLEAN);
    }

    // TODO: garbage collect
}

SNFD_ERROR snfd_read_file(SNFD * snfd,
                          SNFD_FILE_NUMBER file_nr,
                          SNFD_UINT32 source,
                          void * destination,
                          SNFD_UINT32 size)
{

    SNFD_UINT32 order_number = 0;
    SNFD_UINT32 log_loc = snfd_find_next_log_for_file(snfd, file_nr, order_number++);
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
    while(log_loc != 0)
    {
        snfd_direct_read(snfd, log_loc, &log, sizeof(log));

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

            read_offset = log.start_loc - source;
            read_loc = log_loc + sizeof(log);

            if(data_end > source_end) read_size = source_end - log.start_loc;
            else read_size = log.data_size;

            snfd_direct_read(snfd, read_loc, ((SNFD_UINT8 *)destination) + read_offset, read_size);
        }

        log_loc = snfd_find_next_log_for_file(snfd, file_nr, order_number++);
    }

    return SNFD_ERROR_NO_ERROR;
}
