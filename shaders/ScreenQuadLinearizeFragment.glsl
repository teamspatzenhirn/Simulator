#version 330

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;

in vec2 fragTextureCoord;

layout (location = 0) out vec4 fragColor;

void main () {

    // TODO: set near and far as uniforms

    float far = 100;
    float near = 0.1;

    fragColor = texture2D(colorTexture, fragTextureCoord);

    float depth = texture2D(depthTexture, fragTextureCoord).x;
    float z_n = 2.0 * depth - 1.0;
    float z_e = 2.0 * near * far / (far + near - z_n * (far - near));
    gl_FragDepth = z_e;
}
