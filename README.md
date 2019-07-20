# SpatzSim

## Build

SpatzSim is build using CMake (version > 3.0). All major dependencies are
included in the git repository. For creating a window the xorg development
sources must be installed on the system. On Ubuntu these can be installed with
xorg-dev package. To build the simulator execute the following in the project
directory:

```
mkdir build
cd build
cmake ..
make
```

## External libraries:

### GLEW

Used to load OpenGL extensions.

http://glew.sourceforge.net/
Using cmake-version https://github.com/Perlmint/glew-cmake

### GLFW

Provides an OpenGL context, a window and simple input handling.

https://www.glfw.org/
https://github.com/glfw/glfw

### GLM

GLSL like matrix and vector math library for C++.

https://github.com/g-truc/glm
https://glm.g-truc.net/0.9.9/index.html

### ImGui

Immediate mode OpenGL gui library.

https://github.com/ocornut/imgui

### OBJ-Loader

Loads 3D model in the .obj (and corresponding .mtl) file format.

https://github.com/Bly7/OBJ-Loader

### Json11

A json library for modern C++. Used to save and load configurations.

https://github.com/nlohmann/json
