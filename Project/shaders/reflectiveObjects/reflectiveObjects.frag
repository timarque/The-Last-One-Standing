#version 400 core
out vec4 FragColor;
precision mediump float;
in vec3 v_frag_coord;
in vec3 v_normal; 
uniform vec3 u_view_pos; 
uniform samplerCube cubemapSampler;

in vec2 v_tex;
uniform sampler2D ourTexture;

void main() { 
	vec3 N = normalize(v_normal);
	vec3 V = normalize(u_view_pos - v_frag_coord); 
	vec3 R = reflect(-V,N); 
	vec4 refl = texture(cubemapSampler,R);
	vec4 text = texture(ourTexture, v_tex);

	FragColor = refl * text; 
}