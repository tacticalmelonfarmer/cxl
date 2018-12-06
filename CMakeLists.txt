cmake_minimum_required(VERSION 3.10)

project(cxl)

## this tool is our code-generator for aggregate sublibrary
add_executable(srcgen_cxl_aggregate tools/srcgen_cxl_aggregate.cpp)
target_compile_features(srcgen_cxl_aggregate PUBLIC cxx_std_17)
set_target_properties(srcgen_cxl_aggregate
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "$<TARGET_PROPERTY:srcgen_cxl_aggregate,SOURCE_DIR>/tools"
)

## this here command is commented out... reason bein, we can't run it before buildin targets
#add_custom_command(TARGET srcgen_cxl_aggregate POST_BUILD
#	COMMAND $<TARGET_FILE:srcgen_cxl_aggregate> 32
#)

## this target calls our custom build tools, it has dependencies that must be built before it can be run
add_custom_target(tools
	COMMAND $<TARGET_FILE:srcgen_cxl_aggregate> 32
	DEPENDS srcgen_cxl_aggregate
)

## this example showcases some of the parsing sublibrary and other misc. bits
add_executable(csv examples/csv/main.cpp)
target_include_directories(csv PUBLIC include)
target_compile_features(csv PUBLIC cxx_std_17)
add_dependencies(csv tools) # this forces our code generator to be compiled and called
