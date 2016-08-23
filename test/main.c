#include "unity.h"
#include "test_snfd.h"

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_snfd_set_direct_write_func);
    return UNITY_END();
}
