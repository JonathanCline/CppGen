option(INCLUDE_CODEGEN ON)

set(test_folders )
SUBDIRLIST(test_folders ${CMAKE_CURRENT_SOURCE_DIR})

set(test_counter 0)
foreach(test_folder_path IN LISTS test_folders)
	NEWTEST(${test_folder_path} "${test_folder_path}/test.cpp")
	MATH(EXPR test_counter "${test_counter}+1")
endforeach()

if (INCLUDE_CODEGEN)
	target_sources(parse_test PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/parse/output.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/parse/output.cpp")
	target_link_libraries(parse_test PUBLIC pugixml)
endif()
