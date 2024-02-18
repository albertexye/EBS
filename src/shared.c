#include "shared.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "xxhash.h"

void EBS_SquareCalcEntropy(const EBS_Image *image, EBS_Square *square, uint64_t squareSize) {
    double entropy = 0.;
    const uint64_t channel = image->channel, width = image->width;
    const uint64_t real_width = width * channel;

    uint16_t map[128];
    const uint8_t *start = image->pixels + (square->y * width + square->x) * channel;
    for (uint64_t c = 0; c < channel; ++c, ++start) {
        memset(map, 0, sizeof(map));
        const uint8_t *y_start = start;
        for (uint64_t y = 0; y < squareSize; ++y, y_start += real_width) {
            const uint8_t *x_start = y_start;
            for (uint64_t x = 0; x < squareSize; ++x, x_start += channel) {
                ++(map[(*x_start) >> 1]);
            }
        }
        for (uint64_t i = 0; i < 128; ++i) {
            if (map[i] == 0) continue;
            const double p = (double) map[i] / (double) (squareSize * squareSize);
            entropy += -p * log2(p);
        }
    }
    square->entropy = entropy / (double) channel;
}

int EBS_SquareCompare(const void *square1, const void *square2) {
    const double entropy1 = ((EBS_Square *) square1)->entropy;
    const double entropy2 = ((EBS_Square *) square2)->entropy;
    if (entropy1 > entropy2) {
        return -1;
    } else if (entropy1 < entropy2) {
        return 1;
    } else {
        return 0;
    }
}

EBS_SquareList EBS_SquareListCreate(const EBS_Image *image, uint64_t squareSize) {
    EBS_SquareList squareList;
    const uint64_t squareWidth = image->width / squareSize;
    const uint64_t squareHeight = image->height / squareSize;
    squareList.size = squareWidth * squareHeight;
    squareList.squareCapacity = squareSize * squareSize * image->channel / 8;
    squareList.squares = (EBS_Square *) calloc(squareList.size, sizeof(EBS_Square));
    if (squareList.squares == NULL) return squareList;

    uint64_t i = 0;
    for (uint64_t y = 0; y < image->height - image->height % squareSize; y += squareSize) {
        for (uint64_t x = 0; x < image->width - image->width % squareSize; x += squareSize) {
            squareList.squares[i] = (EBS_Square) {.x = x, .y = y};
            EBS_SquareCalcEntropy(image, squareList.squares + i, squareSize);
            ++i;
        }
    }

    qsort(squareList.squares, squareList.size, sizeof(EBS_Square), EBS_SquareCompare);

    return squareList;
}

void EBS_SquareListFree(EBS_SquareList *squareList) {
    free(squareList->squares);
    squareList->squares = NULL;
    squareList->size = 0;
    squareList->squareCapacity = 0;
}

int EBS_ImageCompare(const void *image1, const void *image2) {
    const EBS_Image *ebsImage1 = image1;
    const EBS_Image *ebsImage2 = image2;

    if (ebsImage1->width != ebsImage2->width) {
        return ebsImage1->width > ebsImage2->width ? 1 : -1;
    }
    if (ebsImage1->height != ebsImage2->height) {
        return ebsImage1->height > ebsImage2->height ? 1 : -1;
    }
    if (ebsImage1->channel != ebsImage2->channel) {
        return ebsImage1->channel > ebsImage2->channel ? 1 : -1;
    }

    const uint64_t image1Size = ebsImage1->width * ebsImage1->height * ebsImage1->channel;
    XXH64_hash_t hash1 = XXH64(ebsImage1->pixels, image1Size, 0);
    const uint64_t image2Size = ebsImage2->width * ebsImage2->height * ebsImage2->channel;
    XXH64_hash_t hash2 = XXH64(ebsImage2->pixels, image2Size, 0);
    if (hash1 != hash2) {
        return hash1 > hash2 ? 1 : -1;
    }

    return 0;
}

EBS_ComputedImageList EBS_ComputedImageListCreate(EBS_ImageList *imageList, uint64_t squareSize) {
    EBS_ComputedImageList computedImageList;
    computedImageList.size = imageList->size;
    computedImageList.computedImages = (EBS_ComputedImage *) calloc(computedImageList.size, sizeof(EBS_ComputedImage));
    if (computedImageList.computedImages == NULL) return computedImageList;

    qsort(imageList->images, imageList->size, sizeof(EBS_Image), EBS_ImageCompare);

    for (uint64_t i = 0; i < imageList->size; ++i) {
        const EBS_Image *image = imageList->images + i;
        EBS_ComputedImage computedImage = {
                .image = *image,
                .squareList = EBS_SquareListCreate(image, squareSize)
        };
        if (computedImage.squareList.squares == NULL) {
            EBS_ComputedImageListFree(&computedImageList);
            return computedImageList;
        }
        computedImageList.computedImages[i] = computedImage;
    }

    return computedImageList;
}

void EBS_ComputedImageListFree(EBS_ComputedImageList *computedImageList) {
    for (uint64_t i = 0; i < computedImageList->size; ++i) {
        EBS_SquareListFree(&computedImageList->computedImages[i].squareList);
    }
    free(computedImageList->computedImages);
    computedImageList->computedImages = NULL;
    computedImageList->size = 0;
}

uint64_t EBS_ComputedImageListFindMaxEntropy(const EBS_ComputedImageList *computedImageList, const uint64_t *squareIndex) {
    uint64_t maxComputedImageIndex = 0;
    double maxEntropy = 0.;
    if (squareIndex) {
        for (uint64_t i = 0; i < computedImageList->size; ++i) {
            if (computedImageList->computedImages[i].squareList.size == squareIndex[i]) continue;
            const double entropy = computedImageList->computedImages[i].squareList.squares[squareIndex[i]].entropy;
            if (entropy > maxEntropy) {
                maxEntropy = entropy;
                maxComputedImageIndex = i;
            }
        }
    } else {
        for (uint64_t i = 0; i < computedImageList->size; ++i) {
            const double entropy = computedImageList->computedImages[i].squareList.squares->entropy;
            if (entropy > maxEntropy) {
                maxEntropy = entropy;
                maxComputedImageIndex = i;
            }
        }
    }
    return maxComputedImageIndex;
}

uint64_t EBS_ComputedImageListCalcCapacity(const EBS_ComputedImageList *computedImageList) {
    uint64_t capacity = 0;
    for (uint64_t i = 0; i < computedImageList->size; ++i) {
        const EBS_SquareList *squareList = &computedImageList->computedImages[i].squareList;
        capacity += squareList->size * squareList->squareCapacity;
    }
    {
        const uint64_t maxComputedImageIndex = EBS_ComputedImageListFindMaxEntropy(computedImageList, NULL);
        EBS_ComputedImage *maxComputedImage = computedImageList->computedImages + maxComputedImageIndex;
        capacity -= maxComputedImage->squareList.squareCapacity;
    }
    return capacity;
}

void EBS_MessageFree(EBS_Message *message) {
    free(message->data);
    message->data = NULL;
    message->size = 0;
}

bool EBS_SquareSizeCheck(uint64_t squareSize) {
    return squareSize != 0 && squareSize % 4 == 0 && squareSize < 256;
}

bool EBS_ImageCheck(const EBS_Image *image) {
    return image->width != 0 && image->height != 0 && image->channel != 0 && image->pixels != NULL;
}

bool EBS_ImageListCheck(const EBS_ImageList *imageList) {
    for (uint64_t i = 0; i < imageList->size; ++i) {
        const EBS_Image *image = imageList->images + i;
        if (!EBS_ImageCheck(image)) {
            return false;
        }
    }
    return true;
}
