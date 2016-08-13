#ifndef TEST_SNFD_H
#define TEST_SNFD_H

#include "unity.h"
#include "snfd.h"

SNFD_ERROR my_direct_write_func
(
		SNFD_UINT32 destination, 
		SNFD_UINT8 * source_buffer, 
		SNFD_UINT32 count
){}

void test_snfd_set_direct_write_func(void){
	SNFD_DEVICE my_device;
	snfd_set_direct_write_func(&my_device, &my_direct_write_func);
	TEST_ASSERT_EQUAL(&my_direct_write_func, my_device.direct_write_func);
}




#endif /* end of include guard: TEST_SNFD_H */
