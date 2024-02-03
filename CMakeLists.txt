cmake_minimum_required(VERSION 3.1)
project(Steganography C)
set(CMAKE_C_STANDARD 99)

add_library(Steganography_static STATIC embed.c Steganography.h
        embed.h
        extract.h
        extract.c)

add_library(Steganography SHARED embed.c Steganography.h
        embed.h
        extract.h
        extract.c)
