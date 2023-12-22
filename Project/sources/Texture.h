#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>


#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


class Texture {

private:
	GLuint id;
	int imWidth;
	int imHeight;
	int imNrChannels;

public:
	// celui la n est pas actif pour le moment mais le code ne marche pas sans :) (mais c est bien celui d en bas qui est prit la)
	Texture() {
		const char* file = "../../Project/textures/tronMoto.jpg";
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(file, &imWidth, &imHeight, &imNrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_3D, 0, GL_RGB, imWidth, imHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_3D);
		}
		else {
			std::cout << "Failed to Load texture" << std::endl;
			const char* reason = stbi_failure_reason();
			std::cout << reason << std::endl;
		}

		stbi_image_free(data);

		glGenTextures(1, &this->id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, this->id);
		
		//3. Define the parameters for the texture
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	};

	Texture(const char* file) {

		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(file, &imWidth, &imHeight, &imNrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imWidth, imHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << file << std::endl;
			std::cout << "Failed to Load texture" << std::endl;
			const char* reason = stbi_failure_reason();
			std::cout << reason << std::endl;
		}

		stbi_image_free(data);

		glGenTextures(1, &this->id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->id);

		//3. Define the parameters for the texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	};

	~Texture() { glDeleteTextures(1, &this->id); };

	GLuint getId() { return this->id; };

};


#endif