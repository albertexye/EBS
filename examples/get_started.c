#include "EBS/EBS.h"

#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "lib/stb_image.h"
#include "lib/stb_image_write.h"

EBS_Image openImage(const char *filename) {
    EBS_Image image = {};

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("error while opening file");
        return image;
    }

    int w, h, c;
    image.pixels = stbi_load_from_file(file, &w, &h, &c, 0);
    fclose(file);
    if (image.pixels == NULL) {
        printf("error while opening image '%s', reason: %s\n", filename, stbi_failure_reason());
    }

    image.width = w;
    image.height = h;
    image.channel = c;

    return image;
}

void saveImage(const EBS_Image *image, const char *filename) {
    const int code = stbi_write_png(filename, image->width, image->height, image->channel, image->pixels,
                                    image->width * image->channel);
    if (code == 0) {
        printf("error saving image '%s', reason: %s\n", filename, stbi_failure_reason());
    }
}

void closeImage(EBS_Image *image) {
    stbi_image_free(image->pixels);
    image->pixels = NULL;
}

uint8_t *randomBytes(uint64_t size) {
    uint8_t *buffer = (uint8_t *) calloc(size, sizeof(uint8_t));
    for (uint64_t i = 0; i < size; ++i) {
        buffer[i] = rand();
    }
    return buffer;
}

int main(void) {
    // open images
    EBS_Image sample1 = openImage("./samples/sample1.png");
    EBS_Image sample2 = openImage("./samples/sample2.png");
    EBS_Image sample3 = openImage("./samples/sample3.png");

    // create an ImageList
    EBS_Image images[] = {sample1, sample2, sample3};
    EBS_ImageList imageList = {
            .size = sizeof(images) / sizeof(images[0]), // the size of the image list
            .images = images // a list of images
    };

    // create a random message
    const uint64_t messageSize = 1024 * 1024;
    EBS_Message message = {
            .size = messageSize,
            .data = randomBytes(messageSize)
    };

    int err; // the error code

    // embed
    EBS_MessageEmbed(
            &imageList,
            &message,
            16, // the square size, shouldn't be larger than 256
            &err // receive the error code
    );
    if (err != EBS_OK) { // handle the error code
        printf("error while embedding, code %d\n", err);
        free(message.data);
        closeImage(&sample1);
        closeImage(&sample2);
        closeImage(&sample3);
        return err;
    }

    // save the images with data embedded
    saveImage(&sample1, "samples/embedded1.png");
    saveImage(&sample2, "samples/embedded2.png");
    saveImage(&sample3, "samples/embedded3.png");

    // extract
    EBS_Message extracted = EBS_MessageExtract(
            &imageList, // the image list with data embedded
            16, // the square size, shouldn't be larger than 256
            &err // receive the error code
    );
    if (err != EBS_OK) { // handle the error code
        printf("error while extracting, code %d\n", err);
        free(message.data);
        closeImage(&sample1);
        closeImage(&sample2);
        closeImage(&sample3);
        return err;
    }

    // check the result
    if (extracted.size != message.size || memcmp(extracted.data, message.data, message.size) != 0) {
        printf("unexpected extracted data, expected %s, got %s\n", message.data, extracted.data);
        free(message.data);
        free(extracted.data); // the extracted message needs to be manually freed
        closeImage(&sample1);
        closeImage(&sample2);
        closeImage(&sample3);
        return err;
    }

    // clean up
    free(message.data);
    free(extracted.data); // the extracted message needs to be manually freed
    closeImage(&sample1);
    closeImage(&sample2);
    closeImage(&sample3);

    return 0;
}