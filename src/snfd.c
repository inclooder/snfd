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
 * Returns 0 if not found.
 */
SNFD_UINT32 snfd_find_last_order_number_for_file(SNFD * snfd, 
                                                 SNFD_FILE_NUMBER file_nr)
{
    SNFD_LOG log;
    SNFD_LOG next_log;
    if(snfd_log_find_first(snfd, file_nr, &log) == 0)
    {
        return 0;
    }
    while(snfd_log_find_next(snfd, &log, &next_log) != 0)
    {
        memcpy(&log, &next_log, sizeof(SNFD_LOG));
    }
    return log.order_number;
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

    SNFD_UINT32 collision_log = snfd_log_find_prev_with_collision(snfd,
                                                                  file_nr,
                                                                  log.order_number,
                                                                  destination,
                                                                  size);

    if(collision_log != 0)
    {
        //Mark log as inactive
        //Change block state to SNFD_DIRTY
    }
  

    snfd_garbage_collect(snfd);
    return SNFD_ERROR_NO_ERROR;
}

SNFD_ERROR snfd_read_file(SNFD * snfd,
                          SNFD_FILE_NUMBER file_nr,
                          SNFD_UINT32 source,
                          void * destination,
                          SNFD_UINT32 size)
{

    SNFD_LOG log;
    SNFD_UINT32 log_loc = snfd_log_find_first(snfd, file_nr, &log);
    if(log_loc == 0)
    {
        return SNFD_ERROR_FILE_NOT_FOUND;
    }

    SNFD_LOG next_log;
    SNFD_UINT32 data_end;
    SNFD_UINT32 read_size;
    SNFD_UINT32 read_loc;
    SNFD_UINT32 read_offset;
    SNFD_UINT32 source_end = source + size;
    for(;;)
    {
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

        log_loc = snfd_log_find_next(snfd, &log, &next_log);
        if(log_loc != 0){
            memcpy(&log, &next_log, sizeof(SNFD_LOG));
        } else {
            break;
        }
    }

    return SNFD_ERROR_NO_ERROR;
}
