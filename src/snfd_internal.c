#include "snfd.h"
#include "snfd_internal.h"
#include <string.h>
#include <stdio.h>

SNFD_UINT32 snfd_calc_read_size(SNFD_UINT32 read_buffer_size, 
                                SNFD_UINT32 block_size, 
                                SNFD_UINT32 offset)
{

    block_size = block_size - offset;
    SNFD_UINT32 read_size = read_buffer_size;
    if(read_size > block_size) read_size = block_size;
    return read_size;
}

SNFD_BOOL snfd_is_block_broken(SNFD * snfd, SNFD_BLOCK_HEADER * header)
{
    return header->state == SNFD_BLOCK_BROKEN;
}

SNFD_BOOL snfd_is_block_free(SNFD * snfd, SNFD_BLOCK_HEADER * header)
{
    return header->state == SNFD_BLOCK_FREE;
}

SNFD_BOOL snfd_is_block_clean(SNFD * snfd, SNFD_BLOCK_HEADER * header)
{
    return header->state == SNFD_BLOCK_CLEAN;
}

SNFD_BOOL snfd_is_block_dirty(SNFD * snfd, SNFD_BLOCK_HEADER * header)
{
    return header->state == SNFD_BLOCK_DIRTY;
}

void snfd_read_block_header(SNFD * snfd, 
                            SNFD_BLOCK_NUMBER block_number, 
                            SNFD_BLOCK_HEADER * header)
{
    snfd_direct_read(snfd, block_number * SNFD_BLOCK_SIZE, header, sizeof(SNFD_BLOCK_HEADER));
}


SNFD_BOOL snfd_is_block_initialized(SNFD * snfd, SNFD_BLOCK_HEADER * header)
{
    return memcmp(header->magic_number, SNFD_MAGIC_NUMBER, SNFD_MAGIC_NUMBER_SIZE) == 0;
}

SNFD_BOOL snfd_log_is_invalid(SNFD_LOG * log)
{
    return log->file_number == 0xFFFF || log->state == SNFD_LOG_INVALID;
}

void snfd_block_erase_safe(SNFD * snfd, SNFD_BLOCK_NUMBER block_nr)
{
    
    SNFD_LOG log_tmp;
    SNFD_LOG log_tmp2;
    SNFD_UINT32 offset = sizeof(SNFD_BLOCK_HEADER); //Skip block header
    SNFD_FILE_NUMBER file_number;
    SNFD_UINT32 offset2;
    SNFD_BLOCK_NUMBER block_number;
    while(offset < SNFD_BLOCK_SIZE)
    {
        snfd_log_read(snfd, (block_nr * SNFD_BLOCK_SIZE) + offset, &log_tmp);
        if(snfd_log_is_invalid(&log_tmp)) break; //Break on first invalid log
        if(log_tmp.state == SNFD_LOG_INACTIVE) break;
        file_number = log_tmp.file_number;

        //Mark all logs of this file to moved
        for(block_number = 0; block_number < SNFD_BLOCKS_COUNT; ++block_number)
        {
            offset2 = sizeof(SNFD_BLOCK_HEADER);
            while(offset2 < SNFD_BLOCK_SIZE)
            {
                snfd_log_read(snfd, (block_number * SNFD_BLOCK_SIZE) + offset2, &log_tmp2);
                if(snfd_log_is_invalid(&log_tmp2)) break; //Break on first invalid log

                if(log_tmp2.file_number == file_number && log_tmp2.state != SNFD_LOG_INACTIVE)
                {
                    snfd_log_state_change(snfd, (block_number * SNFD_BLOCK_SIZE) + offset2, SNFD_LOG_MOVED);
                }

                offset2 += sizeof(SNFD_LOG) + log_tmp2.data_size;
            }
        }
        
        //Move file to the new location
        SNFD_UINT32 file_size = snfd_file_calc_size(snfd, file_number);
        SNFD_UINT32 write_loc = snfd_find_space_for_new_log(snfd, file_size);
        if(write_loc == 0){
            return; //Error no space left!
        }
        snfd_log_create(snfd, file_number, 0, file_size, write_loc);

        SNFD_UINT32 remaining_bytes = file_size;
        SNFD_UINT32 read_size;
        while(remaining_bytes > 0)
        {
            read_size = sizeof(snfd->buffer);
            if(read_size > remaining_bytes)
            {
                read_size = remaining_bytes;
            }
            
            snfd_read_file(snfd, file_number, file_size - remaining_bytes, snfd->buffer, read_size);
            snfd_direct_write(snfd, write_loc + sizeof(SNFD_LOG) + file_size - remaining_bytes, snfd->buffer, read_size);

            remaining_bytes -= read_size;
        }

        //Mark all logs with state MOVED as INACTIVE
        for(block_number = 0; block_number < SNFD_BLOCKS_COUNT; ++block_number)
        {
            offset2 = sizeof(SNFD_BLOCK_HEADER);
            while(offset2 < SNFD_BLOCK_SIZE)
            {
                snfd_log_read(snfd, (block_number * SNFD_BLOCK_SIZE) + offset2, &log_tmp2);
                if(snfd_log_is_invalid(&log_tmp2)) break; //Break on first invalid log
                if(log_tmp2.file_number == file_number && log_tmp2.state == SNFD_LOG_MOVED)
                {
                    snfd_log_state_change(snfd, (block_number * SNFD_BLOCK_SIZE) + offset2, SNFD_LOG_INACTIVE);
                }

                offset2 += sizeof(SNFD_LOG) + log_tmp2.data_size;
            }
        }
        

        offset += sizeof(log_tmp) + log_tmp.data_size;
    }
    //Erase a block here
    snfd_erase_and_initialize_block(snfd, block_nr);
}

void snfd_erase_most_dirty_block(SNFD * snfd)
{
    SNFD_BLOCK_NUMBER block_number;
    SNFD_UINT32 offset;
    SNFD_LOG log_tmp;
    SNFD_INT32 block_to_erase = -1;
    SNFD_UINT32 overwritten_logs = 0;
    SNFD_UINT32 overwritten_logs_tmp = 0;
    for(block_number = 0; block_number < SNFD_BLOCKS_COUNT; block_number++)
    {
        if(snfd->blocks[block_number].state != SNFD_BLOCK_DIRTY) continue;

        offset = sizeof(SNFD_BLOCK_HEADER); //Skip block header
        overwritten_logs_tmp = 0;
        while(offset < SNFD_BLOCK_SIZE)
        {
            snfd_log_read(snfd, (block_number * SNFD_BLOCK_SIZE) + offset, &log_tmp);
            if(snfd_log_is_invalid(&log_tmp)) break; //Break on first invalid log
            
            if(log_tmp.state == SNFD_LOG_OVERWRITTEN || log_tmp.state == SNFD_LOG_INACTIVE)
            {
                overwritten_logs_tmp++;
            }

            offset += sizeof(log_tmp) + log_tmp.data_size;
        }
        if(overwritten_logs_tmp > overwritten_logs)
        {
            overwritten_logs = overwritten_logs_tmp;
            block_to_erase = block_number;
        }
    }
    if(block_to_erase != -1)
    {
        //Move data and erase block
        snfd_block_erase_safe(snfd, block_to_erase);
    }
}

/*
 * Find dirty blocks and erase them
 */
void snfd_garbage_collect(SNFD * snfd)
{
    {
        SNFD_BLOCK_NUMBER block_number;
        SNFD_UINT16 free_blocks = 0;
        for(block_number = 0; block_number < SNFD_BLOCKS_COUNT; block_number++)
        {
            if(snfd->blocks[block_number].state == SNFD_BLOCK_FREE) free_blocks++;
        }
        if(free_blocks >= 4) return; //Do nothing if there are at least four free blocks.
    }
    SNFD_UINT32 i;
    for(i = 0; i < 4; ++i)
    {
        snfd_erase_most_dirty_block(snfd);
    }
}


SNFD_UINT16 snfd_calc_block_number_from_physical_addr(SNFD_UINT32 physical_addr)
{
    return physical_addr / SNFD_BLOCK_SIZE;
}

/*
 * Finds first free log in the block.
 * Block must be initialized.
 * Returns 0 if block was not found.
 */
SNFD_UINT32 snfd_find_free_log_in_block(SNFD * snfd,
                                        SNFD_BLOCK_NUMBER block_number)
{
    SNFD_UINT32 next_log_loc = (block_number * SNFD_BLOCK_SIZE) + sizeof(SNFD_BLOCK_HEADER);
    SNFD_LOG log;
    SNFD_UINT32 max_log_loc = (block_number * SNFD_BLOCK_SIZE) + SNFD_BLOCK_SIZE;
    while(next_log_loc < max_log_loc)
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
        //Check only clean, dirty and free blocks, skip others.
        if(state != SNFD_BLOCK_CLEAN && state != SNFD_BLOCK_FREE && state != SNFD_BLOCK_DIRTY) continue;
        tmp_write_loc = snfd_find_free_log_in_block(snfd, i);
        free_size = SNFD_BLOCK_SIZE - (tmp_write_loc - (i * SNFD_BLOCK_SIZE));
        if(free_size < size) continue; //Skip locations that are too small
        // Check if this location is better or it's the first location we found.
        if(last_free_size == 0 || 
           free_size < last_free_size)
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

SNFD_BOOL snfd_block_has_logs(SNFD * snfd, SNFD_BLOCK_NUMBER block_nr)
{
        if(snfd->blocks[block_nr].state == SNFD_BLOCK_CLEAN ||
           snfd->blocks[block_nr].state == SNFD_BLOCK_DIRTY)
        {
            return SNFD_TRUE;
        } else {
            return SNFD_FALSE;
        }
}

