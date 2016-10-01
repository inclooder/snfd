#include "snfd_internal.h"
#include <string.h>
#include <stdio.h>

/*
 * Erase and initialize block.
 */
void snfd_erase_and_initialize_block(SNFD * snfd, SNFD_UINT16 block_number)
{
    snfd_direct_block_erase(snfd, block_number);
    SNFD_BLOCK_HEADER header;
    memcpy(header.magic_number, 
           SNFD_MAGIC_NUMBER,
           sizeof(header.magic_number));

    header.state = SNFD_BLOCK_FREE;

    snfd_direct_write(snfd, block_number * SNFD_BLOCK_SIZE,
                      &header, sizeof(header));
}

/*
 * Writes a pattern to the block
 */
void snfd_write_block_pattern(SNFD * snfd, 
                              SNFD_UINT16 block_number, 
                              const void * pattern, 
                              SNFD_UINT16 pattern_size)
{
    SNFD_UINT32 current_pattern = 0;
    SNFD_UINT32 i;
    for(i = 0; i < SNFD_BLOCK_SIZE;) // For each byte in the block
    {
        // Calc how much memory to write
        SNFD_UINT32 write_size = snfd_calc_read_size(
                sizeof(snfd->buffer), 
                SNFD_BLOCK_SIZE, i
        );
        // Calc write location
        SNFD_UINT32 write_loc = (block_number * SNFD_BLOCK_SIZE) + i;
        SNFD_UINT32 j;
        for(j = 0; j < write_size; ++j) // Write each byte to the buffer
        {
            SNFD_UINT8 byte = ((SNFD_UINT8 *)pattern)[current_pattern];
            snfd->buffer[j] = byte;
             // Set next pattern
            current_pattern = (current_pattern + 1) % pattern_size;
        }
        // Write prepared buffer to the flash memory
        snfd_direct_write(snfd, write_loc, snfd->buffer, write_size);
        i += write_size; // Move forward by number of bytes writen
    }
}

/*
 * Checks if block follows specific pattern
 */
SNFD_BOOL snfd_check_block_pattern(SNFD * snfd, 
                                   SNFD_UINT16 block_number, 
                                   const void * pattern, 
                                   SNFD_UINT16 pattern_size)
{
    SNFD_UINT32 current_pattern = 0;
    SNFD_UINT32 i;
    for(i = 0; i < SNFD_BLOCK_SIZE;)
    {
        SNFD_UINT32 read_size = snfd_calc_read_size(
                sizeof(snfd->buffer), 
                SNFD_BLOCK_SIZE, i
        );
        SNFD_UINT32 read_loc = (block_number * SNFD_BLOCK_SIZE) + i;
        snfd_direct_read(snfd, read_loc, snfd->buffer, read_size);
        SNFD_UINT32 j;
        for(j = 0; j < read_size; ++j)
        {
            SNFD_UINT8 expected_byte = ((SNFD_UINT8 *)pattern)[current_pattern];
            if(snfd->buffer[j] != expected_byte) {
                return SNFD_FALSE;
            }
            current_pattern = (current_pattern + 1) % pattern_size;
        }
        i += read_size;
    }
    return SNFD_TRUE;
}

/*
 * Erases a block, checks if every byte is 0xFF and then writes some pattern then checks again.
 */
SNFD_BOOL snfd_check_block(SNFD * snfd, SNFD_UINT16 block_number)
{
    snfd_direct_block_erase(snfd, block_number);
    const SNFD_UINT8 clean_block_pattern[1] = { 0xFF };
    SNFD_BOOL result = snfd_check_block_pattern(
            snfd, 
            block_number, 
            clean_block_pattern,
            sizeof(clean_block_pattern)
    );
    if(result != SNFD_TRUE) return SNFD_FALSE;

    const SNFD_UINT8 pattern[4] = { 0x5A, 0x55, 0x5A, 0x55 };
    snfd_write_block_pattern(snfd, block_number, pattern, sizeof(pattern));
    return snfd_check_block_pattern(snfd, block_number, pattern, sizeof(pattern));
}

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
                            SNFD_UINT16 block_number, 
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

/*
 * Find and mark blocks as deprecated
 */
void snfd_garbage_collect(SNFD * snfd)
{
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
                                        SNFD_UINT16 block_number)
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
        /*
         * Check only clean and free blocks, skip others.
         */
        if(state != SNFD_BLOCK_CLEAN && state != SNFD_BLOCK_FREE) continue;
        tmp_write_loc = snfd_find_free_log_in_block(snfd, i);
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
