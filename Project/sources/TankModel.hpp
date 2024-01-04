#ifndef VR_PROJECT_TANK_MODEL_H
#define VR_PROJECT_TANK_MODEL_H

#include <iostream>
#include "PhysicModel.h"
#include "PhysicsEngine.hpp"
#include "bullet/btBulletDynamicsCommon.h"
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class TankModel : public PhysicModel {
public:
    // float mSteering;
    
    // std::unique_ptr<btDefaultVehicleRaycaster> mVehicleRaycaster;
    // std::unique_ptr<btRaycastVehicle> mVehicle;

    TankModel(std::string path) : PhysicModel(path){};
    // Créer un objet physique associé au modèle dans le monde physique
    // void createPhysicsObject(PhysicsEngine physics, btCollisionShape *shape, float mass, btVector3 origin);
    // glm::vec3 getPosition();
    void update(GLFWwindow *window, float deltaTime);
    // Mettre à jour la transformation du modèle à partir de la physique
    // void applyEngineForce(float force);
    // void setBrake(float force);
    // void setSteering(float steering);

    // glm::mat4 getOpenGLMatrix();
};
#endif
