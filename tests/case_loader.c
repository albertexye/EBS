#include "case_loader.h"

#include <stdlib.h>
#include <stdio.h>

Case loadCase(const char *filename) {
    Case aCase = {};
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return aCase;
    }

    size_t readNum = fread(&aCase.size, sizeof(aCase.size), 1, file);
    if (readNum != 1) {
        fclose(file);
        freeCase(&aCase);
        return aCase;
    }

    if (aCase.size % 8 != 0) {
        fclose(file);
        freeCase(&aCase);
        return aCase;
    }

    aCase.original = calloc(aCase.size, 1);
    if (aCase.original == NULL) {
        fclose(file);
        freeCase(&aCase);
        return aCase;
    }

    aCase.data = calloc(aCase.size / 8, 1);
    if (aCase.data == NULL) {
        fclose(file);
        freeCase(&aCase);
        return aCase;
    }

    aCase.result = calloc(aCase.size, 1);
    if (aCase.result == NULL) {
        fclose(file);
        freeCase(&aCase);
        return aCase;
    }

    readNum = fread(aCase.original, 1, aCase.size, file);
    if (readNum != aCase.size) {
        fclose(file);
        freeCase(&aCase);
        return aCase;
    }

    readNum = fread(aCase.data, 1, aCase.size / 8, file);
    if (readNum != aCase.size / 8) {
        fclose(file);
        freeCase(&aCase);
        return aCase;
    }

    readNum = fread(aCase.result, 1, aCase.size, file);
    if (readNum != aCase.size) {
        fclose(file);
        freeCase(&aCase);
        return aCase;
    }

    fclose(file);

    return aCase;
}

void freeCase(Case *aCase) {
    free(aCase->original);
    free(aCase->data);
    free(aCase->result);
    aCase->size = 0;
    aCase->original = NULL;
    aCase->data = NULL;
    aCase->result = NULL;
}
