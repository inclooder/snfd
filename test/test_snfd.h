#ifndef TEST_SNFD_H
#define TEST_SNFD_H

#include "unity.h"
#include "snfd.h"
#include "nfe.h"
#include "test_support.h"

void test_snfd_file_write_1(void)
{
    flash = nfe_create_flash(256, 4096);
    nfe_clear_flash(flash, 0xFF);

    SNFD snfd;
    snfd.config.write_func = flash_write_func;
    snfd.config.read_func = flash_read_func;
    snfd.config.block_erase_func = flash_block_erase_func;
    snfd_startup(&snfd);

    //TODO: write a real test

    snfd_cleanup(&snfd);
    nfe_destroy_flash(flash);
}


#endif /* end of include guard: TEST_SNFD_H */
