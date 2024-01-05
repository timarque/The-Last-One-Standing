#version 330 core
in vec3 position;
in vec2 tex_coords;
in vec3 normal;
out vec3 v_frag_coord;
out vec3 v_normal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){ 
	vec4 frag_coord = model*vec4(position, 1.0);
	gl_Position = projection*view*frag_coord;
	v_normal = mat3(transpose(inverse(model))) * normal;
	v_frag_coord = frag_coord.xyz;
}