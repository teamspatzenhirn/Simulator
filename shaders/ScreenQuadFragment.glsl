#version 330

uniform sampler2D tex0;
uniform sampler2D tex1;

in vec2 fragTextureCoord;

layout (location = 0) out vec4 fragColor;

void main () {

    vec4 texel1 = texture(tex1, fragTextureCoord);

    if (texel1.a != 0.0) {
        fragColor = texel1;
    } else {
        fragColor = texture(tex0, fragTextureCoord);
    }
}
