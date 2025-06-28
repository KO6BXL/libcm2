cmake_minimum_required(VERSION 3.5)
project(libcm2 C)

set(CMAKE_C_STANDARD 11)

add_library(cm2 STATIC cm2.c)
