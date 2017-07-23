cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(ColourSpaceConverter LANGUAGES C)

# Compile for ARM if we specified via a command line option
option(COMPILE_ARM "Compile the program for running on ARM on lab machines" OFF)
if(COMPILE_ARM)
    set(CMAKE_C_COMPILER /opt/arm/4.3.2/bin/arm-linux-gcc)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static")
endif()

#Use C99 and enable all compiler warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -save-temps -std=c99 -Wall") 

add_executable(converter "colour_space_conversion_integers.c")
