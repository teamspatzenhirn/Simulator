cmake_minimum_required(VERSION 2.8)
project(CaroloSimulator)

set(BUILD_SHARED_LIBS 1)

# ---[ Check for OpenGL (mandatory) ]---

find_package(OpenGL QUIET)
if(OPENGL_FOUND)
  message(STATUS "Found OpenGL: " ${OPENGL_LIBRARIES})
  message(STATUS "              " ${OPENGL_INCLUDE_DIR})
else(OPENGL_FOUND)
  message(FATAL_ERROR "${ColourBoldRed}OpenGL missing.${ColourReset}")
endif()

# ---[ Check for GLEW (mandatory) ]---

find_package(GLEW QUIET)
if(GLEW_FOUND)
  message(STATUS "Found GLEW: " ${GLEW_LIBRARIES})
  message(STATUS "            " ${GLEW_INCLUDE_DIRS})
else(GLEW_FOUND)
  message(FATAL_ERROR "${ColourBoldRed}GLEW missing.${ColourReset}")
endif()

# --- [ External libs ]---

# Order matters here!
# Because imgui depends of glfw, glfw must be added first.

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

message(STATUS "External lib: extern/glfw_glfw")
add_subdirectory(extern/glfw_glfw)

set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)

message(STATUS "External lib: extern/g-truc_glm")
add_subdirectory(extern/g-truc_glm)

set(JSON_BuildTests OFF CACHE BOOL "" FORCE)

message(STATUS "External lib: extern/nlohmann_json")
add_subdirectory(extern/nlohmann_json)

message(STATUS "External lib: extern/bly7_obj-loader")
add_subdirectory(extern/bly7_obj-loader)

message(STATUS "External lib: extern/ocornut_imgui")
add_subdirectory(extern/ocornut_imgui)

# Collect files.

set(SOURCE_FILES
   ./src/sharedmem/shmcomm.cpp
   ./src/helpers/Capture.cpp
   ./src/helpers/Input.cpp
   ./src/helpers/Shader.cpp
   ./src/helpers/Model.cpp
   ./src/helpers/Camera.cpp
   ./src/helpers/ShaderProgram.cpp
   ./src/helpers/FpsCamera.cpp
   ./src/helpers/FrameBuffer.cpp
   ./src/helpers/Timer.cpp
   ./src/helpers/Id.cpp
   ./src/helpers/Pose.cpp
   ./src/helpers/PointLight.cpp
   ./src/helpers/ScreenQuad.cpp
   ./src/Scene.cpp
   ./src/Loop.cpp
   ./src/modules/Editor.cpp
   ./src/modules/CommModule.cpp
   ./src/modules/GuiModule.cpp
   ./src/modules/ItemsModule.cpp
   ./src/modules/CarModule.cpp
   ./src/modules/CollisionModule.cpp
   ./src/modules/MarkerModule.cpp
   ./src/modules/RuleModule.cpp
   ./src/modules/VisModule.cpp
   ./src/scene/Tracks.cpp
   ./src/scene/Settings.cpp
   )

set(HEADER_FILES
    ./src/sharedmem/shmcomm.h
    ./src/Loop.h
    ./src/helpers/Model.h
    ./src/helpers/Helpers.h
    ./src/helpers/FpsCamera.h
    ./src/helpers/ShaderProgram.h
    ./src/helpers/Input.h
    ./src/helpers/Capture.h
    ./src/helpers/Dirent.h
    ./src/helpers/Camera.h
    ./src/helpers/ScreenQuad.h
    ./src/helpers/PointLight.h
    ./src/helpers/Pose.h
    ./src/helpers/FrameBuffer.h
    ./src/helpers/Timer.h
    ./src/helpers/Id.h
    ./src/helpers/Shader.h
    ./src/Scene.h
    ./src/modules/Editor.h
    ./src/modules/GuiModule.h
    ./src/modules/MarkerModule.h
    ./src/modules/CommModule.h
    ./src/modules/CollisionModule.h
    ./src/modules/CarModule.h
    ./src/modules/ItemsModule.h
    ./src/modules/RuleModule.h
    ./src/modules/VisModule.h
    ./src/scene/Tracks.h
    ./src/scene/Settings.h
    ./src/scene/Car.h
    ./src/scene/ModelStore.h
   )

# Build the library/program.

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME}
                      ${OPENGL_LIBRARIES}
                      ${GLEW_LIBRARIES}
                      glm
                      glfw
                      imgui
                      objloader
                      nlohmann_json)

target_include_directories(${PROJECT_NAME}
                           PUBLIC src/
                           PUBLIC ${OPENGL_INCLUDE_DIR}
                           PUBLIC ${GLEW_INCLUDE_DIRS})

target_compile_options(${PROJECT_NAME} PUBLIC -Wall)
target_compile_options(${PROJECT_NAME} PUBLIC -Wextra)
target_compile_options(${PROJECT_NAME} PUBLIC -Wpedantic)
target_compile_options(${PROJECT_NAME} PUBLIC -Wconversion)
target_compile_options(${PROJECT_NAME} PUBLIC -Wunreachable-code)
target_compile_options(${PROJECT_NAME} PUBLIC -std=c++17)
target_compile_options(${PROJECT_NAME} PUBLIC -fPIC)

# Compile type dependent (release or debug) flags.

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(${PROJECT_NAME} PUBLIC -g)
  target_compile_options(${PROJECT_NAME} PUBLIC -O0)
else()
  target_compile_options(${PROJECT_NAME} PUBLIC -O3)
  target_compile_options(${PROJECT_NAME} PUBLIC -mfpmath=sse)
endif()

# Exports compile commands to .json file for YouCompleteMe support.

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_custom_target(run
    COMMAND if [ \"$ENV{VNCDESKTOP}\" ]\;
        # this make the simulator run via VNC for reasons i do not comprehend
        # otherwise crashes occur
        then vglrun -d :0 ${CMAKE_BINARY_DIR}/${PROJECT_NAME}\;
        else ${CMAKE_BINARY_DIR}/${PROJECT_NAME}\;
    fi
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ../${CMAKE_PROJECT_DIR})
