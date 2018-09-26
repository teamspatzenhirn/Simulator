#version 330

uniform sampler2D tex;

in vec2 fragTextureCoord;

layout (location = 0) out vec4 fragColor;

void main () {

    fragColor = texture(tex, fragTextureCoord);
}
