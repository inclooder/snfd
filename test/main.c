#include "unity.h"
#include "test_snfd.h"
#include "test_snfd_internal.h"

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_snfd_are_blocks_initialized_1);
    RUN_TEST(test_snfd_are_blocks_initialized_2);
    RUN_TEST(test_snfd_calc_read_size_1);
    RUN_TEST(test_snfd_calc_read_size_2);
    RUN_TEST(test_snfd_calc_read_size_3);
    RUN_TEST(test_snfd_calc_read_size_4);
    RUN_TEST(test_snfd_check_block_pattern);
    RUN_TEST(test_snfd_write_block_pattern);
    RUN_TEST(test_snfd_check_block);
    return UNITY_END();
}
