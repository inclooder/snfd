#ifndef MFFS_H
#define MFFS_H

#include "snfd_types.h"

SNFD_ERROR snfd_startup(SNFD * snfd);

SNFD_ERROR snfd_write_file(SNFD * snfd, 
                           SNFD_FILE_NUMBER file_nr, 
                           SNFD_UINT32 destination, 
                           void * source, SNFD_UINT32 size);

SNFD_ERROR snfd_read_file(SNFD * snfd,
                          SNFD_FILE_NUMBER file_nr,
                          SNFD_UINT32 source,
                          void * destination,
                          SNFD_UINT32 size);

SNFD_UINT32 snfd_file_calc_size(SNFD * snfd, SNFD_FILE_NUMBER file_nr);


void snfd_cleanup(SNFD * snfd);


#endif /* end of include guard: MFFS_H */
