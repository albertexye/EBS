#include "unity/unity.h"

#include "embed.h"

#include <string.h>

void setUp(void) {}

void tearDown(void) {}

void test_SquareEmbed_blank(void) {
    uint8_t pixels[16 * 16 * 3] = {0};
    uint8_t data[80] = {0};
    EBS_Image image = {
            .width = 16,
            .height = 16,
            .channel = 3,
            .pixels = pixels
    };
    EBS_Square square = {
            .x = 0,
            .y = 0
    };
    EBS_SquareEmbed(&image, &square, 16, data, sizeof(data));
    for (uint64_t i = 0; i < sizeof(pixels); ++i) {
        if (pixels[i] != 0) {
            TEST_FAIL_MESSAGE("Pixels are not supposed to change");
        }
    }
}

void test_SquareEmbed_filled(void) {
    uint8_t pixels[16 * 16 * 3];
    uint8_t data[80];
    memset(pixels, 255, sizeof(pixels));
    memset(data, 255, sizeof(data));
    EBS_Image image = {
            .width = 16,
            .height = 16,
            .channel = 3,
            .pixels = pixels
    };
    EBS_Square square = {
            .x = 0,
            .y = 0
    };
    EBS_SquareEmbed(&image, &square, 16, data, sizeof(data));
    for (uint64_t i = 0; i < sizeof(pixels); ++i) {
        if (pixels[i] != 255) {
            TEST_FAIL_MESSAGE("Pixels are not supposed to change");
        }
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_SquareEmbed_blank);
    RUN_TEST(test_SquareEmbed_filled);
    return UNITY_END();
}
