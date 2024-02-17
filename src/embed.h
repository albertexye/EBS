#pragma once

#include "../include/EBS/EBS.h"
#include "shared.h"

void EBS_SquareEmbed(EBS_Image *image, const EBS_Square *square, uint64_t squareSize, const uint8_t *data,
                     uint64_t dataSize);
