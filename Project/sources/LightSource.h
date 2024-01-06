#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "PhysicModel.h"

class LightSource {

private:

    const unsigned int SHADOW_WIDTH  = 8192; // resolution of depth map
    const unsigned int SHADOW_HEIGHT = 8192;
    unsigned int depthMapFBO;   // Frame buffer used to create the depth map
    glm::vec3 lightPosition;
    const glm::vec3 centerOfScene = glm::vec3(0.0f, 0.0f, 0.0f);
    Shader depthShader;
    unsigned int depthMap;

public:

    LightSource() = default;    // never used

    LightSource(Shader &depthShader) {
        this->depthShader = depthShader;
        // We want to render the scene from the light source
        glGenFramebuffers(1, &depthMapFBO);
        // Generation of a texture that will be used as depth buffer
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                    SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
        // Attach depth's texture as the framebuffer's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);  // We're not going to render any color data
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void renderSceneToLight(std::vector<PhysicModel> &scene) {
        glm::mat4 lightSpaceMatrix = getLightSpaceMatrix();
        depthShader.use();
        depthShader.setMatrix4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        renderDepthMap(lightSpaceMatrix, scene);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void setPosition(glm::vec3 position) {
        lightPosition = position;
    }

    glm::vec3 getPosition() {
        return lightPosition;
    }

    unsigned int getDepthMapID() {
        return depthMap;
    }

    glm::mat4 getLightSpaceMatrix() {
        // The light sees the whole scene
        float near_plane = 2.0f, far_plane = 50.5f;  // TODO : did not understand these parameters
        glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
        // It points in te direction of the center of the scene
        glm::mat4 lightView = glm::lookAt(lightPosition, 
                                          centerOfScene, 
                                          glm::vec3( 0.0f, 1.0f,  0.0f));   // What is considered as the up direction
        // Transforms each world space vector into the space as visible from the light source
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        return lightSpaceMatrix;
    }

    const unsigned int getShadowWidth() {
        return SHADOW_WIDTH;
    }

    const unsigned int getShadowHeight() {
        return SHADOW_HEIGHT;
    }

    unsigned int getDepthMapFBO() {
        return depthMapFBO;
    }

private:

    void renderDepthMap(glm::mat4 &lightSpaceMatrix, std::vector<PhysicModel> &scene) {
        for (unsigned int i = 0; i < scene.size(); i++) {
            scene.at(i).Draw(depthShader);  // TODO : this does unnecessary operations about textures
        }
    }
};

#endif // LIGHT_SOURCE_H