#pragma once

#include <inttypes.h>
#include <stdbool.h>

static const int EBS_OK = 0;
static const int EBS_ErrorOOM = 1;
static const int EBS_ErrorInvalidMessage = 2;
static const int EBS_ErrorOverflow = 3;

typedef struct EBS_Image {
    uint64_t width;
    uint64_t height;
    uint64_t channel;
    uint8_t *pixels;
} EBS_Image;

typedef struct EBS_ImageList {
    uint64_t size;
    EBS_Image *images;
} EBS_ImageList;

typedef struct EBS_Message {
    uint64_t size;
    uint8_t *data;
} EBS_Message;

void EBS_MessageEmbed(EBS_ImageList *imageList, const EBS_Message *message, uint64_t squareSize, int *errorCode);

EBS_Message EBS_MessageExtract(EBS_ImageList *imageList, uint64_t squareSize, int *errorCode);
