#ifndef TEST_SNFD_H
#define TEST_SNFD_H

#include "unity.h"
#include "snfd.h"
#include "nfe.h"
#include "test_support.h"

/*
 * Simple read and write
 */
void test_snfd_file_write_1(void)
{
    flash = nfe_create_flash(256, 4096);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd_startup(&snfd);

    char file_content[] = { 'T', 'E', 'S', 'T' };
    snfd_write_file(&snfd, 10, 0, file_content, sizeof(file_content));

    char read_buff[sizeof(file_content)] = { 0 };
    snfd_read_file(&snfd, 10, 0, read_buff, sizeof(read_buff));

    TEST_ASSERT_EQUAL_MEMORY(file_content, read_buff, sizeof(file_content));

    snfd_cleanup(&snfd);
    nfe_destroy_flash(flash);
}

/*
 * Write multiple times to the same file
 */
void test_snfd_file_write_2(void)
{
    flash = nfe_create_flash(256, 4096);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd_startup(&snfd);

    char file_content[] = { 'T', 'E', 'S', 'T', 'A', 'B', 'C', 'D', 'E' };
    snfd_write_file(&snfd, 10, 0, file_content, 4);
    snfd_write_file(&snfd, 10, 4, file_content + 4, 5);

    char read_buff[sizeof(file_content)] = { 0 };
    snfd_read_file(&snfd, 10, 0, read_buff, sizeof(read_buff));

    TEST_ASSERT_EQUAL_MEMORY(file_content, read_buff, sizeof(file_content));

    snfd_cleanup(&snfd);
    nfe_destroy_flash(flash);
}

/*
 * Write multiple times with intersections
 */
void test_snfd_file_write_3(void)
{
    flash = nfe_create_flash(256, 4096);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd_startup(&snfd);

    char expected_content[] = { 'E', 'X', 'P', 'E', 'f', 'o', 'E', 'D', '6' };

    char file_content1[] = { 'E', 'X', 'P', '1', '2', '3', '4', '5', '6' };
    char file_content2[] = { 'E', 'C', 'T', 'E', 'D' };
    char file_content3[] = { 'f', 'o' };
    snfd_write_file(&snfd, 10, 0, file_content1, sizeof(file_content1));
    snfd_write_file(&snfd, 10, 3, file_content2, sizeof(file_content2));
    snfd_write_file(&snfd, 10, 4, file_content3, sizeof(file_content3));

    char read_buff[sizeof(expected_content)] = { 0 };
    snfd_read_file(&snfd, 10, 0, read_buff, sizeof(read_buff));
    TEST_ASSERT_EQUAL_MEMORY(expected_content, read_buff, sizeof(expected_content));

    snfd_cleanup(&snfd);
    nfe_destroy_flash(flash);
}

#endif /* end of include guard: TEST_SNFD_H */
