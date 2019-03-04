cmake_minimum_required(VERSION 3.8)

add_library(imgui STATIC
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

target_compile_definitions(imgui
    PRIVATE -DIMGUI_IMPL_OPENGL_LOADER_GLEW
)

target_include_directories(imgui
    PUBLIC ${CMAKE_SOURCE_DIR}/extern/ocornut_imgui
    PUBLIC ${CMAKE_SOURCE_DIR}/extern/ocornut_imgui/examples
)
