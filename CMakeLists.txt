cmake_minimum_required(VERSION 2.8)
project(CaroloSimulator)

# Path to cmake modules.

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake/Modules/")

# ---[ Check for OpenGL (mandatory) ]---

find_package(OpenGL QUIET)
if(OPENGL_FOUND)
  message(STATUS "Found OpenGL: " ${OPENGL_gl_LIBRARY})
  message(STATUS "              " ${OPENGL_glu_LIBRARY})
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

# ---[ Check for GLEW (mandatory) ]---

find_package(GLFW REQUIRED)
if(GLFW_FOUND)
  message(STATUS "Found GLFW: " ${GLFW_LIBRARIES})
  message(STATUS "            " ${GLFW_INCLUDE_DIR})
else(GLFW_FOUND)
  message(FATAL_ERROR "${ColourBoldRed}GLFW missing.${ColourReset}")
endif()

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
   ./src/helpers/Pose.cpp
   ./src/helpers/PointLight.cpp
   ./src/helpers/ObjLoader.cpp
   ./src/helpers/ScreenQuad.cpp
   ./src/imgui/imgui.cpp
   ./src/imgui/imgui_demo.cpp
   ./src/imgui/imgui_draw.cpp
   ./src/imgui/imgui_impl_opengl3.cpp
   ./src/imgui/imgui_impl_glfw.cpp
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
   )

set(HEADER_FILES
    ./src/sharedmem/shmcomm.h
    ./src/Loop.h
    ./src/helpers/Model.h
    ./src/helpers/Helpers.h
    ./src/helpers/Json.h
    ./src/helpers/FpsCamera.h
    ./src/helpers/ShaderProgram.h
    ./src/helpers/Input.h
    ./src/helpers/Capture.h
    ./src/helpers/ObjLoader.h
    ./src/helpers/Dirent.h
    ./src/helpers/Camera.h
    ./src/helpers/ScreenQuad.h
    ./src/helpers/PointLight.h
    ./src/helpers/Pose.h
    ./src/helpers/FrameBuffer.h
    ./src/helpers/Timer.h
    ./src/helpers/Id.h
    ./src/helpers/Shader.h
    ./src/imgui/imconfig.h
    ./src/imgui/stb_textedit.h
    ./src/imgui/imgui_impl_opengl3.h
    ./src/imgui/stb_truetype.h
    ./src/imgui/imgui_internal.h
    ./src/imgui/imgui.h
    ./src/imgui/imgui_impl_glfw.h
    ./src/imgui/stb_rect_pack.h
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
   )

# Build the library/program.

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME}
                      ${OPENGL_gl_LIBRARY}
                      ${GLEW_LIBRARIES}
                      ${GLFW_LIBRARIES})

target_include_directories(${PROJECT_NAME}
                           PUBLIC src/
                           PUBLIC ${OPENGL_INCLUDE_DIR}
                           PUBLIC ${GLEW_INCLUDE_DIRS}
                           PUBLIC ${GLFW_INCLUDE_DIR})

target_compile_options(${PROJECT_NAME} PUBLIC -Wall)
target_compile_options(${PROJECT_NAME} PUBLIC -Wextra)
target_compile_options(${PROJECT_NAME} PUBLIC -Wpedantic)
target_compile_options(${PROJECT_NAME} PUBLIC -std=c++11)
target_compile_options(${PROJECT_NAME} PUBLIC -fPIC)

# Compile type dependent (release or debug) flags.

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(${PROJECT_NAME} PUBLIC -g)
  target_compile_options(${PROJECT_NAME} PUBLIC -O0)
else()
  target_compile_options(${PROJECT_NAME} PUBLIC -march=native)
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
