#include "snfd.h"
#include <stdlib.h>
#include <string.h>


SNFD_ERROR snfd_startup(SNFD * snfd)
{
    if(snfd_are_blocks_initialized(snfd))
    {
    }
    return SNFD_ERROR_NO_ERROR;
}

void snfd_cleanup(SNFD * snfd)
{
}

