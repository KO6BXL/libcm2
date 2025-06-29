cmake_minimum_required(VERSION 3.5)
project(libcm2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

add_library(cm2 STATIC cm2.c)
