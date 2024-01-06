/* This code was inspired from
   https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
   The goal of this shader is to have a very simple shader to render the depth map
   of the lights efficiently 
*/

#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}