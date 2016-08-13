#include "unity.h"
#include "test_snfd.h"

int main(){
	UNITY_BEGIN();
	RUN_TEST(test_add_should_ReturnCorrectSum);
	return UNITY_END();
}
