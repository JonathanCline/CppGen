cmake_minimum_required(VERSION 3.8)

project(JCGenCore)

set(lib_cxx 20)

set(lib_header "include/${PROJECT_NAME}.h")

add_library(${PROJECT_NAME} STATIC "${lib_header}" "source/${PROJECT_NAME}.cpp")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_${lib_cxx})
target_include_directories(${PROJECT_NAME} PUBLIC "include" PRIVATE "source")

install(TARGETS ${PROJECT_NAME} DESTINATION "lib")
install(FILES ${lib_header} DESTINATION "include")
