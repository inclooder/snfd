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
        if(!snfd_is_block_initialized(snfd, i) ||
                snfd_is_block_broken(snfd, i))
        {
            if(snfd_check_block(snfd, i))
            {
                snfd_initialize_block(snfd, i);
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
 */
SNFD_UINT32 snfd_find_free_log_in_block(SNFD * snfd, SNFD_UINT16 block_number)
{
    SNFD_UINT32 next_log_loc = (block_number * SNFD_BLOCK_SIZE) + sizeof(SNFD_BLOCK_HEADER);
    SNFD_LOG log;
    while(next_log_loc < SNFD_BLOCK_SIZE)
    {
        snfd_direct_read(snfd, next_log_loc, &log, sizeof(log));
        if(log.file_number == 0xFFFF)
        {
            return next_log_loc;
        }
        next_log_loc += log.data_size + sizeof(log);
    }
    return 0;
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
         * Check if this location is better
         */
        if(free_size >= size && (last_free_size - size) <= free_size)
        {
            last_free_size = free_size;
            write_loc = tmp_write_loc;
        }
    }
    SNFD_LOG log;
    log.file_number = file_nr;
    log.file_operation = SNFD_LOG_OPERATION_SET;
    log.flags = SNFD_LOG_ACTIVE;
    log.start_loc = destination;
    log.data_size = size;
    log.next_log = 0;

    snfd_direct_write(snfd, write_loc, &log, sizeof(log));
    snfd_direct_write(snfd, write_loc + sizeof(log), source, size);
    // TODO: check if write succeded

    // TODO:
}
