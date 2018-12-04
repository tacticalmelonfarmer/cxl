cmake_minimum_required(VERSION 3.10)

project(cxl LANGUAGES CXX)

add_subdirectory(tools)

add_executable(csv examples/csv/main.cpp)
target_include_directories(csv PUBLIC include)
target_compile_features(csv PUBLIC cxx_std_17)

add_executable(fake_class examples/fake_class/main.cpp)
target_include_directories(fake_class PUBLIC include)
target_compile_features(fake_class PUBLIC cxx_std_17)