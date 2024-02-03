#include "embed.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>

void EBS_SquareCalcEntropy(const EBS_Image *image, EBS_Square *square, uint64_t squareSize) {
    double entropy = 0.;
    const uint64_t channel = image->channel, width = image->width;
    const uint64_t real_width = width * channel;

    uint16_t map[128];
    const uint8_t *start = image->pixels + (square->y * width + square->x) * channel;
    for (uint64_t c = 0; c < channel; ++c, ++start) {
        memset(map, 0, 128);
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
    if (squareList.squares == NULL) {
        return squareList;
    }

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

    return 0;
}

EBS_ComputedImageList EBS_ComputedImageListCreate(EBS_ImageList *imageList, uint64_t squareSize) {
    EBS_ComputedImageList computedImageList;
    computedImageList.size = imageList->size;
    computedImageList.computedImages = (EBS_ComputedImage *) calloc(computedImageList.size, sizeof(EBS_ComputedImage));
    if (computedImageList.computedImages == NULL) {
        return computedImageList;
    }

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
}

void EBS_SquareEmbed(EBS_Image *image, const EBS_Square *square, uint64_t squareSize, const uint8_t *data,
                     uint64_t dataSize) {
    const uint64_t channel = image->channel;
    const uint64_t realWidth = image->width * channel;
    uint64_t index = 0, bit = 0;
    uint8_t *yStart = image->pixels + (square->y * image->width + square->x) * channel;
    for (uint64_t y = 0; y < squareSize; ++y, yStart += realWidth) {
        uint8_t *xStart = yStart;
        for (uint64_t x = 0; x < squareSize; ++x) {
            for (uint64_t c = 0; c < channel; ++c, ++xStart) {
                *xStart &= (uint8_t) 0b11111110;
                *xStart |= (uint8_t) ((data[index] & (1 << bit)) >> bit);
                if (++bit == 8) {
                    bit = 0;
                    ++index;
                    if (index == dataSize) return;
                }
            }
        }
    }
}

bool EBS_MessageEmbed(EBS_ImageList *imageList, const EBS_Message *message, uint64_t squareSize) {
    EBS_ComputedImageList computedImageList = EBS_ComputedImageListCreate(imageList, squareSize);
    if (computedImageList.computedImages == NULL) return false;

    uint64_t messageIndex = 0;
    uint64_t squareIndex[computedImageList.size];
    memset(squareIndex, 0, computedImageList.size * sizeof(uint64_t));

    while (true) {
        uint64_t maxComputedImageIndex = -1;
        double maxEntropy = 0.;
        for (uint64_t i = 1; i < computedImageList.size; ++i) {
            if (computedImageList.computedImages[i].squareList.size == squareIndex[i]) continue;
            const double entropy = computedImageList.computedImages[i].squareList.squares[squareIndex[i]].entropy;
            if (entropy > maxEntropy) {
                maxEntropy = entropy;
                maxComputedImageIndex = i;
            }
        }
        if (maxComputedImageIndex == -1) {
            EBS_ComputedImageListFree(&computedImageList);
            return false;
        }
        EBS_ComputedImage *maxComputedImage = computedImageList.computedImages + maxComputedImageIndex;

        uint64_t messagePieceSize = maxComputedImage->squareList.squareCapacity;
        if (messagePieceSize > message->size - messageIndex) {
            messagePieceSize = message->size - messageIndex;
            EBS_SquareEmbed(
                    &maxComputedImage->image,
                    maxComputedImage->squareList.squares + squareIndex[maxComputedImageIndex],
                    squareSize,
                    message->data + messageIndex,
                    messagePieceSize
            );
            break;
        }
        EBS_SquareEmbed(
                &maxComputedImage->image,
                maxComputedImage->squareList.squares + squareIndex[maxComputedImageIndex],
                squareSize,
                message->data + messageIndex,
                messagePieceSize
        );

        ++squareIndex[maxComputedImageIndex];
        messageIndex += messagePieceSize;
    }

    EBS_ComputedImageListFree(&computedImageList);

    return true;
}
