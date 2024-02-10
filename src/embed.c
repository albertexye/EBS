#include "embed.h"

#include <string.h>

void EBS_SquareEmbed(EBS_Image *image, const EBS_Square *square, uint64_t squareSize, const uint8_t *data,
                     uint64_t dataSize) {
    if (dataSize == 0) return;
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
                    if (++index == dataSize) return;
                }
            }
        }
    }
}

void EBS_MessageEmbed(EBS_ImageList *imageList, const EBS_Message *message, uint64_t squareSize, int *errorCode) {
    if (!EBS_CheckSquareSize(squareSize)) {
        *errorCode = EBS_ErrorBadSquareSize;
        return;
    }

    if (!EBS_CheckImageList(imageList)) {
        *errorCode = EBS_ErrorInvalidImage;
        return;
    }

    EBS_ComputedImageList computedImageList = EBS_ComputedImageListCreate(imageList, squareSize);
    if (computedImageList.computedImages == NULL) {
        *errorCode = EBS_ErrorOOM;
        return;
    }

    const uint64_t capacity = EBS_ComputedImageListCapacity(&computedImageList);
    if (message->size > capacity) {
        EBS_ComputedImageListFree(&computedImageList);
        *errorCode = EBS_ErrorOverflow;
        return;
    }

    uint64_t messageIndex = 0;
    uint64_t squareIndex[computedImageList.size];
    memset(squareIndex, 0, computedImageList.size * sizeof(uint64_t));

    {
        const uint64_t maxComputedImageIndex = EBS_ComputedImageListMaxEntropy(&computedImageList, squareIndex);
        EBS_ComputedImage *maxComputedImage = computedImageList.computedImages + maxComputedImageIndex;
        EBS_SquareEmbed(&maxComputedImage->image, maxComputedImage->squareList.squares, squareSize,
                        (const uint8_t *) &message->size, sizeof(message->size));
        ++squareIndex[maxComputedImageIndex];
    }

    while (true) {
        const uint64_t maxComputedImageIndex = EBS_ComputedImageListMaxEntropy(&computedImageList, squareIndex);
        EBS_ComputedImage *maxComputedImage = computedImageList.computedImages + maxComputedImageIndex;

        const EBS_Square *square = maxComputedImage->squareList.squares + squareIndex[maxComputedImageIndex];
        uint64_t messagePieceSize = maxComputedImage->squareList.squareCapacity;
        if (messagePieceSize > message->size - messageIndex) {
            messagePieceSize = message->size - messageIndex;
            EBS_SquareEmbed(&maxComputedImage->image, square, squareSize, message->data + messageIndex,
                            messagePieceSize);
            break;
        }
        EBS_SquareEmbed(&maxComputedImage->image, square, squareSize, message->data + messageIndex, messagePieceSize);

        ++squareIndex[maxComputedImageIndex];
        messageIndex += messagePieceSize;
    }

    EBS_ComputedImageListFree(&computedImageList);

    *errorCode = EBS_OK;
}
