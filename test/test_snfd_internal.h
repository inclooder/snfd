#ifndef TEST_SNFD_INTERNAL_H

#define TEST_SNFD_INTERNAL_H

#include "unity.h"
#include "snfd.h"
#include "nfe.h"
#include "test_support.h"


void test_snfd_calc_read_size_1(void)
{
    SNFD_UINT32 result = snfd_calc_read_size(100, 50, 0);
    TEST_ASSERT_EQUAL(50, result);
}

void test_snfd_calc_read_size_2(void)
{
    SNFD_UINT32 result = snfd_calc_read_size(100, 101, 0);
    TEST_ASSERT_EQUAL(100, result);
}

void test_snfd_calc_read_size_3(void)
{
    SNFD_UINT32 result = snfd_calc_read_size(100, 101, 2);
    TEST_ASSERT_EQUAL(99, result);
}

void test_snfd_calc_read_size_4(void)
{
    SNFD_UINT32 result = snfd_calc_read_size(100, 141, 2);
    TEST_ASSERT_EQUAL(100, result);
}

void test_snfd_check_block_pattern(void)
{
    flash = nfe_create_flash(10, 10);
    nfe_clear_flash(flash, 0xFF);

    SNFD_UINT8 memory[10] = { 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02 };
    nfe_write(flash, 20, memory, 10);
    SNFD_UINT8 pattern[2] = { 0x01, 0x02 };

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd.config.block_count = 10;
    snfd.config.block_size = 10;

    SNFD_BOOL result = snfd_check_block_pattern(&snfd, 2, pattern, sizeof(pattern));
    TEST_ASSERT_EQUAL(SNFD_TRUE, result);

    nfe_destroy_flash(flash);
}

void test_snfd_write_block_pattern(void)
{
    flash = nfe_create_flash(10, 10);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd.config.block_count = 10;
    snfd.config.block_size = 10;


    SNFD_UINT8 pattern[2] = { 0x32, 0xF1 };
    snfd_write_block_pattern(&snfd, 5, pattern, sizeof(pattern));
    SNFD_BOOL result = snfd_check_block_pattern(&snfd, 5, pattern, sizeof(pattern));
    TEST_ASSERT_EQUAL(SNFD_TRUE, result);

    nfe_destroy_flash(flash);
}

void test_snfd_check_block(void)
{
    flash = nfe_create_flash(10, 10);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd.config.block_count = 10;
    snfd.config.block_size = 10;

    SNFD_BOOL result = snfd_check_block(&snfd, 5);
    TEST_ASSERT_EQUAL(SNFD_TRUE, result);

    nfe_destroy_flash(flash);
}

#endif /* end of include guard: TEST_SNFD_INTERNAL_H */
