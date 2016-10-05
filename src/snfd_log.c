#include "snfd_internal.h"

SNFD_ERROR snfd_log_read(SNFD * snfd, SNFD_UINT32 source, SNFD_LOG * log)
{
    return snfd_direct_read(snfd, source, log, sizeof(SNFD_LOG));
}
