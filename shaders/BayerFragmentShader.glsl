#version 330

uniform bool lighting = true;

uniform vec3 lightPosition;
uniform vec3 ia;
uniform vec3 id;
uniform vec3 is;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float ns;

in vec4 fragPosition;
in vec3 fragNormal;
in vec2 fragTextureCoord;
in vec3 fragCameraPosition;

layout (pixel_center_integer) in vec4 gl_FragCoord;

layout (location = 0) out vec4 fragColor;

void main () {

    if (lighting) {
        // implements phong shading

        vec3 L = normalize(lightPosition - fragPosition.xyz);
        vec3 V = normalize(fragCameraPosition - fragPosition.xyz);
        vec3 R = reflect(-L, fragNormal);

        // the ambient part is not following the phong shading model
        // here, instead kd is used to compensate for blender not
        // exporting a color in the ka value of .obj files
        vec3 ambient = kd * ia; 

        vec3 diffuse = kd * id * max(dot(fragNormal, L), 0);
        vec3 specular = ks * is * pow(max(dot(V, R), 0), ns);

        fragColor = vec4(ambient + diffuse + specular, 1.0);
    } else {
        fragColor = vec4(kd, 1.0);
    }
    
    // bayer conversion
    // depending on the position in the image either only
    // the red, blue or green part of the pixel is kept

    if (int(gl_FragCoord.y) % 2 == 0) {
       if (int(gl_FragCoord.x) % 2 == 0) {
           fragColor = vec4(fragColor.r, 0, 0, 1);
       } else {
           fragColor = vec4(fragColor.g, 0, 0, 1);
       }
    } else {
       if (int(gl_FragCoord.x) % 2 == 0) {
           fragColor = vec4(fragColor.g, 0, 0, 1);
       } else {
           fragColor = vec4(fragColor.b, 0, 0, 1);
       }
    }
}
