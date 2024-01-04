#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aColor;


// Outputs the color for the Fragment Shader
out vec3 color;

// Imports the camera matrix from the main function


void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = view * projection * vec4(aPos, 1.0);
	// Assigns the colors from the Vertex Data to "color"
	color = aPos;
}