#version 330

in vec4 fragViewPosition;

layout (location = 0) out vec3 depthPoint;

void main () {

    depthPoint = vec3(25.0, 25.0, 25.0); //vec3(fragViewPosition.x, fragViewPosition.y, -fragViewPosition.z);
}
