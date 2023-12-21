//
// Created by hanno on 21/12/2023.
//

#include "Shader.h"
#include "Camera.h"
#include "Object.h"

#ifndef VR_PROJECT_MOTORBIKE_H
#define VR_PROJECT_MOTORBIKE_H


class Motorbike : public Object {
private:
    Texture moto;
    GLint u_texture;

public:
    Motorbike(const char *modelPath, Shader *shader, bool texture = true) : Object(modelPath, shader, texture) {
        auto u_tex = glGetUniformLocation(shader->ID, "ourTexture"); 
        this->u_texture = u_tex;
    }

    void createTexture() {
        const char *file = "../../../../Project/textures/tronMoto.jpg";
        Texture moto_tex(file);
        this->moto = moto_tex;
    }

    void draw(Camera *camera) override {

        /*
        glm::vec3 light_pos = glm::vec3(1.0, 2.0, 1.5);
        float ambient = 0.1;
        float diffuse = 0.5;
        float specular = 0.8;
        glm::vec3 materialColour = glm::vec3(0.5f,0.6,0.8);
        */

        /*
        shader.use();
        shader.setFloat("shininess", 32.0f);
        shader.setVector3f("materialColour", materialColour);
        shader.setFloat("light.ambient_strength", ambient);
        shader.setFloat("light.diffuse_strength", diffuse);
        shader.setFloat("light.specular_strength", specular);
        shader.setFloat("light.constant", 1.0);
        shader.setFloat("light.linear", 0.14);
        shader.setFloat("light.quadratic", 0.07);
        */

        /*
        shader.use();

        shader.setMatrix4("M", moto1.model);
        shader.setMatrix4("itM", inverseModel);
        shader.setMatrix4("V", view);
        shader.setMatrix4("P", perspective);
        shader.setVector3f("u_view_pos", camera.Position);
        shader.setVector3f("light.light_pos", delta);

        reflectiveShader.use();

        reflectiveShader.setMatrix4("M", moto1.model);
        reflectiveShader.setMatrix4("itM", inverseModel);
        reflectiveShader.setMatrix4("V", view);
        reflectiveShader.setMatrix4("P", perspective);
        reflectiveShader.setVector3f("u_view_pos", camera.Position);
        reflectiveShader.setVector3f("light.light_pos", delta);


        refractiveShader.use();

        refractiveShader.setMatrix4("M", moto1.model);
        refractiveShader.setMatrix4("itM", inverseModel);
        refractiveShader.setMatrix4("V", view);
        refractiveShader.setMatrix4("P", perspective);
        refractiveShader.setVector3f("u_view_pos", camera.Position);
        */

        //auto delta = light_pos + glm::vec3(0.0, 0.0, 2 * std::sin(now));

        shader->use();
        shader->setMatrix4("M", model);
        glm::mat4 inverseModel = glm::transpose( glm::inverse(model));
        shader->setMatrix4("itM", inverseModel);
        shader->setMatrix4("V", camera->GetViewMatrix());
        shader->setMatrix4("P", camera->GetProjectionMatrix());
        shader->setVector3f("u_view_pos", camera->Position);


        glUniform1i(this->u_texture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->moto.getId());

        Object::draw(camera);
    }

};



#endif //VR_PROJECT_MOTORBIKE_H
