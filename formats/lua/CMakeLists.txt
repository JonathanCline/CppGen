cmake_minimum_required(VERSION 3.8)
project(JCGenLua)

set(lib_cxx 20)
set(lib_link_public 
	JCGenCore)

add_library(${PROJECT_NAME}-static STATIC "include/${PROJECT_NAME}.h" "source/${PROJECT_NAME}.cpp")
#add_library(${PROJECT_NAME}-shared SHARED "include/${PROJECT_NAME}.h" "source/${PROJECT_NAME}.cpp")
install(FILES "include/${PROJECT_NAME}.h" DESTINATION "include")

set(lib_targets
	${PROJECT_NAME}-static
	#${PROJECT_NAME}-shared
)

foreach(lib IN LISTS lib_targets)
	target_compile_features(${lib} PUBLIC cxx_std_${lib_cxx})
	target_compile_definitions(${lib} PRIVATE JCGEN_FORMAT_NAME=${PROJECT_NAME} PUBLIC JCGEN_FORMAT_${PROJECT_NAME}=true)
	target_include_directories(${lib} PUBLIC "include" PRIVATE "source")
	target_link_libraries(${lib} PUBLIC ${lib_link_public} PRIVATE liblua)

	install(TARGETS ${lib} DESTINATION "lib")
endforeach()
