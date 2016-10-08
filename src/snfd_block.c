#include "snfd_internal.h"
#include "string.h"

SNFD_BOOL snfd_block_is_state_change_allowed(SNFD_BLOCK_STATE old_state, 
                                             SNFD_BLOCK_STATE new_state)
{
    if(new_state == SNFD_BLOCK_BROKEN) return SNFD_TRUE;

    if((old_state == SNFD_BLOCK_BROKEN) ||
       (old_state == SNFD_BLOCK_NOT_INITIALIZED && new_state != SNFD_BLOCK_FREE) ||
       (old_state == SNFD_BLOCK_FREE && new_state != SNFD_BLOCK_CLEAN) ||
       (old_state == SNFD_BLOCK_CLEAN && new_state != SNFD_BLOCK_DIRTY) ||
       (old_state == SNFD_BLOCK_DIRTY && new_state != SNFD_BLOCK_NOT_INITIALIZED))
    {
        return SNFD_FALSE;
    }
    return SNFD_TRUE;
}

SNFD_ERROR snfd_block_state_change(SNFD * snfd,
                                   SNFD_BLOCK_NUMBER block_nr,
                                   SNFD_BLOCK_STATE new_state)
{
    SNFD_BLOCK_STATE old_state = snfd->blocks[block_nr].state;
    if(!snfd_block_is_state_change_allowed(old_state, new_state))
    {
        return SNFD_ERROR_BLOCK_STATE_CHANGE_NOT_ALLOWED;
    }
    SNFD_BLOCK_HEADER block_header;
    snfd_read_block_header(snfd, block_nr, &block_header);
    block_header.state = new_state;
    snfd_direct_write(snfd, block_nr * SNFD_BLOCK_SIZE,
                      &block_header,
                      sizeof(block_header));
    snfd->blocks[block_nr].state = new_state;
    return SNFD_ERROR_NO_ERROR;
}

/*
 * Erase and initialize block.
 */
void snfd_erase_and_initialize_block(SNFD * snfd, SNFD_BLOCK_NUMBER block_number)
{
    snfd_direct_block_erase(snfd, block_number);
    SNFD_BLOCK_HEADER header;
    memcpy(header.magic_number, 
           SNFD_MAGIC_NUMBER,
           sizeof(header.magic_number));

    header.state = SNFD_BLOCK_FREE;

    snfd_direct_write(snfd, block_number * SNFD_BLOCK_SIZE,
                      &header, sizeof(header));
    snfd->blocks[block_number].state = SNFD_BLOCK_FREE;
}

/*
 * Writes a pattern to the block
 */
void snfd_write_block_pattern(SNFD * snfd, 
                              SNFD_BLOCK_NUMBER block_number, 
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
                                   SNFD_BLOCK_NUMBER block_number, 
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
SNFD_BOOL snfd_check_block(SNFD * snfd, SNFD_BLOCK_NUMBER block_number)
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
