#include "unity/unity.h"

#include "embed_tests.h"
#include "extract_tests.h"

void setUp(void) {}

void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_SquareEmbed);
    RUN_TEST(test_SquareExtract);
    return UNITY_END();
}