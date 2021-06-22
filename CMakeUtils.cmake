macro(SUBDIRLIST result curdir)
	file(GLOB children RELATIVE ${curdir} ${curdir}/*)
	set(dirlist "")
	foreach(child ${children})
		if(IS_DIRECTORY ${curdir}/${child})
			list(APPEND dirlist ${child})
		endif()
	endforeach()
	set(${result} ${dirlist})
endmacro()


macro(NEWTEST tname tsource)
	add_executable(${tname}_test ${tsource})
	target_link_libraries(${tname}_test PUBLIC ${PROJECT_NAME})
	target_compile_definitions(${tname}_test PRIVATE
		SOURCE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/${tname}/"
		PROJECT_NAMESPACE=${lib_namespace})
	add_test(${tname}_CTest ${tname}_test)
endmacro(NEWTEST)

#
#	Adds a list of sources to a target, sourceList should be a list variable
#
macro(ADD_SOURCES_LIST targetName sourceList)
	list(TRANSFORM ${sourceList} PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
	set(lfefiles )
	foreach(lfe IN LISTS ${sourceList})
		set(lfefiles ${lfefiles} ${lfe})
	endforeach()
	target_sources(${targetName} PRIVATE ${lfefiles})
endmacro(ADD_SOURCES_LIST)

#
#	Adds a list of subdirectories to the project, pathList should be a list variable
#
macro(ADD_SUBDIRS_LIST pathList)
	foreach(lfe IN LISTS ${pathList})
		add_subdirectory(${lfe})
	endforeach()
endmacro(ADD_SUBDIRS_LIST)

#
#	Includes all subdirectories from the current source path
#
macro(ADD_SUBDIRS_HERE)
	set(dirlist )
	SUBDIRLIST(dirlist ${CMAKE_CURRENT_SOURCE_DIR})
	foreach(lfe IN LISTS dirlist)
		set(lfename )		
		get_filename_component(lfename ${lfe} NAME)
		add_subdirectory(${lfename})
	endforeach()
endmacro()
