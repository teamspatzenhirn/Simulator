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

uniform float time;

uniform float noise = 0.0;

in vec4 fragPosition;
in vec3 fragNormal;
in vec2 fragTextureCoord;
in vec3 fragCameraPosition;

layout (location = 0) out vec4 fragColor;

float rand (vec2 co) {

    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * (43758.5453 + time));
}

void main () {

    if (lighting) {
        // implements blinn-phong shading
        
        vec3 lightFragVector = lightPosition - fragPosition.xyz / fragPosition.w;
        float d = length(lightFragVector);

        vec3 L = lightFragVector / d;
        vec3 V = normalize(fragCameraPosition - fragPosition.xyz / fragPosition.w);
        vec3 H = normalize(L + V); 

        // the ambient part is not following the phong shading model
        // here, instead kd is used to compensate for blender not
        // exporting a color in the ka value of .obj files
        vec3 ambient = kd * ia; 

        vec3 diffuse = kd * id * max(dot(fragNormal, L), 0.0);
        vec3 specular = ks * is * pow(max(dot(fragNormal, H), 0.0), ns);

        // account for distance

        diffuse = diffuse / d / d;
        specular = specular / d / d;

        // gamma correction

        vec3 colorLinear = ambient + diffuse + specular * 10000;
        float gamma = 1.3;
        vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0/gamma));

        fragColor = vec4(colorGammaCorrected, 1.0);
    } else {
        fragColor = vec4(kd, 1.0);
    }

    // additive noise

    float noiseColor = rand(vec2(gl_FragCoord));

    fragColor = fragColor * (1 - noise) + noiseColor * noise;
}
