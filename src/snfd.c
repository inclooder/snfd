#include "snfd.h"
#include <stdlib.h>
#include <string.h>


SNFD * snfd_startup(SNFD_CONFIG * config)
{
    SNFD * snfd = (SNFD *)malloc(sizeof(SNFD));
    memcpy(&snfd->config, config, sizeof(SNFD_CONFIG));
    return snfd;
}

void snfd_cleanup(SNFD * snfd)
{
    free(snfd);
}

