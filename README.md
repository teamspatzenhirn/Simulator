# SpatzSim

## Build

SpatzSim is build using CMake (version > 3.0) and a compiler supporting C++17.
Most dependencies are included directly in the git repository. Additionally
SpatzSim requires GLFW and GLEW to be installed as system libraries. On Ubuntu
18.04 these can be installed with:

```
sudo apt install libglfw-dev libglew-dev
```

To build the simulator execute the following in the project directory:

```
mkdir build
cd build
cmake ..
make
```

To build and run the simulator afterwards the "run" target can also be used:

```
make run
```

## Used libraries:

### GLEW

Used to load OpenGL extensions.

http://glew.sourceforge.net/

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
