#ifndef SNFD_INTERNAL_H
#define SNFD_INTERNAL_H

#include "snfd_types.h"

SNFD_BOOL snfd_is_block_broken(SNFD * snfd, SNFD_BLOCK_HEADER * header);
SNFD_BOOL snfd_is_block_free(SNFD * snfd, SNFD_BLOCK_HEADER * header);
SNFD_BOOL snfd_is_block_clean(SNFD * snfd, SNFD_BLOCK_HEADER * header);
SNFD_BOOL snfd_is_block_dirty(SNFD * snfd, SNFD_BLOCK_HEADER * header);
SNFD_BOOL snfd_is_block_initialized(SNFD * snfd,
                                    SNFD_BLOCK_HEADER * header);

void snfd_read_block_header(SNFD * snfd, 
                            SNFD_BLOCK_NUMBER block_number,
                            SNFD_BLOCK_HEADER * header);

void snfd_erase_and_initialize_block(SNFD * snfd, SNFD_BLOCK_NUMBER block_number);
SNFD_BOOL snfd_check_block(SNFD * snfd, SNFD_BLOCK_NUMBER block_number);
SNFD_BOOL snfd_check_block_pattern(SNFD * snfd, 
                                   SNFD_BLOCK_NUMBER block_number,
                                   const void * pattern,
                                   SNFD_UINT16 pattern_size);

SNFD_BOOL snfd_log_is_invalid(SNFD_LOG * log);

void snfd_garbage_collect(SNFD * snfd);

void snfd_write_block_pattern(SNFD * snfd, 
                              SNFD_BLOCK_NUMBER block_number,
                              const void * pattern, 
                              SNFD_UINT16 pattern_size);

SNFD_UINT32 snfd_calc_read_size(SNFD_UINT32 read_buffer_size,
                                SNFD_UINT32 block_size,
                                SNFD_UINT32 offset);

SNFD_ERROR snfd_direct_write(SNFD * snfd,
                             SNFD_UINT32 destination,
                             void * source,
                             SNFD_UINT32 size);

SNFD_ERROR snfd_direct_read(SNFD * snfd,
                            SNFD_UINT32 source,
                            void * destination,
                            SNFD_UINT32 size);

SNFD_ERROR snfd_direct_block_erase(SNFD * snfd,
                                   SNFD_BLOCK_NUMBER block_number);

SNFD_UINT16 snfd_calc_block_number_from_physical_addr(SNFD_UINT32 physical_addr);
SNFD_UINT32 snfd_find_free_log_in_block(SNFD * snfd, SNFD_BLOCK_NUMBER block_number);
SNFD_UINT32 snfd_find_space_for_new_log(SNFD * snfd, SNFD_UINT32 size);

SNFD_ERROR snfd_block_state_change(SNFD * snfd,
                                   SNFD_BLOCK_NUMBER block_nr,
                                   SNFD_BLOCK_STATE new_state);

SNFD_BOOL snfd_block_is_state_change_allowed(SNFD_BLOCK_STATE old_state, 
                                             SNFD_BLOCK_STATE new_state);

SNFD_BOOL snfd_block_has_logs(SNFD * snfd, SNFD_BLOCK_NUMBER block_nr);

#endif /* end of include guard: SNFD_INTERNAL_H */
