#include "snfd_internal.h"

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
}
