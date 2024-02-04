cmake_minimum_required(VERSION 3.1)
project(Steganography C)
set(CMAKE_C_STANDARD 99)

find_package(xxHash 0.7 CONFIG REQUIRED)

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR})

add_library(Steganography SHARED embed.c steganography.h
        embed.h
        extract.h
        extract.c
        shared.h
        shared.c)
target_link_libraries(Steganography PRIVATE xxHash::xxhash)
install(TARGETS Steganography DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

add_library(Steganography_static STATIC embed.c steganography.h
        embed.h
        extract.h
        extract.c
        shared.h
        shared.c)
target_link_libraries(Steganography_static PRIVATE xxHash::xxhash)
install(TARGETS Steganography_static DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

add_executable(Example examples/comprehensive.c)
target_link_libraries(Example PRIVATE Steganography_static)
install(TARGETS Example DESTINATION ${CMAKE_INSTALL_PREFIX}/examples)
if (TARGET Example)
    install(FILES examples/samples/sample*.png DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/samples)
endif()
