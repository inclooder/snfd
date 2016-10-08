#ifndef TEST_SNFD_UTILS_H
#define TEST_SNFD_UTILS_H

#include "unity.h"
#include "snfd_utils.h"


void test_snfd_check_segment_intersection_1(void)
{
    SNFD_SEGMENT a;
    a.begin = 0;
    a.end = 7;
    SNFD_SEGMENT b;
    b.begin = 4;
    b.end = 10;

    SNFD_SEGMENT_INTERSECTION inter;
    snfd_check_segment_intersection(&a, &b, &inter);


    TEST_ASSERT_EQUAL_MESSAGE(3, inter.size, 
            "intersection size should be equal to 3");

    TEST_ASSERT_EQUAL_MESSAGE(4, inter.int_start_a, 
            "intersection start on segment 'a' should be 4");

    TEST_ASSERT_EQUAL_MESSAGE(7, inter.int_end_a,
            "intersection end on segment 'a' should be 7");

    TEST_ASSERT_EQUAL_MESSAGE(0, inter.int_start_b, 
            "intersection start on segment 'b' should be 0");

    TEST_ASSERT_EQUAL_MESSAGE(3, inter.int_end_b,
            "intersection end on segment 'b' should be 3");
}


void test_snfd_check_segment_intersection_2(void)
{
    SNFD_SEGMENT a;
    a.begin = 3;
    a.end = 7;
    SNFD_SEGMENT b;
    b.begin = 1;
    b.end = 8;

    SNFD_SEGMENT_INTERSECTION inter;
    snfd_check_segment_intersection(&a, &b, &inter);


    TEST_ASSERT_EQUAL_MESSAGE(4, inter.size, 
            "intersection size should be equal to 4");

    TEST_ASSERT_EQUAL_MESSAGE(0, inter.int_start_a, 
            "intersection start on segment 'a' should be 0");

    TEST_ASSERT_EQUAL_MESSAGE(4, inter.int_end_a,
            "intersection end on segment 'a' should be 4");

    TEST_ASSERT_EQUAL_MESSAGE(2, inter.int_start_b, 
            "intersection start on segment 'b' should be 2");

    TEST_ASSERT_EQUAL_MESSAGE(6, inter.int_end_b,
            "intersection end on segment 'b' should be 6");
}

void test_snfd_check_segment_intersection_3(void)
{
    SNFD_SEGMENT a;
    a.begin = 8;
    a.end = 14;
    SNFD_SEGMENT b;
    b.begin = 8;
    b.end = 14;

    SNFD_SEGMENT_INTERSECTION inter;
    snfd_check_segment_intersection(&a, &b, &inter);


    TEST_ASSERT_EQUAL_MESSAGE(6, inter.size, 
            "intersection size should be equal to 6");

    TEST_ASSERT_EQUAL_MESSAGE(0, inter.int_start_a, 
            "intersection start on segment 'a' should be 0");

    TEST_ASSERT_EQUAL_MESSAGE(6, inter.int_end_a,
            "intersection end on segment 'a' should be 6");

    TEST_ASSERT_EQUAL_MESSAGE(0, inter.int_start_b, 
            "intersection start on segment 'b' should be 0");

    TEST_ASSERT_EQUAL_MESSAGE(6, inter.int_end_b,
            "intersection end on segment 'b' should be 6");
}

void test_snfd_check_segment_intersection_4(void)
{
    SNFD_SEGMENT a;
    a.begin = 4;
    a.end = 10;
    SNFD_SEGMENT b;
    b.begin = 0;
    b.end = 7;

    SNFD_SEGMENT_INTERSECTION inter;
    snfd_check_segment_intersection(&a, &b, &inter);


    TEST_ASSERT_EQUAL_MESSAGE(3, inter.size, 
            "intersection size should be equal to 3");

    TEST_ASSERT_EQUAL_MESSAGE(0, inter.int_start_a, 
            "intersection start on segment 'a' should be 0");

    TEST_ASSERT_EQUAL_MESSAGE(3, inter.int_end_a,
            "intersection end on segment 'a' should be 3");

    TEST_ASSERT_EQUAL_MESSAGE(4, inter.int_start_b, 
            "intersection start on segment 'b' should be 4");

    TEST_ASSERT_EQUAL_MESSAGE(7, inter.int_end_b,
            "intersection end on segment 'b' should be 7");
}

void test_snfd_check_segment_intersection_5(void)
{
    SNFD_SEGMENT a;
    a.begin = 2;
    a.end = 7;
    SNFD_SEGMENT b;
    b.begin = 4;
    b.end = 6;

    SNFD_SEGMENT_INTERSECTION inter;
    snfd_check_segment_intersection(&a, &b, &inter);


    TEST_ASSERT_EQUAL_MESSAGE(2, inter.size, 
            "intersection size should be equal to 2");

    TEST_ASSERT_EQUAL_MESSAGE(2, inter.int_start_a, 
            "intersection start on segment 'a' should be 2");

    TEST_ASSERT_EQUAL_MESSAGE(4, inter.int_end_a,
            "intersection end on segment 'a' should be 4");

    TEST_ASSERT_EQUAL_MESSAGE(0, inter.int_start_b, 
            "intersection start on segment 'b' should be 0");

    TEST_ASSERT_EQUAL_MESSAGE(2, inter.int_end_b,
            "intersection end on segment 'b' should be 2");
}

void test_snfd_check_segment_intersection_6(void)
{
    SNFD_SEGMENT a;
    a.begin = 4;
    a.end = 9;
    SNFD_SEGMENT b;
    b.begin = 12;
    b.end = 17;

    SNFD_SEGMENT_INTERSECTION inter;
    snfd_check_segment_intersection(&a, &b, &inter);


    TEST_ASSERT_EQUAL_MESSAGE(0, inter.size, 
            "intersection size should be equal to 0");

    TEST_ASSERT_EQUAL_MESSAGE(-1, inter.int_start_a, 
            "intersection start on segment 'a' should be -1");

    TEST_ASSERT_EQUAL_MESSAGE(-1, inter.int_end_a,
            "intersection end on segment 'a' should be -1");

    TEST_ASSERT_EQUAL_MESSAGE(-1, inter.int_start_b, 
            "intersection start on segment 'b' should be -1");

    TEST_ASSERT_EQUAL_MESSAGE(-1, inter.int_end_b,
            "intersection end on segment 'b' should be -1");
}

#endif /* end of include guard: TEST_SNFD_UTILS_H */
