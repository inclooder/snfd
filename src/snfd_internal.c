#include "snfd_internal.h"
#include <string.h>
#include <stdio.h>

/*
 * Initialize an empty block, you need to erase it first.
 */
void snfd_initialize_block(SNFD * snfd, SNFD_UINT16 block_number)
{
    // Write magic number at the beginning.
    snfd->config.write_func(block_number * snfd->config.block_size, SNFD_MAGIC_NUMBER, SNFD_MAGIC_NUMBER_SIZE);

    SNFD_UINT32 bytes_writen = 0;
    bytes_writen += SNFD_MAGIC_NUMBER_SIZE;

    SNFD_UINT32 block_free_status = SNFD_BLOCK_FREE;

    // Set block state as FREE
    snfd->config.write_func(
            (block_number * snfd->config.block_size) + bytes_writen,
            &block_free_status,
            sizeof(block_free_status)
    );
}

/*
 * Writes a pattern to the block
 */
void snfd_write_block_pattern(SNFD * snfd, SNFD_UINT16 block_number, const void * pattern, SNFD_UINT16 pattern_size)
{
    SNFD_UINT32 current_pattern = 0;
    SNFD_UINT32 i;
    for(i = 0; i < snfd->config.block_size;) // For each byte in the block
    {
        SNFD_UINT32 write_size = snfd_calc_read_size(sizeof(snfd->buffer), snfd->config.block_size, i); // Calc how much memory to write
        SNFD_UINT32 write_loc = (block_number * snfd->config.block_size) + i; // Calc write location
        SNFD_UINT32 j;
        for(j = 0; j < write_size; ++j) // Write each byte to the buffer
        {
            SNFD_UINT8 byte = ((SNFD_UINT8 *)pattern)[current_pattern];
            snfd->buffer[j] = byte;
            current_pattern = (current_pattern + 1) % pattern_size; // Set next pattern
        }
        snfd->config.write_func(write_loc, snfd->buffer, write_size); // Write prepared buffer to the flash memory
        i += write_size; // Move forward by number of bytes writen
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