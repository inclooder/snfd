#include "snfd_internal.h"
#include "string.h"
#include "snfd_utils.h"

SNFD_ERROR snfd_log_read(SNFD * snfd, SNFD_UINT32 source, SNFD_LOG * log)
{
    return snfd_direct_read(snfd, source, log, sizeof(SNFD_LOG));
}

/*
 * Returns 0 if log was not found.
 */
SNFD_UINT32 snfd_log_find_first(SNFD * snfd, SNFD_FILE_NUMBER file_nr, SNFD_LOG * log_out)
{
    SNFD_BLOCK_NUMBER block_number;
    SNFD_UINT32 next_log_number = 0xFFFFFFFF;
    SNFD_UINT32 offset;
    SNFD_LOG log_tmp;
    SNFD_UINT32 log_loc = 0;
    for(block_number = 0; block_number < SNFD_BLOCKS_COUNT; ++block_number)
    {
        if(!snfd_block_has_logs(snfd, block_number)) break;
        offset = sizeof(SNFD_BLOCK_HEADER); //Skip block header
        while(offset < SNFD_BLOCK_SIZE)
        {
            snfd_log_read(snfd, (block_number * SNFD_BLOCK_SIZE) + offset, &log_tmp);
            if(snfd_log_is_invalid(&log_tmp)) break; //Break on first invalid log
            if(log_tmp.file_number == file_nr &&
               log_tmp.order_number > 0 &&
               log_tmp.order_number < next_log_number)
            {
                memcpy(log_out, &log_tmp, sizeof(SNFD_LOG));
                log_loc = (block_number * SNFD_BLOCK_SIZE) + offset;
                next_log_number = log_tmp.order_number;
            }

            offset += sizeof(log_tmp) + log_tmp.data_size;
        }
    }
    return log_loc;
}

/*
 * Searches for next log.
 * Returns 0 if log was not found.
 */
SNFD_UINT32 snfd_log_find_next(SNFD * snfd, 
                               SNFD_LOG * current_log, 
                               SNFD_LOG * next_log)
{
    SNFD_BLOCK_NUMBER block_number;
    SNFD_UINT32 next_log_number = 0xFFFFFFFF;
    SNFD_UINT32 offset;
    SNFD_LOG log_tmp;
    SNFD_UINT32 log_loc = 0;
    for(block_number = 0; block_number < SNFD_BLOCKS_COUNT; ++block_number)
    {
        if(!snfd_block_has_logs(snfd, block_number)) break;
        offset = sizeof(SNFD_BLOCK_HEADER); //Skip block header
        while(offset < SNFD_BLOCK_SIZE)
        {
            snfd_log_read(snfd, (block_number * SNFD_BLOCK_SIZE) + offset, &log_tmp);
            if(snfd_log_is_invalid(&log_tmp)) break; //Break on first invalid log
            if(log_tmp.file_number == current_log->file_number &&
               log_tmp.order_number > current_log->order_number &&
               log_tmp.order_number < next_log_number)
            {
                memcpy(next_log, &log_tmp, sizeof(SNFD_LOG));
                next_log_number = log_tmp.order_number;
                log_loc = (block_number * SNFD_BLOCK_SIZE) + offset;
            }

            offset += sizeof(log_tmp) + log_tmp.data_size;
        }
    }
    return log_loc;
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
    SNFD_UINT32 prev_log_number = 0;
    SNFD_UINT32 offset;
    SNFD_LOG log;
    for(block_number = 0; block_number < SNFD_BLOCKS_COUNT; ++block_number)
    {
        if(!snfd_block_has_logs(snfd, block_number)) continue;
        offset = sizeof(SNFD_BLOCK_HEADER); //Skip block header
        while(offset < SNFD_BLOCK_SIZE)
        {
            snfd_log_read(snfd, (block_number * SNFD_BLOCK_SIZE) + offset, &log);
            if(snfd_log_is_invalid(&log)) break; //Break on first invalid log
            if(log.file_number == file_nr && log.order_number < log_number && log.order_number > prev_log_number)
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
                                             SNFD_UINT32 size,
                                             SNFD_LOG * collision_log)
{
    SNFD_LOG log;
    SNFD_UINT32 log_loc;
    SNFD_UINT32 data_end;
    SNFD_UINT32 source_end = file_offset + size;
    while(log_number != 0){
        log_loc = snfd_log_find_prev(snfd, file_nr, log_number--);
        snfd_log_read(snfd, log_loc, &log);
        if(log.state == SNFD_LOG_OVERWRITTEN) 
        {
            continue; //Ignore logs that was already marked as overwritten
        }

        SNFD_SEGMENT a;
        a.begin = file_offset;
        a.end = file_offset + size;
        SNFD_SEGMENT b;
        b.begin = log.start_loc;
        b.end = log.start_loc + log.data_size;
        SNFD_SEGMENT_INTERSECTION inter;
        snfd_check_segment_intersection(&a, &b, &inter);
        if(inter.size != 0)
        {
            memcpy(collision_log, &log, sizeof(SNFD_LOG));
            return log_loc;
        }
    }
    return 0;
}

SNFD_ERROR snfd_log_state_change(SNFD * snfd, SNFD_UINT32 log_loc, SNFD_LOG_STATE new_state)
{
    SNFD_LOG log;
    snfd_log_read(snfd, log_loc, &log);
    log.state = new_state;
    snfd_direct_write(snfd, log_loc, &log, sizeof(log));
    return SNFD_ERROR_NO_ERROR;
}

