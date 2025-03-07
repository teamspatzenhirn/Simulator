cmake_minimum_required(VERSION 3.0)
project(spatzsim)

set(CMAKE_CXX_STANDARD 17)

set(EXE_TARGET_NAME "${PROJECT_NAME}")
set(LIB_TARGET_NAME "lib${PROJECT_NAME}")
set(PY_TARGET_NAME "py${PROJECT_NAME}")

# Define uninstall target here to prevent glm from creating
# a target with the same name
# TODO: does not remove created directories
add_custom_target(uninstall COMMAND xargs -a install_manifest.txt -i echo rm -v {})

# ---[ Check for OpenGL (mandatory) ]---

set(OpenGL_GL_PREFERENCE "GLVND")

find_package(OpenGL QUIET)
if (OPENGL_FOUND)
    message(STATUS "Found OpenGL: " ${OPENGL_LIBRARIES})
    message(STATUS "              " ${OPENGL_INCLUDE_DIR})
else (OPENGL_FOUND)
    message(FATAL_ERROR "${ColourBoldRed}OpenGL missing.${ColourReset}")
endif ()

# ---[ Check for GLEW (mandatory) ]---

find_package(GLEW QUIET)
if (GLEW_FOUND)
    message(STATUS "Found GLEW: " ${GLEW_LIBRARIES})
    message(STATUS "            " ${GLEW_INCLUDE_DIR})
else (GLEW_FOUND)
    message(FATAL_ERROR "${ColourBoldRed}GLEW missing.${ColourReset}")
endif ()

# ---[ Check for GLFW3 (mandatory) ]---

find_package(glfw3 QUIET)
if (glfw3_FOUND)
    message(STATUS "Found GLFW3")
else (glfw3_FOUND)
    message(FATAL_ERROR "${ColourBoldRed}GLFW3 missing.${ColourReset}")
endif ()

# --- [ External libs ]---

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)

# EXCLUDE_FROM_ALL is used here to prevent execution of the
# install targets of these subdirectories
add_subdirectory(extern/g-truc_glm EXCLUDE_FROM_ALL)

find_package(pybind11 QUIET)
include(FetchContent)
if (NOT pybind11_FOUND)
    # if we did not find pybind11 as systems include
    # download it from the inter-webs ...
    FetchContent_Declare(
            pybind
            GIT_REPOSITORY "https://github.com/pybind/pybind11"
            GIT_TAG "v2.9.1"
    )
    message(STATUS "Loading pybind ...")
    FetchContent_MakeAvailable(pybind)
endif ()

# Collect files.

set(SOURCE_FILES
    ./extern/ocornut_imgui/imgui.cpp
    ./extern/ocornut_imgui/imgui_draw.cpp
    ./extern/ocornut_imgui/imgui_impl_glfw.cpp
    ./extern/ocornut_imgui/imgui_widgets.cpp
    ./extern/ocornut_imgui/imgui_impl_opengl3.cpp
    ./extern/ocornut_imgui/imgui_stdlib.cpp
    ./src/sharedmem/shmcomm.cpp
    ./src/helpers/Capture.cpp
    ./src/helpers/Input.cpp
    ./src/helpers/Shader.cpp
    ./src/helpers/Model.cpp
    ./src/helpers/Camera.cpp
    ./src/helpers/ShaderProgram.cpp
    ./src/helpers/FollowCamera.cpp
    ./src/helpers/FpsCamera.cpp
    ./src/helpers/CinematicCamera.cpp
    ./src/helpers/OrthoCamera.cpp
    ./src/helpers/FrameBuffer.cpp
    ./src/helpers/Clock.cpp
    ./src/helpers/Id.cpp
    ./src/helpers/Pose.cpp
    ./src/helpers/PointLight.cpp
    ./src/helpers/ScreenQuad.cpp
    ./src/scene/Scene.cpp
    ./src/Storage.cpp
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
    ./src/modules/AutoTracksModule.cpp
    ./src/scene/Tracks.cpp
   )

set(HEADER_FILES
        ./src/sharedmem/shmcomm.h
        ./src/Loop.h
        ./src/helpers/Model.h
        ./src/helpers/Helpers.h
        ./src/helpers/FollowCamera.h
        ./src/helpers/FpsCamera.h
        ./src/helpers/CinematicCamera.h
        ./src/helpers/ShaderProgram.h
        ./src/helpers/Input.h
        ./src/helpers/Capture.h
        ./src/helpers/Camera.h
        ./src/helpers/ScreenQuad.h
        ./src/helpers/PointLight.h
        ./src/helpers/Pose.h
        ./src/helpers/FrameBuffer.h
        ./src/helpers/Clock.h
        ./src/helpers/Id.h
        ./src/helpers/Shader.h
        ./src/scene/Scene.h
        ./src/Storage.h
        ./src/modules/Editor.h
        ./src/modules/GuiModule.h
        ./src/modules/MarkerModule.h
        ./src/modules/CommModule.h
        ./src/modules/CollisionModule.h
        ./src/modules/CarModule.h
        ./src/modules/ItemsModule.h
        ./src/modules/RuleModule.h
        ./src/modules/VisModule.h
        ./src/modules/AutoTracksModule.h
        ./src/scene/Tracks.h
        ./src/scene/Settings.h
        ./src/scene/Car.h
        ./src/scene/ModelStore.h
        )

# Build the main static library.

add_library(${LIB_TARGET_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})

set_target_properties(${LIB_TARGET_NAME} PROPERTIES PREFIX "")

target_link_libraries(${LIB_TARGET_NAME}
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        stdc++fs
        glm
        glfw)

target_include_directories(${LIB_TARGET_NAME}
        PUBLIC src/
        PUBLIC extern/
        PUBLIC ${OPENGL_INCLUDE_DIR})

target_compile_options(${LIB_TARGET_NAME} PUBLIC -Wall)
target_compile_options(${LIB_TARGET_NAME} PUBLIC -Wextra)
target_compile_options(${LIB_TARGET_NAME} PUBLIC -Wpedantic)
target_compile_options(${LIB_TARGET_NAME} PUBLIC -Wunreachable-code)
target_compile_options(${LIB_TARGET_NAME} PUBLIC -std=c++17)
target_compile_options(${LIB_TARGET_NAME} PUBLIC -fPIC)
# For the really paranoid.
#target_compile_options(${PROJECT_NAME} PUBLIC -Wconversion)

target_compile_definitions(${LIB_TARGET_NAME}
        PRIVATE -DIMGUI_IMPL_OPENGL_LOADER_GLEW
        )

# Compile type dependent (release or debug) flags.

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${LIB_TARGET_NAME} PUBLIC -g)
    target_compile_options(${LIB_TARGET_NAME} PUBLIC -O0)
else ()
    target_compile_options(${LIB_TARGET_NAME} PUBLIC -O3)
    target_compile_options(${LIB_TARGET_NAME} PUBLIC -mfpmath=sse)
endif ()

# Builds the python bindings module.

pybind11_add_module(${PY_TARGET_NAME} MODULE
        python/bindings.cpp)

set_target_properties(${PY_TARGET_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/python/build")

target_link_libraries(${PY_TARGET_NAME} PUBLIC
        pybind11::module
        pybind11::embed
        ${LIB_TARGET_NAME})

# Builds the actual executable.

add_executable(${EXE_TARGET_NAME} src/main.cpp)

target_link_libraries(${EXE_TARGET_NAME} ${LIB_TARGET_NAME})

# Exports compile commands to .json file for vim YouCompleteMe support.

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Convenience target for build & execute.

add_custom_target(run
        COMMAND if [ \"$ENV{VNCDESKTOP}\" ]\;
        # This make the simulator run via VNC by using gl from display :0
        then vglrun -d :0 ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
        -r ${PROJECT_SOURCE_DIR}/
        -s test_settings.json\;
        else ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
        -r ${PROJECT_SOURCE_DIR}/
        -s test_settings.json\;
        fi
        DEPENDS ${EXE_TARGET_NAME})

# Define install target

install(TARGETS ${PROJECT_NAME} DESTINATION bin/)
install(DIRECTORY
        "${PROJECT_SOURCE_DIR}/shaders"
        "${PROJECT_SOURCE_DIR}/models"
        DESTINATION share/${PROJECT_NAME})
install(CODE "execute_process(COMMAND xdg-desktop-menu install ${CMAKE_SOURCE_DIR}/spatzenhirn-spatzsim.desktop)")
