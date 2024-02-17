# Entropy Based Steganography (EBS)

Entropy Based Steganography (EBS) is a C library designed for embedding data into different areas of images based on the entropy of the
area. This library provides a powerful tool for hiding information within images, utilizing the concept of entropy to
ensure the produced images are small enough.

## Features

- Embeds data into multiple images.
- High-entropy areas are used first to reduce the size and entropy changes. 

## Dependencies

- [xxHash](https://github.com/Cyan4973/xxHash)
- [Unity Test](https://github.com/ThrowTheSwitch/Unity) (Testing Only)

## Installation

To use the Steganography library, follow these steps:

1. Clone the repository:

   ```bash
   git clone https://github.com/albertexye/EBS.git
   ```

2. Build the library:

   ```bash
   cd EBS
   mkdir build
   cmake ..
   make
   ```
   
   You can specify the installation prefix and building type by using
   
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
   ```

3. Install the library (optional):

   ```bash
   make install
   ```

## Usage

1. Find the package in CMakeLists.txt (installation required):

   ```cmake
   find_package(EBS REQUIRED)
   ```

2. Include the Steganography header as well as other libraries in your C code:

   ```c
   #include <stdio.h>
   #include <string.h>
   
   // stb_image.h is used to decode image files in this example
   #define STB_IMAGE_IMPLEMENTATION
   #include "lib/stb_image.h"
   
   #include "EBS/EBS.h"
   ```
   
3. Create a Message

   ```c
   const char *message = "Hello, world!";
   EBS_Message message = {
           .size = stelen(message),
           .data = (uint8_t *) message
   };
   ```
   
4. Open an images:

   ```c
   EBS_Image image = {};
   
   FILE *file = fopen(filename, "rb");
   // error handling
   
   int w, h, c;
   // you can use any image libraries, 
   // this example uses stb_image.h
   image.pixels = stbi_load_from_file(file, &w, &h, &c, 0);
   // error handling
   
   fclose(file);
   
   image.width = w;
   image.height = h;
   image.channel = c;
   ```

5. Create an ImageList:

   ```c
   EBS_Image images[] = {image};
   EBS_ImageList imageList = {
           .size = sizeof(images) / sizeof(images[0]),
           .images = images
   };
   ```
   
6. Embed the Message into the ImageList

   ```c
   int errorCode;
   EBS_MessageEmbed(
           &imageList,
           &message,
           16, // the square size, shouldn't be larger than 256
           &errorCode // receive the error code
   );
   if (errorCode != EBS_OK) { // error handling
       printf("error while embedding, code %d\n", errorCode);
       stbi_image_free(image->pixels);
       return errorCode;
   }
   ```
   
7. Extract the Message from the ImageList

   ```c
   EBS_Message extracted = EBS_MessageExtract(
           &imageList, // the image list with data embedded
           16, // the square size, shouldn't be larger than 256
           &errorCode // receive the error code
   );
   if (errorCode != EBS_OK) { // error handling
       printf("error while extracting, code %d\n", errorCode);
       stbi_image_free(image->pixels);
       return errorCode;
   }
   ```
   
8. Clean up

   ```c
   free(extracted.data); // the extracted message needs to be manually freed
   stbi_image_free(image->pixels);
   ```

## Examples

Explore the [examples](examples) for sample code snippets and use cases.

## License

This project is licensed under the [BSD 2-Clause License](LICENSE).
