cmake_minimum_required(VERSION 3.5)
project(EBS LANGUAGES C CXX VERSION 0.1.0)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

find_package(xxHash 0.7 CONFIG REQUIRED)
find_package(unity QUIET)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_library(${PROJECT_NAME} SHARED)
target_link_libraries(${PROJECT_NAME} PRIVATE xxHash::xxhash m)

add_library(${PROJECT_NAME}Static STATIC)
target_link_libraries(${PROJECT_NAME}Static PRIVATE xxHash::xxhash m)

add_executable(${PROJECT_NAME}_tests)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE xxHash::xxhash unity::framework m)

add_executable(${PROJECT_NAME}_c_example)
target_link_libraries(${PROJECT_NAME}_c_example PRIVATE ${PROJECT_NAME} m)

add_executable(${PROJECT_NAME}_cpp_example)
target_link_libraries(${PROJECT_NAME}_cpp_example PRIVATE ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}_cpp_example PROPERTIES LANGUAGE CXX m stdc++)

if (TARGET ${PROJECT_NAME}_tests)
    if (NOT unity_FOUND)
        message(FATAL_ERROR "Unity Test not found. ${PROJECT_NAME}_tests cannot build. ")
    endif ()
endif ()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

target_sources(${PROJECT_NAME}
        PRIVATE
        include/EBS/EBS.h
        src/embed.h
        src/embed.c
        src/extract.h
        src/extract.c
        src/shared.h
        src/shared.c
)

target_sources(${PROJECT_NAME}Static
        PRIVATE
        include/EBS/EBS.h
        src/embed.h
        src/embed.c
        src/extract.h
        src/extract.c
        src/shared.h
        src/shared.c
)

target_sources(${PROJECT_NAME}_tests
        PRIVATE
        tests/tests.c
        tests/embed_tests.h
        tests/embed_tests.c
        tests/case_loader.h
        tests/case_loader.c
        tests/extract_tests.h
        tests/extract_tests.c
        tests/shared_tests.c
        tests/shared_tests.h
        include/EBS/EBS.h
        src/embed.h
        src/embed.c
        src/extract.h
        src/extract.c
        src/shared.h
        src/shared.c
)

target_sources(${PROJECT_NAME}_c_example
        PRIVATE
        include/EBS/EBS.h
        examples/c_example.c
)

target_sources(${PROJECT_NAME}_cpp_example
        PRIVATE
        include/EBS/EBS.hpp
        examples/cpp_example.cpp
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(${PROJECT_NAME}Static
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(${PROJECT_NAME}_tests
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(${PROJECT_NAME}_c_example
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(${PROJECT_NAME}_cpp_example
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(${PROJECT_NAME}_PUBLIC_HEADERS
        include/EBS/EBS.h
        include/EBS/EBS.hpp
)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        C_STANDARD          11
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS        OFF
        PUBLIC_HEADER       "${${PROJECT_NAME}_PUBLIC_HEADERS}"
)

set_target_properties(${PROJECT_NAME}_tests
        PROPERTIES
        C_STANDARD          11
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS        OFF
)

write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
        VERSION       ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

## Target installation
install(TARGETS   ${PROJECT_NAME} ${PROJECT_NAME}_tests
        EXPORT        ${PROJECT_NAME}Targets
        ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        COMPONENT     library
)

## Target's cmake files: targets export
install(EXPORT  ${PROJECT_NAME}Targets
        NAMESPACE   ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

## Target's cmake files: config and version config for find_package()
install(FILES   ${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
