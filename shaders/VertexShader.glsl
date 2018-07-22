#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMat;
uniform mat4 projection;
uniform vec3 cameraPosition;

uniform bool billboard = false;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoord;

out vec4 fragPosition;
out vec3 fragNormal;
out vec2 fragTextureCoord;
out vec3 fragCameraPosition;

void main () {

    fragTextureCoord = textureCoord;
    fragCameraPosition = cameraPosition;

    if (billboard) {
        /*
         * This transforms each vertex along the coordinate axis 
         * of the camera coordiante system. Therefore the model
         * will always be aligned with the viewing direction of
         * the camera. This will result in the typical "billboard"
         * effect which can be used for objects that should always
         * face the camera.
         */

        vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
        vec3 up = vec3(view[0][1], view[1][1], view[2][1]);
        vec3 eye = vec3(view[0][2], view[1][2], view[2][2]);

        vec3 center = vec3(model * vec4(0, 0, 0, 1));
        mat4 rotScaleModelMat = mat4(
            model[0].xyz, 0, model[1].xyz, 0, model[2].xyz, 0, vec3(0, 0, 0), 1);
        vec3 modVertex = vec3(rotScaleModelMat * vec4(vertex, 1));

        fragNormal = normalize(vec3(right * normal.x + up * normal.y + eye * normal.z));
        fragPosition = vec4(
            center
            + right * modVertex.x
            + up * modVertex.y
            + eye * modVertex.z, 1.0);
        gl_Position = projection * view * fragPosition;
    } else {
        fragNormal = normalize(normalMat * normal);
        fragPosition = model * vec4(vertex, 1);
        gl_Position = projection * view * fragPosition;
    }
}
