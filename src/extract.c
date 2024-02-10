#include "extract.h"

#include <string.h>
#include <stdlib.h>

void EBS_SquareExtract(const EBS_Image *image, const EBS_Square *square, uint64_t squareSize, uint8_t *data,
                       uint64_t dataSize) {
    const uint64_t realWidth = image->width * image->channel;
    uint64_t index = 0, bit = 0;
    uint8_t *yStart = image->pixels + square->y * realWidth + square->x * image->channel;
    for (uint64_t y = 0; y < squareSize; ++y, yStart += realWidth) {
        uint8_t *xStart = yStart;
        for (uint64_t x = 0; x < squareSize; ++x) {
            for (uint64_t c = 0; c < image->channel; ++c, ++xStart) {
                data[index] |= (*xStart & 1) << bit;
                if (++bit == 8) {
                    bit = 0;
                    ++index;
                    if (index == dataSize) return;
                }
            }
        }
    }
}

EBS_Message EBS_MessageExtract(EBS_ImageList *imageList, uint64_t squareSize, int *errorCode) {
    EBS_Message message = {
            .size = 0,
            .data = NULL
    };

    if (!EBS_CheckSquareSize(squareSize)) {
        *errorCode = EBS_ErrorBadSquareSize;
        return message;
    }

    if (!EBS_CheckImageList(imageList)) {
        *errorCode = EBS_ErrorInvalidImage;
        return message;
    }

    EBS_ComputedImageList computedImageList = EBS_ComputedImageListCreate(imageList, squareSize);
    if (computedImageList.computedImages == NULL) {
        *errorCode = EBS_ErrorOOM;
        return message;
    }

    uint64_t messageIndex = 0;
    uint64_t squareIndex[computedImageList.size];
    memset(squareIndex, 0, computedImageList.size * sizeof(uint64_t));

    {
        const uint64_t maxComputedImageIndex = EBS_ComputedImageListMaxEntropy(&computedImageList, squareIndex);
        EBS_ComputedImage *maxComputedImage = computedImageList.computedImages + maxComputedImageIndex;
        EBS_SquareExtract(&maxComputedImage->image, maxComputedImage->squareList.squares, squareSize,
                          (uint8_t *) &message.size, sizeof(message.size));
        ++squareIndex[maxComputedImageIndex];
    }

    const uint64_t capacity = EBS_ComputedImageListCapacity(&computedImageList);
    if (message.size > capacity) {
        message.size = 0;
        EBS_ComputedImageListFree(&computedImageList);
        *errorCode = EBS_ErrorInvalidMessage;
        return message;
    }

    message.data = (uint8_t *) calloc(message.size, sizeof(uint8_t));

    while (true) {
        const uint64_t maxComputedImageIndex = EBS_ComputedImageListMaxEntropy(&computedImageList, squareIndex);
        EBS_ComputedImage *maxComputedImage = computedImageList.computedImages + maxComputedImageIndex;

        const EBS_Square *square = maxComputedImage->squareList.squares + squareIndex[maxComputedImageIndex];
        uint64_t messagePieceSize = maxComputedImage->squareList.squareCapacity;
        if (messagePieceSize > message.size - messageIndex) {
            messagePieceSize = message.size - messageIndex;
            EBS_SquareExtract(&maxComputedImage->image, square, squareSize, message.data + messageIndex,
                              messagePieceSize);
            break;
        }
        EBS_SquareExtract(&maxComputedImage->image, square, squareSize, message.data + messageIndex, messagePieceSize);

        ++squareIndex[maxComputedImageIndex];
        messageIndex += messagePieceSize;
    }

    EBS_ComputedImageListFree(&computedImageList);

    *errorCode = EBS_OK;
    return message;
}
