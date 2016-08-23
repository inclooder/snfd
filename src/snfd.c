#include "snfd.h"


SNFD_ERROR snfd_set_direct_write_func(SNFD_DEVICE * device, SNFD_DIRECT_WRITE_FUNC write_func)
{
    device->direct_write_func = write_func;
}


