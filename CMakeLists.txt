cmake_minimum_required(VERSION 3.10)

project(cxl)

## this tool is our code-generator for aggregate sublibrary
add_executable(aggregate.generator tools/code-generators/aggregate.generator.cpp)
target_compile_features(aggregate.generator PUBLIC cxx_std_17)
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	target_link_libraries(aggregate.generator PUBLIC stdc++fs)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
endif()

## this command calls aggregate.generator produced executable
add_custom_command(TARGET aggregate.generator POST_BUILD
	COMMAND $<TARGET_FILE:aggregate.generator> ${PROJECT_SOURCE_DIR}/include/cxl 32
)

## this example showcases some of the parsing sublibrary and other misc. bits
add_executable(csv examples/csv/main.cpp)
target_include_directories(csv PUBLIC include)
target_compile_features(csv PUBLIC cxx_std_17)

add_executable(string_manip examples/string_manip/main.cpp)
target_include_directories(string_manip PUBLIC include)
target_compile_features(string_manip PUBLIC cxx_std_17)

add_executable(struct_manip examples/struct_manip/main.cpp)
target_include_directories(struct_manip PUBLIC include)
target_compile_features(struct_manip PUBLIC cxx_std_17)
add_dependencies(struct_manip aggregate.generator)