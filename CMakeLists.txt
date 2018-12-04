cmake_minimum_required(VERSION 3.10)

project(cxl)

add_executable(srcgen_cxl_aggregate tools/srcgen_cxl_aggregate.cpp)
target_compile_features(srcgen_cxl_aggregate PUBLIC cxx_std_17)

add_custom_target(tools ALL
	COMMAND $<TARGET_FILE:srcgen_cxl_aggregate>
		ARGS ${CMAKE_CURRENT_SOURCE_DIRECTORY}/include/cxl 32
)
add_dependencies(tools srcgen_cxl_aggregate)

add_executable(csv examples/csv/main.cpp)
target_include_directories(csv PUBLIC include)
target_compile_features(csv PUBLIC cxx_std_17)

add_executable(fake_class examples/fake_class/main.cpp)
target_include_directories(fake_class PUBLIC include)
target_compile_features(fake_class PUBLIC cxx_std_17)