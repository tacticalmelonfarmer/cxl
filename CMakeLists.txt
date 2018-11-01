cmake_minimum_required(VERSION 3.7)

project(cxl LANGUAGES CXX)

add_executable(csv example/csv/main.cpp)
target_include_directories(csv PUBLIC include)
target_compile_features(csv PUBLIC cxx_std_17)

set_target_properties(csv
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)