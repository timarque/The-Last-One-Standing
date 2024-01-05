#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 v_frag_coord;
out vec3 v_normal;
out vec4 fragPosLightSpace;   // Where are we in the light space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 itM;
uniform mat4 lightSpaceMatrix;  // Used to tranform coords into light space (for shadows)

void main()
{
    TexCoords = aTexCoords;    
    vec4 posModel = model * vec4(aPos, 1.0);
    gl_Position = projection * view * posModel;
    v_normal = mat3(transpose(inverse(model))) * aNormal;
    vec4 frag_coord = model*vec4(aPos, 1.0);
    v_frag_coord = frag_coord.xyz;
    fragPosLightSpace = lightSpaceMatrix * vec4(v_frag_coord, 1.0);
}