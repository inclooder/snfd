#include "snfd.h"
#include <stdlib.h>
#include <string.h>
#include "snfd_internal.h"

SNFD_ERROR snfd_startup(SNFD * snfd)
{
    SNFD_UINT32 i;
    for(i = 0; i < SNFD_BLOCKS_COUNT; ++i)
    {
        SNFD_BLOCK_HEADER header;
        snfd_read_block_header(snfd, i, &header);
        if(!snfd_is_block_initialized(snfd, i) ||
                snfd_is_block_broken(snfd, i))
        {
            snfd_is_block_initialized(snfd, i);
        }
    }
    return SNFD_ERROR_NO_ERROR;
}

void snfd_cleanup(SNFD * snfd)
{
}

