cmake_minimum_required(VERSION 3.15)
project(filesys VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory(src)

add_compile_options("-g")
add_executable(test ${PROJECT_SOURCE_DIR}/test/testmain.cc)

target_link_libraries(test PUBLIC src)
target_include_directories(test PUBLIC  "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/src")