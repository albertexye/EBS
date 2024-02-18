#include "embed_tests.h"

#include "unity/unity.h"
#include "embed.h"
#include "case_loader.h"
#include <string.h>
#include <stdlib.h>

void test_SquareEmbed_single(const char *filename, uint64_t squareSize, uint64_t channels) {
    Case aCase = loadCase(filename);
    if (aCase.size == 0) TEST_FAIL_MESSAGE("Failed to load the case");

    uint8_t *output = calloc(aCase.size, 1);
    if (output == NULL) {
        freeCase(&aCase);
        TEST_FAIL_MESSAGE("Failed allocating memory");
    }
    memcpy(output, aCase.original, aCase.size);

    EBS_Image image = {
            .width = squareSize,
            .height = squareSize,
            .channel = channels,
            .pixels = output
    };
    EBS_Square square = {
            .x = 0,
            .y = 0
    };

    EBS_SquareEmbed(&image, &square, squareSize, aCase.data, aCase.size / 8);
    if (memcmp(aCase.result, output, aCase.size) != 0) {
        free(output);
        freeCase(&aCase);
        TEST_FAIL_MESSAGE("Data don't match");
    }
    free(output);
    freeCase(&aCase);
}

void test_SquareEmbed(void) {
    test_SquareEmbed_single("./cases/case_32x32x1", 32, 1);
    test_SquareEmbed_single("./cases/case_32x32x2", 32, 2);
    test_SquareEmbed_single("./cases/case_32x32x3", 32, 3);
    test_SquareEmbed_single("./cases/case_32x32x4", 32, 4);
}
