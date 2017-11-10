cmake_minimum_required(VERSION 3.7)

project(utility LANGUAGES CXX)

if(MSVC)
    add_compile_options("/W4")
elseif(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options("-Wall -Wextra -Wpedantic")
endif()

add_library(variant INTERFACE)
target_include_directories (variant INTERFACE
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>
)

add_library(callstack INTERFACE)
target_include_directories (callstack INTERFACE
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>
)