cmake_minimum_required(VERSION 3.8)
project(NeuralKicker)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp main.h grapher.cpp grapher.h)
add_executable(NeuralKicker ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED network audio graphics window system)
find_package(Threads)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(NeuralKicker ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
target_link_libraries(NeuralKicker ${CMAKE_THREAD_LIBS_INIT} )