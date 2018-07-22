#version 330

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

out vec4 fragColor;

void main () {

    // implements phong shading

    vec3 L = normalize(lightPosition - fragPosition.xyz);
    vec3 V = normalize(fragCameraPosition - fragPosition.xyz);
    vec3 R = reflect(-L, fragNormal);

    // the ambient part is not following the phong shading model
    // here, kd is also used to compensate for blender not exporting
    // a color in the ka value of .obj files
    vec3 ambient = kd * ia; 

    vec3 diffuse = kd * id * max(dot(fragNormal, L), 0);
    vec3 specular = ks * is * pow(max(dot(V, R), 0), ns);

    fragColor = vec4(ambient + diffuse + specular, 1.0);
}
