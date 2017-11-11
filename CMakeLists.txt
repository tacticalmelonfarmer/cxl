cmake_minimum_required(VERSION 3.7)

project(utility LANGUAGES CXX)

set(BUILD_TREE ${CMAKE_BINARY_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_TREE}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_TREE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_TREE}/bin)

add_subdirectory(type_safe)
include_directories(type_safe/include ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(variant_use example/variant_use.cpp)
target_compile_features(variant_use PUBLIC cxx_std_17)

find_package(Threads)
add_executable(callstack_use example/callstack_use.cpp)
target_compile_features(callstack_use PUBLIC cxx_std_17)
target_link_libraries(callstack_use ${CMAKE_THREAD_LIBS_INIT})

add_library(utility INTERFACE)
target_include_directories (utility INTERFACE
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>
)