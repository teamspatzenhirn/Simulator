# Note: this is a custom CMakeLists.txt file that is
# not part of the original ObjLoader repo.

cmake_minimum_required(VERSION 3.8)

project(objloader)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_SOURCE_DIR}/extern/)
