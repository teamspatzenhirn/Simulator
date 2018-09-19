cmake_minimum_required(VERSION 2.8)

project(CaroloSimulator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-unused-function -Wno-unused-parameter -Wextra -Wpedantic -std=c++11")

include_directories(src/)

file(GLOB SOURCE_FILES
        src/imgui/*.cpp
        src/imgui/*.h
        src/helpers/*.cpp
        src/helpers/*.h
        src/modules/*.cpp
        src/modules/*.h
        src/sharedmem/*.cpp
        src/sharedmem/*.h
        src/*.h
        src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} 
    GL
    GLEW
    glfw)

add_custom_target(run
    COMMAND if [ \"$ENV{VNCDESKTOP}\" ]\;
        # this make the simulator run via VNC for reasons i do not comprehend
        # otherwise crashes occur
        then vglrun -d :0 ${CMAKE_BINARY_DIR}/${PROJECT_NAME}\;
        else ${CMAKE_BINARY_DIR}/${PROJECT_NAME}\;
    fi
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ../${CMAKE_PROJECT_DIR})
