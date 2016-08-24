#include "snfd_internal.h"
#include <string.h>

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
