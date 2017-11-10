cmake_minimum_required(VERSION 3.8)
project(NeuralKicker)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp main.h)
add_executable(NeuralKicker ${SOURCE_FILES})