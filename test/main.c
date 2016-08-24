#include "unity.h"
#include "test_snfd.h"

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_snfd_are_blocks_initialized_1);
    RUN_TEST(test_snfd_are_blocks_initialized_2);
    return UNITY_END();
}
