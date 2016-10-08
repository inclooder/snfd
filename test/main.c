#include "unity.h"
#include "test_snfd.h"
#include "test_snfd_internal.h"
#include "test_snfd_utils.h"

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_snfd_calc_read_size_2);
    RUN_TEST(test_snfd_calc_read_size_3);
    RUN_TEST(test_snfd_calc_read_size_4);
    RUN_TEST(test_snfd_check_block_pattern);
    RUN_TEST(test_snfd_write_block_pattern);
    RUN_TEST(test_snfd_check_block);
    RUN_TEST(test_snfd_file_write_1);
    RUN_TEST(test_snfd_file_write_2);
    RUN_TEST(test_snfd_file_write_3);
    RUN_TEST(test_snfd_file_write_4);
    RUN_TEST(test_snfd_file_write_5);
    RUN_TEST(test_snfd_file_read_1);
    RUN_TEST(test_snfd_file_calc_size);
    RUN_TEST(test_snfd_check_segment_intersection_1);
    RUN_TEST(test_snfd_check_segment_intersection_2);
    RUN_TEST(test_snfd_check_segment_intersection_3);
    RUN_TEST(test_snfd_check_segment_intersection_4);
    RUN_TEST(test_snfd_check_segment_intersection_5);
    RUN_TEST(test_snfd_check_segment_intersection_6);
    RUN_TEST(test_snfd_log_find_prev_1);
    return UNITY_END();
}
