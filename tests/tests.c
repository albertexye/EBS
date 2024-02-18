#include "unity/unity.h"

#include "embed_tests.h"
#include "extract_tests.h"
#include "shared_tests.h"

void setUp(void) {}

void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_SquareEmbed);

    RUN_TEST(test_SquareExtract);

    RUN_TEST(test_SquareCalcEntropy);
    RUN_TEST(test_SquareCompare);
    RUN_TEST(test_SquareListCreate);
    RUN_TEST(test_SquareListFree);
    RUN_TEST(test_ImageCompare);
    RUN_TEST(test_ComputedImageListCreate);
    RUN_TEST(test_ComputedImageListFree);
    RUN_TEST(test_ComputedImageListFindMaxEntropy);
    RUN_TEST(test_ComputedImageListCalcCapacity);
    RUN_TEST(test_SquareSizeCheck);
    RUN_TEST(test_ImageCheck);
    RUN_TEST(test_ImageListCheck);
    RUN_TEST(test_MessageFree);

    return UNITY_END();
}