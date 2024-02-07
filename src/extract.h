#pragma once

#include "EBS.h"
#include "shared.h"

void EBS_SquareExtract(const EBS_Image *image, const EBS_Square *square, uint64_t squareSize, uint8_t *data,
                       uint64_t dataSize);
