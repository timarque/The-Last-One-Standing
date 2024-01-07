#version 330 core
out vec4 FragColor;
precision mediump float;

in vec3 v_frag_coord;
in vec3 v_normal;
in vec4 fragPosLightSpace;	// Where are we in the light space

uniform vec3 u_view_pos;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D shadowMap;	// This is the depth map

struct Light {
	vec3 light_pos;
	float ambient_strength;
	float diffuse_strength;
	float specular_strength;
	float constant;
	float linear;
	float quadratic;
};

uniform Light light;

uniform float shininess;
uniform vec3 materialColour;


float specularCalculation(vec3 N, vec3 L, vec3 V){
	vec3 R = reflect(-L, N);
	float cosTheta = dot(R, V);
	float spec = pow(max(cosTheta, 0.0), shininess);
	return light.specular_strength * spec;
}


float isInShadow(vec4 fragPosLightSpace) {
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;	// maps to values in [-1, 1]
	projCoords = projCoords * 0.5 + 0.5; // maps to values between [0, 1]
	float closestDepth = texture(shadowMap, projCoords.xy).r;	// Depth of the closest object from light in that direction
	float currentDepth = projCoords.z;	// Distance between current object and light
	//float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
	
	float bias = 0.005; // Adjust the bias as needed
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	
	return shadow;
}


void main()
{    
	vec3 N = normalize(v_normal); 
	vec3 L = normalize(light.light_pos - v_frag_coord);
	vec3 V = normalize(u_view_pos - v_frag_coord);

	vec3 normalMap = normalize(texture(texture_normal1, TexCoords).xyz * 2.0 - 1.0);
	//normalMap.z = normalMap.z - 0.995;
	//normalMap.z = normalMap.z * 100; // it seems like all the z coords are between 0.98 and 1.0
    N = normalize(N + normalMap);

	float specular = specularCalculation(N, L, V);
	float diffuse = light.diffuse_strength * max(dot(N,L), 0.0);
	float distance = length(light.light_pos - v_frag_coord);
	float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	float shadow = isInShadow(fragPosLightSpace);
	float light = light.ambient_strength + (1.0 - shadow) * attenuation * (diffuse + specular);
    FragColor = texture(texture_diffuse1, TexCoords) * light;
}