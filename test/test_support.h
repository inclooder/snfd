#ifndef SUPPORT_H
#define SUPPORT_H

#include "snfd.h"
#include "nfe.h"

NFE_FLASH * flash;

SNFD_ERROR flash_write_func (SNFD_UINT32 destination, void * source, SNFD_UINT32 size)
{
    nfe_write(flash, destination, source, size);
}

SNFD_ERROR flash_read_func (SNFD_UINT32 source, void * destination, SNFD_UINT32 size)
{
    nfe_read(flash, destination, source, size);
}

SNFD_ERROR flash_block_erase_func (SNFD_UINT16 block_number)
{
    nfe_erase_block(flash, block_number);
}

#endif /* end of include guard: SUPPORT_H */
