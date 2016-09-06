#ifndef SUPPORT_H
#define SUPPORT_H

#include "snfd.h"
#include "nfe.h"

NFE_FLASH * flash;

SNFD_ERROR flash_write_func (SNFD_UINT32 destination, void * source, SNFD_UINT32 size)
{

    NFE_ERROR error = nfe_write(flash, destination, source, size);
    if(error != NFE_ERROR_NO_ERROR) TEST_FAIL_MESSAGE("flash write error");
    
}

SNFD_ERROR flash_read_func (SNFD_UINT32 source, void * destination, SNFD_UINT32 size)
{
    NFE_ERROR error = nfe_read(flash, destination, source, size);
    if(error != NFE_ERROR_NO_ERROR) TEST_FAIL_MESSAGE("flash read error");
}

SNFD_ERROR flash_block_erase_func (SNFD_UINT16 block_number)
{
    NFE_ERROR error = nfe_erase_block(flash, block_number);
    if(error != NFE_ERROR_NO_ERROR) TEST_FAIL_MESSAGE("flash block erase error");
}

#endif /* end of include guard: SUPPORT_H */
