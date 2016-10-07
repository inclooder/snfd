#ifndef SNFD_UTILS_H
#define SNFD_UTILS_H

#include "snfd_types.h"

typedef struct {
    SNFD_UINT16 begin;
    SNFD_UINT16 end;
} SNFD_SEGMENT;

typedef struct {
    SNFD_UINT16 size;
    SNFD_INT32 int_start_a;
    SNFD_INT32 int_end_a;
    SNFD_INT32 int_start_b;
    SNFD_INT32 int_end_b;
} SNFD_SEGMENT_INTERSECTION;

void snfd_check_segment_intersection(
        const SNFD_SEGMENT * a,
        const SNFD_SEGMENT * b,
        SNFD_SEGMENT_INTERSECTION * inter
);

#endif /* end of include guard: SNFD_UTILS_H */
