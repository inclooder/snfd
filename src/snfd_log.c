#include "snfd_internal.h"

SNFD_ERROR snfd_log_read(SNFD * snfd, SNFD_UINT32 source, SNFD_LOG * log)
{
    return snfd_direct_read(snfd, source, log, sizeof(SNFD_LOG));
}

/*
 * Searches for next log based on order_number.
 * Returns first log if order_number == 0
 * Returns 0 if log was not found.
 */
SNFD_UINT32 snfd_log_find_next(SNFD * snfd, 
                               SNFD_FILE_NUMBER file_nr, 
                               SNFD_UINT32 order_number)
{
    SNFD_UINT32 next_log_number = order_number + 1;
    SNFD_BLOCK_NUMBER block_number;
    SNFD_UINT32 offset;
    SNFD_LOG log;
    SNFD_BLOCK_STATE block_state;
    for(block_number = 0; block_number < SNFD_BLOCKS_COUNT; ++block_number)
    {
        if(!snfd_block_has_logs(snfd, block_number)) break;
        offset = sizeof(SNFD_BLOCK_HEADER); //Skip block header
        while(offset < SNFD_BLOCK_SIZE)
        {
            snfd_log_read(snfd, (block_number * SNFD_BLOCK_SIZE) + offset, &log);
            if(snfd_log_is_invalid(&log)) break; //Break on first invalid log
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
 * Returns 0 if previous log was not found.
 */
SNFD_UINT32 snfd_log_find_prev(SNFD * snfd, 
                               SNFD_FILE_NUMBER file_nr,
                               SNFD_UINT32 log_number)
{
    SNFD_BLOCK_NUMBER block_number;
    SNFD_UINT32 log_loc = 0;
    SNFD_UINT32 prev_log_number = log_number;
    SNFD_UINT32 offset;
    SNFD_LOG log;
    SNFD_BLOCK_STATE block_state;
    for(block_number = 0; block_number < SNFD_BLOCKS_COUNT; ++block_number)
    {
        if(!snfd_block_has_logs(snfd, block_number)) break;
        offset = sizeof(SNFD_BLOCK_HEADER); //Skip block header
        while(offset < SNFD_BLOCK_SIZE)
        {
            snfd_log_read(snfd, (block_number * SNFD_BLOCK_SIZE) + offset, &log);
            if(snfd_log_is_invalid(&log)) break; //Break on first invalid log
            if(log.file_number == file_nr && log.order_number < prev_log_number)
            {
                log_loc = (block_number * SNFD_BLOCK_SIZE) + offset;
                prev_log_number = log.order_number;
            }

            offset += sizeof(log) + log.data_size;
        }
    }
    return log_loc;
}

/*
 * Find previous log that bytes collie
 */
SNFD_UINT32 snfd_log_find_prev_with_collision(SNFD * snfd,
                                             SNFD_FILE_NUMBER file_nr,
                                             SNFD_UINT32 log_number,
                                             SNFD_UINT32 file_offset,
                                             SNFD_UINT32 size)
{
    SNFD_LOG log;
    SNFD_UINT32 log_loc;
    SNFD_UINT32 data_end;
    SNFD_UINT32 source_end = file_offset + size;
    while(log_number != 0){
        log_loc = snfd_log_find_prev(snfd, file_nr, log_number);
        snfd_log_read(snfd, log_loc, &log);
        data_end = log.start_loc + log.data_size;
        SNFD_UINT32 collision_size = 0;
        if(file_offset >= log.start_loc && file_offset < data_end)
        {
            if(data_end < (source_end)) collision_size = data_end - file_offset;
            else collision_size = size;
        } 
        else if(log.start_loc >= file_offset && log.start_loc < (source_end)) 
        {
            if(data_end > source_end) collision_size = source_end - log.start_loc;
            else collision_size = log.data_size;
        }
        if(collision_size != 0 && collision_size >= log.data_size / 2)
        {
            return log_loc;
        }
    }
    return 0;
}
