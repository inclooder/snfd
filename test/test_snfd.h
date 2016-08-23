#ifndef TEST_SNFD_H
#define TEST_SNFD_H

#include "unity.h"
#include "snfd.h"


SNFD_ERROR my_direct_write_func(SNFD_UINT32 destination, SNFD_UINT8 * source, SNFD_UINT32 size)
{
}

SNFD_ERROR my_direct_read_func(SNFD_UINT32 source, SNFD_UINT8 * destination, SNFD_UINT32 size)
{
}

SNFD_ERROR my_direct_block_erase_func(SNFD_UINT16 block_number)
{
}

void test_snfd_startup(void){
    SNFD_CONFIG config;
    config.write_func = my_direct_write_func;
    config.read_func = my_direct_read_func;
    config.block_erase_func = my_direct_block_erase_func;
    SNFD * snfd = snfd_startup(&config);
    TEST_ASSERT_EQUAL(snfd->config.write_func, my_direct_write_func);
    TEST_ASSERT_EQUAL(snfd->config.read_func, my_direct_read_func);
    TEST_ASSERT_EQUAL(snfd->config.block_erase_func, my_direct_block_erase_func);
    snfd_cleanup(snfd);
}

#endif /* end of include guard: TEST_SNFD_H */
