#include "snfd.h"

SNFD_ERROR snfd_direct_write(SNFD * snfd, 
                             SNFD_UINT32 destination, 
                             void * source, 
                             SNFD_UINT32 size)
{
    return snfd->config.write_func(destination, source, size);
}

SNFD_ERROR snfd_direct_read(SNFD * snfd, 
                            SNFD_UINT32 source, 
                            void * destination,
                            SNFD_UINT32 size)
{
    return snfd->config.read_func(source, destination, size);
}

SNFD_ERROR snfd_direct_block_erase(SNFD * snfd,
                                   SNFD_BLOCK_NUMBER block_number)
{
    return snfd->config.block_erase_func(block_number);
}
