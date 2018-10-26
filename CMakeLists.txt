cmake_minimum_required(VERSION 3.7)

project(utility LANGUAGES CXX)

add_executable(csv_parser_generator example/csv_parser_generator.cpp)
target_include_directories(csv_parser_generator PUBLIC include)
target_compile_features(csv_parser_generator PUBLIC cxx_std_17)

set_target_properties(csv_parser_generator
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)