cmake_minimum_required(VERSION 3.1)
project(Steganography C)
set(CMAKE_C_STANDARD 99)

find_package(xxHash 0.7 CONFIG REQUIRED)

add_library(Steganography SHARED embed.c steganography.h
        embed.h
        extract.h
        extract.c
        shared.h
        shared.c)
target_link_libraries(Steganography PRIVATE xxHash::xxhash)

add_library(Steganography_static STATIC embed.c steganography.h
        embed.h
        extract.h
        extract.c
        shared.h
        shared.c)
target_link_libraries(Steganography_static PRIVATE xxHash::xxhash)

add_executable(Test examples/comprehensive.c)
target_link_libraries(Test PRIVATE Steganography_static)
