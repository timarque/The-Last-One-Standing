//
// Created by Merlin Hannon on 16/12/2023.
//

#ifndef CUBEMAP_H
#define CUBEMAP_H


#include <map>
#include "Object.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

class CubeMap : public Object {

protected:
    GLuint cubeMapTextureID;

public:
    CubeMap(const char *modelPath, Shader *shader, bool texture = true) : Object(modelPath, shader, texture) {}

    /**
     * Applies a texture on all the faces of the cube map
     * @param texturePath is the path to the folder containing the 6 images
     */
    void addTexture(std::string* texturePath) {
        glGenTextures(1, &cubeMapTextureID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);

        // texture parameters
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::map<std::string, GLenum> facesToLoad = {
                {*texturePath + "posx.jpg",GL_TEXTURE_CUBE_MAP_POSITIVE_X},
                {*texturePath + "posy.jpg",GL_TEXTURE_CUBE_MAP_POSITIVE_Y},
                {*texturePath + "posz.jpg",GL_TEXTURE_CUBE_MAP_POSITIVE_Z},
                {*texturePath + "negx.jpg",GL_TEXTURE_CUBE_MAP_NEGATIVE_X},
                {*texturePath + "negy.jpg",GL_TEXTURE_CUBE_MAP_NEGATIVE_Y},
                {*texturePath + "negz.jpg",GL_TEXTURE_CUBE_MAP_NEGATIVE_Z},
        };

        //load the six faces
        for (std::pair<std::string, GLenum> pair : facesToLoad) {
            loadCubeMapFace(pair.first.c_str(), pair.second);
        }
    }

    GLuint getTexture() { return cubeMapTextureID; }

    /**
     * Draws the cubeMap in the window
     */
    void draw(Camera *camera) override {
        shader->use();
        shader->setMatrix4("V", camera->GetViewMatrix());
        shader->setMatrix4("P", camera->GetProjectionMatrix());
        shader->setInteger("cubemapTexture", 0);

        glDepthFunc(GL_LEQUAL);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
        Object::draw(camera);
        glDepthFunc(GL_LESS);
    }

private:
    /**
     * Assigns an image to one face of the cube map.
     * @param path is the path of the image to load
     * @param targetFace is the face of the cube we want the image to be on
     */
    static void loadCubeMapFace(const char *path, const GLenum& targetFace) {
        int imWidth, imHeight, imNrChannels;
        unsigned char* data = stbi_load(path, &imWidth, &imHeight, &imNrChannels, 0);
        if (data) {
            glTexImage2D(targetFace, 0, GL_RGB, imWidth, imHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else {
            std::cout << "Failed to Load texture" << std::endl;
            const char* reason = stbi_failure_reason();
            std::cout << reason << std::endl;
        }
        stbi_image_free(data);
    }
};


#endif //CUBEMAP_H
