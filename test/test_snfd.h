#ifndef TEST_SNFD_H
#define TEST_SNFD_H

#include "unity.h"
#include "snfd.h"
#include "nfe.h"

NFE_FLASH * flash;

SNFD_ERROR flash_write_func (SNFD_UINT32 destination, SNFD_UINT8 * source, SNFD_UINT32 size)
{
    nfe_write(flash, destination, source, size);
}

SNFD_ERROR flash_read_func (SNFD_UINT32 source, SNFD_UINT8 * destination, SNFD_UINT32 size)
{
    nfe_read(flash, destination, source, size);
}

SNFD_ERROR flash_block_erase_func (SNFD_UINT16 block_number)
{
    nfe_erase_block(flash, block_number);
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
