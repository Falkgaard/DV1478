cmake_minimum_required(VERSION 3.14)
project(DV1478 VERSION 0.1.0 LANGUAGES CXX)
add_subdirectory(lib)

add_executable(my_executable main.cpp)
target_compile_features(my_executable PRIVATE cxx_std_20)

