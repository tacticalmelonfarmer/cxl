cmake_minimum_required(VERSION 3.7)

project(utility LANGUAGES CXX)

add_executable(cintegral src/tests/cintegral.cpp)
target_include_directories(cintegral PUBLIC include)
target_compile_features(cintegral PUBLIC cxx_std_17)

add_executable(citerator src/tests/citerator.cpp)
target_include_directories(citerator PUBLIC include)
target_compile_features(citerator PUBLIC cxx_std_17)

add_executable(cparse src/tests/cparse.cpp)
target_include_directories(cparse PUBLIC include)
target_compile_features(cparse PUBLIC cxx_std_17)

set_target_properties(cintegral citerator cparse
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)