cmake_minimum_required(VERSION 3.8)
project(CppGen)

set(lib_namespace cpg)

add_library(${PROJECT_NAME} STATIC "include/${PROJECT_NAME}.h" "source/${PROJECT_NAME}.cpp")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_NAMESPACE=${lib_namespace})
target_include_directories(${PROJECT_NAME} PUBLIC "include" PRIVATE "source")
target_link_libraries(${PROJECT_NAME} PUBLIC pugixml liblua)

include(CTest)
include("CMakeUtils.cmake")

add_subdirectory("lib")
add_subdirectory("tests")
add_subdirectory("source")

add_executable(${PROJECT_NAME}-cli "main.cpp")
target_link_libraries(${PROJECT_NAME}-cli PUBLIC ${PROJECT_NAME})
target_compile_definitions(${PROJECT_NAME}-cli PUBLIC PROJECT_NAMESPACE=${lib_namespace})

install(TARGETS ${PROJECT_NAME}-cli DESTINATION "bin")
install(TARGETS ${PROJECT_NAME} DESTINATION "lib")
install(FILES "include/${PROJECT_NAME}.h" DESTINATION "include")
