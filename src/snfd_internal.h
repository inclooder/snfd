#ifndef SNFD_INTERNAL_H
#define SNFD_INTERNAL_H

#include "snfd_types.h"

void snfd_initialize_block(SNFD * snfd, SNFD_UINT16 block_number);
SNFD_BOOL snfd_check_block(SNFD * snfd, SNFD_UINT16 block_number);
SNFD_BOOL snfd_check_block_pattern(SNFD * snfd, SNFD_UINT16 block_number, const void * pattern, SNFD_UINT16 pattern_size);
void snfd_write_block_pattern(SNFD * snfd, SNFD_UINT16 block_number, const void * pattern, SNFD_UINT16 pattern_size);

SNFD_UINT32 snfd_calc_read_size(SNFD_UINT32 read_buffer_size, SNFD_UINT32 block_size, SNFD_UINT32 offset);

SNFD_ERROR snfd_direct_write(SNFD * snfd, SNFD_UINT32 destination, void * source, SNFD_UINT32 size);
SNFD_ERROR snfd_direct_read(SNFD * snfd, SNFD_UINT32 source, void * destination, SNFD_UINT32 size);
SNFD_ERROR snfd_direct_block_erase(SNFD * snfd, SNFD_UINT16 block_number);


#endif /* end of include guard: SNFD_INTERNAL_H */
