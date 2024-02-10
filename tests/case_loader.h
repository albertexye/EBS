#pragma once

#include <inttypes.h>

typedef struct {
    uint64_t size;
    uint8_t *original;
    uint8_t *data;
    uint8_t *result;
} Case;

Case loadCase(const char *filename);

void freeCase(Case *aCase);
