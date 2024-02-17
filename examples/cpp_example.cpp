#include "EBS/EBS.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "lib/stb_image.h"
#include "lib/stb_image_write.h"

#include <iostream>
#include <random>
#include <algorithm>

std::shared_ptr<EBS::Image> openImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == nullptr) {
        perror("error while opening file");
        return nullptr;
    }

    int w, h, c;
    auto ptr = stbi_load_from_file(file, &w, &h, &c, 0);
    if (ptr == nullptr) {
        std::cout << "error opening image '" << filename << "', reason: " << stbi_failure_reason() << std::endl;
        exit(1);
    }

    auto pixels = std::make_shared<std::vector<uint8_t>>(ptr, ptr + w * h * c);
    fclose(file);
    stbi_image_free(ptr);

    auto image = std::make_shared<EBS::Image>(w, h, c, pixels);

    return image;
}

void saveImage(const std::shared_ptr<EBS::Image> &image, const std::string &filename) {
    const int code = stbi_write_png(filename.c_str(), image->width, image->height, image->channel, image->pixels->data(),
                                    image->width * image->channel);
    if (code == 0) {
        std::cout << "error saving image '" << filename << "', reason: " << stbi_failure_reason() << std::endl;
    }
}

EBS::Data randomBytes(uint64_t size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(0, 255);

    EBS::Data data(size);
    std::generate(data.begin(), data.end(), gen);
    return data;
}

int main() {
    // open images
    auto sample1 = openImage("./samples/sample1.png");
    auto sample2 = openImage("./samples/sample2.png");
    auto sample3 = openImage("./samples/sample3.png");

    // create an ImageList
    EBS::ImageList imageList{sample1, sample2, sample3};

    // create a random message
    EBS::Data data{randomBytes(1024 * 1024)};

    EBS::Message message{16};

    // embed
    try { message.embed(imageList, data); }
    catch (const EBS::Error &error) {
        std::cout << error.what() << std::endl;
        return 1;
    }

    // save the images with data embedded
    saveImage(sample1, "samples/embedded1.png");
    saveImage(sample2, "samples/embedded2.png");
    saveImage(sample3, "samples/embedded3.png");

    EBS::Data extracted;
    // extract
    try { extracted = message.extract(imageList); }
    catch (const EBS::Error &error) {
        std::cout << error.what() << std::endl;
        return 1;
    }

    // check the result
    if (extracted != data) {
        std::cout << "unexpected extracted data, expected " << std::string(data.begin(), data.end()) << ", got " << std::string(data.begin(), data.end()) << std::endl;
        return 1;
    }

    return 0;
}
