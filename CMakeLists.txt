cmake_minimum_required(VERSION 3.10)
project(png2jpg LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    png2jpg.cpp
    stb_image.h
    stb_image_write.h
)

add_executable(png2jpg ${SOURCES})

if(MSVC)
    target_compile_options(png2jpg PRIVATE /O2 /W3)
else()
    target_compile_options(png2jpg PRIVATE -O2 -Wall -Wextra)
endif()

if(ANDROID)
    message(STATUS "Building for Android (NDK)...")
    target_compile_definitions(png2jpg PRIVATE ANDROID_BUILD)
    set_target_properties(png2jpg PROPERTIES
        LINK_FLAGS "-static"
    )
endif()

if(UNIX AND NOT ANDROID)
    add_custom_command(TARGET png2jpg POST_BUILD
        COMMAND ${CMAKE_STRIP} $<TARGET_FILE:png2jpg>
        COMMENT "Stripping symbols to reduce binary size"
    )
endif()

set_target_properties(png2jpg PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
