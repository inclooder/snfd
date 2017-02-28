#ifndef TEST_SNFD_H
#define TEST_SNFD_H

#include "unity.h"
#include "snfd.h"
#include "nfe.h"
#include "test_support.h"
#include "string.h"

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

/*
 * Write data till free space is exhausted.
 */
void test_snfd_file_write_4(void)
{
    flash = nfe_create_flash(256, 4096);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd_startup(&snfd);

    char data[4000];

    SNFD_ERROR error;
    SNFD_UINT32 i;
    SNFD_UINT32 j;
    for(j = 0; j < 256; ++j)
    {
        for(i = 0; i < 4000; ++i)
        {
            data[i] = 'a' + j;
        }
        error = snfd_write_file(&snfd, 10 + j, 0, data, sizeof(data));
        TEST_ASSERT_EQUAL_MESSAGE(SNFD_ERROR_NO_ERROR, error, "shouldn't return error");
    }
    error = snfd_write_file(&snfd, 3, 0, data, sizeof(data));
    TEST_ASSERT_EQUAL_MESSAGE(SNFD_ERROR_NO_SPACE_LEFT, error, "should return ERROR_NO_SPACE_LEFT");

    snfd_cleanup(&snfd);
    nfe_destroy_flash(flash);
}

/*
 * Write data to the same file and location till free space is exhausted.
 */
void test_snfd_file_write_5(void)
{
    flash = nfe_create_flash(256, 4096);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd_startup(&snfd);

    char data[4000];
    SNFD_UINT32 i;
    for(i = 0; i < 4000; ++i)
    {
        data[i] = 0xCC;
    }

    SNFD_ERROR error;
    SNFD_UINT32 j;
    for(j = 0; j < 300; ++j)
    {
        error = snfd_write_file(&snfd, 5, 0, &data, 4000);
        TEST_ASSERT_EQUAL_MESSAGE(SNFD_ERROR_NO_ERROR, error, "shouldn't return error");
    }

    snfd_cleanup(&snfd);
    nfe_destroy_flash(flash);
}

/*
 * Write multiple files
 */
void test_snfd_file_write_multiple_files(void)
{
    flash = nfe_create_flash(256, 4096);
    nfe_clear_flash(flash, 0xFF);
    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd_startup(&snfd);
    int i;
    char file1_content[] = { 'F', 'I', 'L', 'E', '1' };
    char file2_content[] = { 'F', 'I', 'L', 'E', '2' };
    char read_buff1[sizeof(file1_content)] = { 0 };
    char read_buff2[sizeof(file2_content)] = { 0 };
    for(i = 0; i < 10; i++)
    {
        snfd_write_file(&snfd, 1, 0, file1_content, sizeof(file1_content));
        snfd_read_file(&snfd, 1, 0, read_buff1, sizeof(read_buff1));
        snfd_write_file(&snfd, 2, 0, file2_content, sizeof(file2_content));
        snfd_read_file(&snfd, 2, 0, read_buff2, sizeof(read_buff2));
    }

    TEST_ASSERT_EQUAL_MEMORY(file1_content, read_buff1, sizeof(file1_content));
    TEST_ASSERT_EQUAL_MEMORY(file2_content, read_buff2, sizeof(file2_content));

    snfd_cleanup(&snfd);
    nfe_destroy_flash(flash);
}

/*
 * Check if read operation overflows the buffer
 */
void test_snfd_file_read_1(void)
{
    flash = nfe_create_flash(256, 4096);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd_startup(&snfd);

    char data[4000];
    SNFD_UINT32 i;
    for(i = 0; i < 4000; ++i)
    {
        data[i] = 101;
    }

    snfd_write_file(&snfd, 5, 0, &data, 4000);
    snfd_write_file(&snfd, 5, 100, &data, 100);
    snfd_write_file(&snfd, 5, 11, &data, 3000);
    for(i = 0; i < 4000; ++i)
    {
        data[i] = 99;
    }
    snfd_write_file(&snfd, 2, 89, &data, 1000);
    char buffer[3] = { 0 };
    snfd_read_file(&snfd, 5, 10, buffer + 1, 1);
    TEST_ASSERT_EQUAL(0, buffer[0]);
    TEST_ASSERT_EQUAL(101, buffer[1]);
    TEST_ASSERT_EQUAL(0, buffer[2]);

    snfd_cleanup(&snfd);
    nfe_destroy_flash(flash);
}

/*
 * Check file size calculation
 */
void test_snfd_file_calc_size(void)
{
    flash = nfe_create_flash(256, 4096);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd_startup(&snfd);

    char * data = "test";

    snfd_write_file(&snfd, 5, 0, data, strlen(data));
    snfd_write_file(&snfd, 5, 4, data, strlen(data));
    snfd_write_file(&snfd, 5, 8, data, strlen(data));
    SNFD_UINT32 calculated_size = snfd_file_calc_size(&snfd, 5);
    TEST_ASSERT_EQUAL(12, calculated_size);

    snfd_cleanup(&snfd);
    nfe_destroy_flash(flash);
}

#endif /* end of include guard: TEST_SNFD_H */
