#version 330 core
out vec4 FragColor;
precision mediump float; 

in vec3 v_frag_coord; 
in vec3 v_normal; 

uniform vec3 u_light_pos; 
uniform vec3 u_view_pos; 
float shininess = 32.0;
float spec_strength = 0.8;

void main() { 
    vec3 N = normalize(v_normal);
    vec3 L = normalize(u_light_pos - v_frag_coord); 
    vec3 V = normalize(u_view_pos - v_frag_coord); 
    vec3 R = reflect (-L,N); //max (2 * dot(N,L) * N - L , 0.0) ; \n "
    float cosTheta = dot(R , V); 
    float spec = pow(max(cosTheta,0.0), shininess); 
    float specular = spec_strength * spec;
    FragColor = vec4(vec3(1.0, 1.0, 0.0) * specular, 1.0); 
} 