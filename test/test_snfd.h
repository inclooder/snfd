#ifndef TEST_SNFD_H
#define TEST_SNFD_H

#include "unity.h"
#include "snfd.h"


void test_add_should_ReturnCorrectSum(void){
	TEST_ASSERT_EQUAL_INT32(54, add(31, 23));
}



#endif /* end of include guard: TEST_SNFD_H */
