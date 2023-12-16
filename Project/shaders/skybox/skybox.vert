#version 330 core
in vec3 position;
in vec2 tex_coords;
in vec3 normal;

//only P and V are necessary
uniform mat4 V;
uniform mat4 P;

out vec3 texCoord_v;

void main() {
    texCoord_v = position;
    mat4 rotation = mat4(mat3(V));
    vec4 pos = P * rotation * vec4(position, 1.0);
    gl_Position = pos.xyww;
}