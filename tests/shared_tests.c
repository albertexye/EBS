#include "shared_tests.h"

#include <stdlib.h>

#include "unity/unity.h"
#include "shared.h"

void test_SquareCalcEntropy(void) {
    uint8_t pixels[] = {
            36, 66, 15, 59, 112, 172, 61, 80,
            230, 29, 113, 186, 205, 252, 101, 220,
            222, 21, 146, 216, 232, 205, 158, 79,
            17, 170, 204, 80, 68, 34, 125, 244,
            36, 243, 244, 87, 35, 239, 131, 4,
            167, 177, 13, 146, 225, 78, 9, 62,
            188, 101, 227, 1, 250, 124, 104, 243,
            204, 52, 79, 101, 82, 162, 68, 251,
    };
    EBS_Image image = {
            .width = 8,
            .height = 8,
            .channel = 1,
            .pixels = pixels
    };
    EBS_Square square = {
            .x = 4,
            .y = 4,
    };
    EBS_SquareCalcEntropy(&image, &square, 4);
    TEST_ASSERT(3.875 == square.entropy);

    image.channel = 2;
    image.height = 4;
    square.x = 0;
    square.y = 0;
    square.entropy = 0;
    EBS_SquareCalcEntropy(&image, &square, 4);
    TEST_ASSERT(3.875 == square.entropy);
}

void test_SquareCompare(void) {
    EBS_Square square1, square2;

    square1.entropy = 3.14;
    square2.entropy = 3.14;
    TEST_ASSERT_EQUAL(0, EBS_SquareCompare(&square1, &square2));

    square1.entropy = 5;
    square2.entropy = 3.14;
    TEST_ASSERT_EQUAL(-1, EBS_SquareCompare(&square1, &square2));

    square1.entropy = 3.14;
    square2.entropy = 5;
    TEST_ASSERT_EQUAL(1, EBS_SquareCompare(&square1, &square2));
}

void test_SquareListCreate(void) {
    uint8_t pixels[19 * 19 * 2];
    EBS_Image image = {
            .width = 19,
            .height = 19,
            .channel = 2,
            .pixels = pixels,
    };
    EBS_SquareList list;

    list = EBS_SquareListCreate(&image, 4);
    TEST_ASSERT_EQUAL(16, list.size);
    TEST_ASSERT_EQUAL(4, list.squareCapacity);
    EBS_SquareListFree(&list);

    image.channel = 1;
    list = EBS_SquareListCreate(&image, 4);
    TEST_ASSERT_EQUAL(16, list.size);
    TEST_ASSERT_EQUAL(2, list.squareCapacity);
    EBS_SquareListFree(&list);
    image.channel = 2;

    image.width = 12;
    image.height = 12;
    list = EBS_SquareListCreate(&image, 4);
    TEST_ASSERT_EQUAL(9, list.size);
    TEST_ASSERT_EQUAL(4, list.squareCapacity);
    EBS_SquareListFree(&list);
    image.width = 19;
    image.height = 19;


    list = EBS_SquareListCreate(&image, 8);
    TEST_ASSERT_EQUAL(4, list.size);
    TEST_ASSERT_EQUAL(16, list.squareCapacity);
    EBS_SquareListFree(&list);
}

void test_SquareListFree(void) {
    uint8_t pixels[19 * 19 * 2];
    EBS_Image image = {
            .width = 19,
            .height = 19,
            .channel = 2,
            .pixels = pixels,
    };
    EBS_SquareList list;

    list = EBS_SquareListCreate(&image, 4);
    EBS_SquareListFree(&list);
    TEST_ASSERT_NULL(list.squares);
    TEST_ASSERT_EQUAL(0, list.size);
}

void test_ImageCompare(void) {
    uint8_t pixels1[] = {85, 162, 85, 132, 202, 29, 196, 30};
    uint8_t pixels2[] = {248, 75, 179, 103, 111, 34, 75, 172};
    EBS_Image image1, image2;
    image1.pixels = pixels1;
    image2.pixels = pixels2;

    image1.width = 0;
    image2.width = 1;
    TEST_ASSERT_EQUAL(EBS_ImageCompare(&image1, &image2), -1);
    image1.width = 1;
    image2.width = 0;
    TEST_ASSERT_EQUAL(EBS_ImageCompare(&image1, &image2), 1);
    image1.width = 1;
    image2.width = 1;

    image1.height = 0;
    image2.height = 1;
    TEST_ASSERT_EQUAL(EBS_ImageCompare(&image1, &image2), -1);
    image1.height = 1;
    image2.height = 0;
    TEST_ASSERT_EQUAL(EBS_ImageCompare(&image1, &image2), 1);
    image1.height = 1;
    image2.height = 1;

    image1.channel = 0;
    image2.channel = 1;
    TEST_ASSERT_EQUAL(EBS_ImageCompare(&image1, &image2), -1);
    image1.channel = 1;
    image2.channel = 0;
    TEST_ASSERT_EQUAL(EBS_ImageCompare(&image1, &image2), 1);
    image1.channel = 1;
    image2.channel = 1;

    TEST_ASSERT_EQUAL(EBS_ImageCompare(&image1, &image2), -1);
    image1.pixels = pixels2;
    image2.pixels = pixels1;
    TEST_ASSERT_EQUAL(EBS_ImageCompare(&image1, &image2), 1);
    image1.pixels = pixels1;
    TEST_ASSERT_EQUAL(EBS_ImageCompare(&image1, &image2), 0);
}

void test_ComputedImageListCreate(void) {
    uint8_t pixels[64];

    EBS_Image images[] = {
            {8, 8, 1, pixels},
            {4, 4, 2, pixels},
            {4, 4, 3, pixels},
            {4, 4, 1, pixels},
    };

    EBS_ImageList imageList = {
            .size = 4,
            .images = images,
    };

    EBS_ComputedImageList computedImageList = EBS_ComputedImageListCreate(&imageList, 2);

    TEST_ASSERT_EQUAL(imageList.size, computedImageList.size);
    TEST_ASSERT_NOT_NULL(computedImageList.computedImages);

    for (int i = 0; i < computedImageList.size; ++i) {
        EBS_ComputedImage computedImage = computedImageList.computedImages[i];
        EBS_Image image = images[i];
        TEST_ASSERT_EQUAL(image.width, computedImage.image.width);
        TEST_ASSERT_EQUAL(image.height, computedImage.image.height);
        TEST_ASSERT_EQUAL(image.channel, computedImage.image.channel);
        TEST_ASSERT_EQUAL(image.pixels, computedImage.image.pixels);
        TEST_ASSERT_NOT_NULL(computedImage.squareList.squares);
    }

    EBS_ComputedImageListFree(&computedImageList);
}

void test_ComputedImageListFree(void) {
    uint8_t pixels[64];

    EBS_Image images[] = {
            {8, 8, 1, pixels},
            {4, 4, 2, pixels},
            {4, 4, 3, pixels},
            {4, 4, 1, pixels},
    };

    EBS_ImageList imageList = {
            .size = 4,
            .images = images,
    };

    EBS_ComputedImageList computedImageList = EBS_ComputedImageListCreate(&imageList, 4);

    EBS_ComputedImageListFree(&computedImageList);

    TEST_ASSERT_NULL(computedImageList.computedImages);
    TEST_ASSERT_EQUAL(0, computedImageList.size);
}

void test_ComputedImageListFindMaxEntropy(void) {
    uint8_t pixels[64];
    uint64_t squareIndex[] = {0, 0, 0, 0};

    EBS_Image images[] = {
            {8, 8, 1, pixels},
            {4, 4, 2, pixels},
            {4, 4, 3, pixels},
            {4, 4, 1, pixels},
    };

    EBS_ImageList imageList = {
            .size = 4,
            .images = images,
    };

    EBS_ComputedImageList computedImageList = EBS_ComputedImageListCreate(&imageList, 4);

    const uint64_t maxEntropy = EBS_ComputedImageListFindMaxEntropy(&computedImageList, squareIndex);

    EBS_ComputedImageListFree(&computedImageList);

    TEST_ASSERT_EQUAL(3, maxEntropy);
}

void test_ComputedImageListCalcCapacity(void) {
    uint8_t pixels[64];

    EBS_Image images[] = {
            {8, 8, 1, pixels},
            {4, 4, 2, pixels},
            {4, 4, 3, pixels},
            {4, 4, 1, pixels},
    };

    EBS_ImageList imageList = {
            .size = 4,
            .images = images,
    };

    EBS_ComputedImageList computedImageList = EBS_ComputedImageListCreate(&imageList, 2);

    const uint64_t capacity = EBS_ComputedImageListCalcCapacity(&computedImageList);

    EBS_ComputedImageListFree(&computedImageList);

    TEST_ASSERT_EQUAL(8, capacity);
}

void test_SquareSizeCheck(void) {
    TEST_ASSERT(EBS_SquareSizeCheck(16));

    TEST_ASSERT(!EBS_SquareSizeCheck(0));

    TEST_ASSERT(!EBS_SquareSizeCheck(256));
    TEST_ASSERT(!EBS_SquareSizeCheck(365));

    TEST_ASSERT(!EBS_SquareSizeCheck(3));
    TEST_ASSERT(!EBS_SquareSizeCheck(17));
}

void test_ImageCheck(void) {
    uint8_t pixels[4];

    EBS_Image image = {
            .pixels = pixels,
            .width = 2,
            .height = 2,
            .channel = 1
    };
    TEST_ASSERT(EBS_ImageCheck(&image));

    image.pixels = NULL;
    TEST_ASSERT(!EBS_ImageCheck(&image));
    image.pixels = pixels;

    image.width = 0;
    TEST_ASSERT(!EBS_ImageCheck(&image));
    image.width = 2;

    image.height = 0;
    TEST_ASSERT(!EBS_ImageCheck(&image));
    image.height = 2;

    image.channel = 0;
    TEST_ASSERT(!EBS_ImageCheck(&image));
    image.channel = 1;
}

void test_ImageListCheck(void) {
    uint8_t pixels[4];

    EBS_Image image = {
            .pixels = pixels,
            .width = 2,
            .height = 2,
            .channel = 1
    };

    EBS_Image images[5];
    for (int i = 0; i < 5; ++i) {
        images[i] = image;
    }

    EBS_ImageList imageList = {
            .size = 5,
            .images = images,
    };

    TEST_ASSERT(EBS_ImageListCheck(&imageList));

    images[0].width = 0;
    TEST_ASSERT(!EBS_ImageListCheck(&imageList));
    images[0].width = 2;

    images[4].width = 0;
    TEST_ASSERT(!EBS_ImageListCheck(&imageList));
    images[4].width = 2;

    images[2].width = 0;
    TEST_ASSERT(!EBS_ImageListCheck(&imageList));
    images[2].width = 2;

    images[0].width = 0;
    images[2].width = 0;
    TEST_ASSERT(!EBS_ImageListCheck(&imageList));
    images[0].width = 2;
    images[2].width = 2;
}

void test_MessageFree(void) {
    EBS_Message message = {
            .size = 64,
            .data = calloc(64, 1),
    };
    TEST_ASSERT_NOT_NULL_MESSAGE(message.data, "Failed to alloc memory");
    EBS_MessageFree(&message);
    TEST_ASSERT_EQUAL(0, message.size);
    TEST_ASSERT_NULL(message.data);
}
