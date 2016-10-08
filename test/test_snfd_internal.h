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
    flash = nfe_create_flash(256, 4096);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;

    SNFD_UINT8 pattern[2] = { 0x01, 0x02 };
    snfd_write_block_pattern(&snfd, 2, pattern, sizeof(pattern));

    SNFD_BOOL result = snfd_check_block_pattern(&snfd, 2, pattern, sizeof(pattern));
    TEST_ASSERT_EQUAL(SNFD_TRUE, result);

    nfe_destroy_flash(flash);
}

void test_snfd_write_block_pattern(void)
{
    flash = nfe_create_flash(256, 4096);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;


    SNFD_UINT8 pattern[2] = { 0x32, 0xF1 };
    snfd_write_block_pattern(&snfd, 5, pattern, sizeof(pattern));
    SNFD_BOOL result = snfd_check_block_pattern(&snfd, 5, pattern, sizeof(pattern));
    TEST_ASSERT_EQUAL(SNFD_TRUE, result);

    nfe_destroy_flash(flash);
}

void test_snfd_check_block(void)
{
    flash = nfe_create_flash(256, 4096);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;

    SNFD_BOOL result = snfd_check_block(&snfd, 5);
    TEST_ASSERT_EQUAL(SNFD_TRUE, result);

    nfe_destroy_flash(flash);
}

void test_snfd_log_find_prev_1(void)
{
    flash = nfe_create_flash(256, 4096);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd_startup(&snfd);

    char * data = "ABCDEFGHIJK";
    SNFD_UINT32 log_loc;
    log_loc = snfd_find_space_for_new_log(&snfd, strlen(data));
    snfd_log_create(&snfd, 3, 0, strlen(data), log_loc);
    SNFD_UINT32 prev_log;
    prev_log = snfd_log_find_prev(&snfd, 3, 1);
    TEST_ASSERT_EQUAL(0, prev_log);

    log_loc = snfd_find_space_for_new_log(&snfd, strlen(data));
    snfd_log_create(&snfd, 3, 0, strlen(data), log_loc);
    prev_log = snfd_log_find_prev(&snfd, 3, 2);
    TEST_ASSERT_EQUAL(16, prev_log);

    log_loc = snfd_find_space_for_new_log(&snfd, strlen(data));
    snfd_log_create(&snfd, 3, 0, strlen(data), log_loc);
    prev_log = snfd_log_find_prev(&snfd, 3, 3);
    TEST_ASSERT_EQUAL(43, prev_log);

    prev_log = snfd_log_find_prev(&snfd, 3, 2);
    TEST_ASSERT_EQUAL(16, prev_log);

    nfe_dump_to_file(flash, "/tmp/flashdump");

    snfd_cleanup(&snfd);
    nfe_destroy_flash(flash);
}

#endif /* end of include guard: TEST_SNFD_INTERNAL_H */
