cmake_minimum_required(VERSION 3.10)

project(cxl)

## this tool is our code-generator for aggregate sublibrary
add_executable(code-generators tools/code-generators/aggregate.generator.cpp)
target_compile_features(code-generators PUBLIC cxx_std_17)
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	target_link_libraries(code-generators PUBLIC stdc++fs)
endif()

## this command calls code-generators produced executable
add_custom_command(TARGET code-generators POST_BUILD
	COMMAND $<TARGET_FILE:code-generators> ${PROJECT_SOURCE_DIR}/include/cxl 32
)

## this example showcases some of the parsing sublibrary and other misc. bits
add_executable(csv examples/csv/main.cpp)
target_include_directories(csv PUBLIC include)
target_compile_features(csv PUBLIC cxx_std_17)

add_executable(string_manip examples/string_manip/main.cpp)
target_include_directories(string_manip PUBLIC include)
target_compile_features(string_manip PUBLIC cxx_std_17)