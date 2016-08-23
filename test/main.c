#include "unity.h"
#include "test_snfd.h"

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_snfd_startup);
    return UNITY_END();
}
