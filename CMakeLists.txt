cmake_minimum_required(VERSION 3.10)

project(cxl)

set(IncludeDir ${PROJECT_SOURCE_DIR}/include)
set(ExamplesDir ${PROJECT_SOURCE_DIR}/examples)
set(ToolsDir ${PROJECT_SOURCE_DIR}/tools)

## this tool is our code-generator for aggregate sublibrary
add_executable(aggregate.generator ${ToolsDir}/code-generators/aggregate.generator.cpp)
target_compile_features(aggregate.generator PUBLIC cxx_std_17)
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	target_link_libraries(aggregate.generator PUBLIC stdc++fs)
endif()

## this command calls aggregate.generator produced executable
add_custom_command(TARGET aggregate.generator POST_BUILD
	COMMAND $<TARGET_FILE:aggregate.generator> destructure ${IncludeDir}/cxl 32
	COMMAND $<TARGET_FILE:aggregate.generator> make_struct ${IncludeDir}/cxl 32
)

add_library(cxl INTERFACE)
target_include_directories(cxl INTERFACE ${IncludeDir})
target_compile_features(cxl INTERFACE cxx_std_17)
add_dependencies(cxl INTERFACE aggregate.generator)

## this example showcases some of the parsing sublibrary and other misc. bits
add_executable(csv ${ExamplesDir}/csv/main.cpp)
target_link_libraries(csv cxl)

add_executable(string_manip ${ExamplesDir}/string_manip/main.cpp)
target_link_libraries(string_manip cxl)

add_executable(struct_manip ${ExamplesDir}/struct_manip/main.cpp)
target_link_libraries(struct_manip cxl)