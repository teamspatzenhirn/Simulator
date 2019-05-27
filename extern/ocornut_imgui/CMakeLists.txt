# Note: this is a custom CMakeLists.txt file that is
# not part of the original imgui repo.

# This builds the glew + glfw + opengl3 version of imgui.

cmake_minimum_required(VERSION 3.6)

project(imgui)

add_library(${PROJECT_NAME} SHARED
    imconfig.h
    imgui.cpp
    imgui.h
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    imgui_internal.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    examples/imgui_impl_glfw.cpp
    examples/imgui_impl_glfw.h
    examples/imgui_impl_opengl3.cpp
    examples/imgui_impl_opengl3.h
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE -DIMGUI_IMPL_OPENGL_LOADER_GLEW
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${SPATZSIM_PROJECT_DIR}/extern/
    PRIVATE ${SPATZSIM_PROJECT_DIR}/extern/ocornut_imgui
    PRIVATE ${SPATZSIM_PROJECT_DIR}/extern/ocornut_imgui/examples
)

target_link_libraries(${PROJECT_NAME} glfw)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
