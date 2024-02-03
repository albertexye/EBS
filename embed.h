#pragma once

#include "Steganography.h"

typedef struct EBS_Square {
    uint64_t x;
    uint64_t y;
    double entropy;
} EBS_Square;

typedef struct EBS_SquareList {
    uint64_t size;
    uint64_t squareCapacity;
    EBS_Square *squares;
} EBS_SquareList;

typedef struct EBS_ComputedImage {
    EBS_Image image;
    EBS_SquareList squareList;
} EBS_ComputedImage;

typedef struct EBS_ComputedImageList {
    uint64_t size;
    EBS_ComputedImage *computedImages;
} EBS_ComputedImageList;

void EBS_SquareCalcEntropy(const EBS_Image *image, EBS_Square *square, uint64_t squareSize);

int EBS_SquareCompare(const void *square1, const void *square2);

EBS_SquareList EBS_SquareListCreate(const EBS_Image *image, uint64_t squareSize);

void EBS_SquareListFree(EBS_SquareList *squareList);

int EBS_ImageCompare(const void *image1, const void *image2);

EBS_ComputedImageList EBS_ComputedImageListCreate(EBS_ImageList *imageList, uint64_t squareSize);

void EBS_ComputedImageListFree(EBS_ComputedImageList *computedImageList);

void EBS_SquareEmbed(EBS_Image *image, const EBS_Square *square, uint64_t squareSize, const uint8_t *data,
                     uint64_t dataSize);
