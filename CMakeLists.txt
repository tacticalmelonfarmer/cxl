cmake_minimum_required(VERSION 3.10)

project(cxl)

add_executable(srcgen_cxl_aggregate tools/srcgen_cxl_aggregate.cpp)
target_compile_features(srcgen_cxl_aggregate PUBLIC cxx_std_17)
set_target_properties(srcgen_cxl_aggregate
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "$<TARGET_PROPERTY:srcgen_cxl_aggregate,SOURCE_DIR>/tools"
)

add_custom_target(tools
	COMMAND $<TARGET_FILE:srcgen_cxl_aggregate> 32
	DEPENDS srcgen_cxl_aggregate
)

add_executable(csv examples/csv/main.cpp)
target_include_directories(csv PUBLIC include)
target_compile_features(csv PUBLIC cxx_std_17)

add_executable(fake_class examples/fake_class/main.cpp)
add_dependencies(fake_class tools)
target_include_directories(fake_class PUBLIC include)
target_compile_features(fake_class PUBLIC cxx_std_17)