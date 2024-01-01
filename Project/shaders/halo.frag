#version 330 core
out vec4 FragColor;

uniform vec3 objectColor; // Couleur de l'objet
uniform vec3 haloColor;   // Couleur du halo

void main()
{
    float distance = length(gl_PointCoord - 0.5);
    float haloRadius = 0.1;

    vec3 halo = mix(objectColor, haloColor, smoothstep(0.5 - haloRadius, 0.5 + haloRadius, distance));

    FragColor = vec4(halo, 1.0);
}