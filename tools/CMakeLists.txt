project(cxl_tools)

set(SRCGEN_CXL_AGGREGATE_MAX_MEMBERS 32)

add_executable(srcgen_cxl_aggregate srcgen_cxl_aggregate.cpp)
target_compile_features(srcgen_cxl_aggregate PUBLIC cxx_std_17)
set_target_properties(srcgen_cxl_aggregate 
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_custom_command(OUTPUT ../include/cxl/aggregate.generated.h
	COMMAND $<TARGET_FILE:srcgen_cxl_aggregate> 
		ARGS ${SRCGEN_CXL_AGGREGATE_MAX_MEMBERS}
	DEPENDS srcgen_cxl_aggregate
)

add_custom_target(run
	ALL
    DEPENDS ../include/cxl/aggregate.generated.h
)