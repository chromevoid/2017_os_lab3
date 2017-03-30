cmake_minimum_required(VERSION 3.6)
project(Banker)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp banker.h)
add_executable(Banker ${SOURCE_FILES})