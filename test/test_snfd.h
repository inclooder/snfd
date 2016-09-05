#ifndef TEST_SNFD_H
#define TEST_SNFD_H

#include "unity.h"
#include "snfd.h"
#include "nfe.h"
#include "test_support.h"


void test_snfd_initialize_blocks(void)
{
    flash = nfe_create_flash(10, 200);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd.config.block_count = 10;
    snfd.config.block_size = 200;

    // TODO


    nfe_destroy_flash(flash);
}

void test_snfd_are_blocks_initialized_1(void)
{
    flash = nfe_create_flash(10, 200);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd.config.block_count = 10;
    snfd.config.block_size = 200;

    SNFD_BOOL result = snfd_are_blocks_initialized(&snfd);
    TEST_ASSERT_EQUAL(SNFD_FALSE, result);

    nfe_destroy_flash(flash);
}

void test_snfd_are_blocks_initialized_2(void)
{
    flash = nfe_create_flash(10, 200);
    nfe_clear_flash(flash, 0xFF);


    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd.config.block_count = 10;
    snfd.config.block_size = 200;

    snfd_initialize_blocks(&snfd);

    SNFD_BOOL result = snfd_are_blocks_initialized(&snfd);
    TEST_ASSERT_EQUAL(SNFD_TRUE, result);

    nfe_destroy_flash(flash);
}

#endif /* end of include guard: TEST_SNFD_H */
