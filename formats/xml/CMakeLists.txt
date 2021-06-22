cmake_minimum_required(VERSION 3.8)
project(JCGenXML)

set(lib_cxx 20)
set(lib_link_public 
	JCGenCore)

add_library(${PROJECT_NAME} SHARED "include/${PROJECT_NAME}.h" "source/${PROJECT_NAME}.cpp")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_${lib_cxx})
target_include_directories(${PROJECT_NAME} PUBLIC "include" PRIVATE "source")
target_link_libraries(${PROJECT_NAME} PUBLIC ${lib_link_public})


install(TARGETS ${PROJECT_NAME} DESTINATION "lib")
install(FILES "include/${PROJECT_NAME}.h" DESTINATION "include")
