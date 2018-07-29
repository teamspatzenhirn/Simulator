#version 330

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 textureCoord;

out vec2 fragTextureCoord;

void main () {

    fragTextureCoord = textureCoord;
    gl_Position = vec4(vertex, 1);
}
