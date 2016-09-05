#include "snfd_internal.h"
#include <string.h>
#include <stdio.h>

/*
 * Go trough entire flash and check if are there blocks that start with the magic number.
 */
SNFD_BOOL snfd_are_blocks_initialized(SNFD * snfd)
{
    unsigned char buff[4] = { 'T', 'E', 'S', 'T' };
    unsigned char read_buff[4] = { 0 };
    snfd->config.read_func(0, read_buff, 4);
    if(memcmp(buff, read_buff, 4) == 0){
        return SNFD_TRUE;
    } else {
        return SNFD_FALSE;
    }
}


SNFD_BOOL snfd_initialize_blocks(SNFD * snfd)
{
    char buff[4] = { 'T', 'E', 'S', 'T' };
    snfd->config.write_func(0, buff, 4);
}

/*
 * Writes a pattern to the block
 */
void snfd_write_block_pattern(SNFD * snfd, SNFD_UINT16 block_number, const void * pattern, SNFD_UINT16 pattern_size)
{
    SNFD_UINT32 current_pattern = 0;
    SNFD_UINT32 i;
    for(i = 0; i < snfd->config.block_size;)
    {
        SNFD_UINT32 write_size = snfd_calc_read_size(sizeof(snfd->buffer), snfd->config.block_size, i);
        SNFD_UINT32 write_loc = (block_number * snfd->config.block_size) + i;
        SNFD_UINT32 j;
        for(j = 0; j < write_size; ++j)
        {
            SNFD_UINT8 byte = ((SNFD_UINT8 *)pattern)[current_pattern];
            snfd->buffer[j] = byte;
            current_pattern = (current_pattern + 1) % pattern_size;
        }
        snfd->config.write_func(write_loc, snfd->buffer, write_size);
        i += write_size;
    }
}

/*
 * Checks if block follows specific pattern
 */
SNFD_BOOL snfd_check_block_pattern(SNFD * snfd, SNFD_UINT16 block_number, const void * pattern, SNFD_UINT16 pattern_size)
{
    SNFD_UINT32 current_pattern = 0;
    SNFD_UINT32 i;
    for(i = 0; i < snfd->config.block_size;)
    {
        SNFD_UINT32 read_size = snfd_calc_read_size(sizeof(snfd->buffer), snfd->config.block_size, i);
        SNFD_UINT32 read_loc = (block_number * snfd->config.block_size) + i;
        snfd->config.read_func(read_loc, snfd->buffer, read_size);
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
    snfd->config.block_erase_func(block_number);
    const SNFD_UINT8 clean_block_pattern[1] = { 0xFF };
    SNFD_BOOL result = snfd_check_block_pattern(snfd, block_number, clean_block_pattern, sizeof(clean_block_pattern));
    if(result != SNFD_TRUE) return SNFD_FALSE;

    const SNFD_UINT8 pattern[4] = { 0x5A, 0x55, 0x5A, 0x55 };
    snfd_write_block_pattern(snfd, block_number, pattern, sizeof(pattern));
    return snfd_check_block_pattern(snfd, block_number, pattern, sizeof(pattern));
}

SNFD_UINT32 snfd_calc_read_size(SNFD_UINT32 read_buffer_size, SNFD_UINT32 block_size, SNFD_UINT32 offset)
{
    block_size = block_size - offset;
    SNFD_UINT32 read_size = read_buffer_size;
    if(read_size > block_size) read_size = block_size;
    return read_size;
}
