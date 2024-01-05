#ifndef VR_PROJECT_PHYSIC_MODEL_H
#define VR_PROJECT_PHYSIC_MODEL_H

#include <iostream>
#include "Model.h"
#include "PhysicsEngine.hpp"
#include "bullet/btBulletDynamicsCommon.h"
#include <memory>

class PhysicModel : public Model {
protected:
    std::unique_ptr<btCollisionShape> mShape;
    std::unique_ptr<btDefaultMotionState> mMotionState;
public:

    std::unique_ptr<btRigidBody> physicsObject;
    std::unique_ptr<btCollisionObject> physicsCol;
    PhysicModel(std::string path) : Model(path) {};
    // Créer un objet physique associé au modèle dans le monde physique
    void createPhysicsObject(PhysicsEngine physics, btCollisionShape *shape, float mass, btVector3 origin);
    // Mettre à jour la transformation du modèle à partir de la physique
    void updateFromPhysics();

    glm::vec3 getPosition();

    btTransform getTransform() { return physicsObject->getWorldTransform(); }

    btVector3 getForward() { return physicsObject->getWorldTransform().getBasis().getColumn(2).normalized();}

    glm::mat4 getRotation();
    glm::mat4 getModelMatrix(glm::vec3 scale);

    void moveForward(float speed, glm::vec3 forward_dir);
    void moveForward(float speed, btVector3 forward_dir);
    void moveForward(float speed);
       
    void moveBackward(float speed);

    void rotate(float angleDegrees);

    void updatePosition(glm::vec3 position);

    glm::mat4 getOpenGLMatrix();
    void applyImpulse(btVector3 impulse);
    btVector3 getCenterOfMassPosition();
};
#endif
