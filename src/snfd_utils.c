#include "snfd_utils.h"

void snfd_check_segment_intersection(
        const SNFD_SEGMENT * a,
        const SNFD_SEGMENT * b,
        SNFD_SEGMENT_INTERSECTION * inter
)
{
    if(a->begin >= b->begin)
    {
        if(a->end > b->end) 
        {
            inter->size = b->end - a->begin;
            inter->int_end_b = b->end;
        }
        else
        {
            inter->size = a->end - a->begin;
            inter->int_end_b = a->end - b->begin;
        }

        inter->int_start_a = 0;
        inter->int_end_a = inter->size;
        inter->int_start_b = a->begin - b->begin;
    }
    else if(b->begin > a->begin && a->end > b->end)
    {
        inter->size = b->end - b->begin;
        inter->int_start_a = inter->size;
        inter->int_end_a = b->end - a->begin;
        inter->int_start_b = 0;
        inter->int_end_b = inter->size;
    } 
    else
    {
        inter->size = a->end - b->begin;
        inter->int_start_a = b->begin;
        inter->int_end_a = a->end;
        inter->int_start_b = a->begin;
        inter->int_end_b = a->end - b->begin;
    }
}
