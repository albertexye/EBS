#include "extract_tests.h"

#include "unity/unity.h"
#include "extract.h"
#include "case_loader.h"
#include <string.h>
#include <stdlib.h>

void test_SquareExtract_single(const char *filename, uint64_t squareSize, uint64_t channels) {
    char message[128];
    sprintf(message, "Testing %s (%llux%llux%llu)", filename, squareSize, squareSize, channels);
    TEST_MESSAGE(message);

    Case aCase = loadCase(filename);
    if (aCase.size == 0) TEST_FAIL_MESSAGE("Failed to load the case");

    uint8_t *output = calloc(aCase.size / 8, 1);
    if (output == NULL) {
        freeCase(&aCase);
        TEST_FAIL_MESSAGE("Failed allocating memory");
    }

    EBS_Image image = {
            .width = squareSize,
            .height = squareSize,
            .channel = channels,
            .pixels = aCase.result
    };
    EBS_Square square = {
            .x = 0,
            .y = 0
    };

    EBS_SquareExtract(&image, &square, squareSize, output, aCase.size / 8);
    if (memcmp(aCase.data, output, aCase.size / 8) != 0) {
        free(output);
        freeCase(&aCase);
        TEST_FAIL_MESSAGE("Data don't match");
    }
    free(output);
    freeCase(&aCase);
}

void test_SquareExtract(void) {
    test_SquareExtract_single("./cases/case_32x32x1", 32, 1);
    test_SquareExtract_single("./cases/case_32x32x2", 32, 2);
    test_SquareExtract_single("./cases/case_32x32x3", 32, 3);
    test_SquareExtract_single("./cases/case_32x32x4", 32, 4);
}
