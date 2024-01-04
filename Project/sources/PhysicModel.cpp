#include "PhysicModel.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void PhysicModel::createPhysicsObject(PhysicsEngine physics, btCollisionShape *collision_shape, float mass, btVector3 origin)
{

    btVector3 localInertia(0, 0, 0);
    collision_shape->calculateLocalInertia(mass, localInertia);
    // Ajuster l'inertie selon vos besoins

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(origin);

    btDefaultMotionState *motionState = new btDefaultMotionState(transform);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collision_shape);
    physicsObject = std::make_unique<btRigidBody>(*new btRigidBody(rbInfo));
    physicsObject->setRestitution(0.8);
    physicsObject->setFriction(0.5f);
    physics.getWorld()->addRigidBody(physicsObject.get());
    physicsObject->activate();
}

// Mettre à jour la transformation du modèle à partir de la physique
void PhysicModel::updateFromPhysics()
{
    btTransform transform;
    physicsObject->getMotionState()->getWorldTransform(transform);
    btVector3 position = transform.getOrigin();
    btQuaternion rotation = transform.getRotation();
    for (Mesh mesh : meshes)
        mesh.setTransform(position.x(), position.y(), position.z(), rotation.w(), rotation.x(), rotation.y(), rotation.z());
}

glm::vec3 PhysicModel::getPosition() {
    btTransform worldTransform;
    physicsObject->getMotionState()->getWorldTransform(worldTransform);
    btVector3 position = worldTransform.getOrigin();
    glm::vec3 modelCoordinates(position.x(), position.y(), position.z());
    return modelCoordinates;
}

void PhysicModel::moveForward(float speed, glm::vec3 forward_dir)
{
    moveForward(speed, btVector3(forward_dir.x, forward_dir.y, forward_dir.z));
}

//void PhysicModel::moveForward(float speed, btVector3 forward_dir)
//{
//    btTransform worldTransform;
//    physicsObject->getMotionState()->getWorldTransform(worldTransform);
//    worldTransform.setOrigin(worldTransform.getOrigin() + speed * forward_dir);
//    physicsObject->getMotionState()->setWorldTransform(worldTransform);
//}

void PhysicModel::moveForward(float speed, btVector3 forward_dir)
{
    // Get the rigid body's current velocity
    btVector3 currentVelocity = physicsObject->getLinearVelocity();

    // Calculate the desired velocity based on the speed and forward direction
    btVector3 desiredVelocity = speed * forward_dir + btVector3(1.0, 1.0, 1.0);

    // Calculate the change in velocity needed
    btVector3 velocityChange = desiredVelocity - currentVelocity;
    std::cout << "velocity " << velocityChange.x() << " " << velocityChange.y() << " " << velocityChange.z() << " " << std::endl;

    // Apply central impulse to the object to achieve the desired velocity
    physicsObject->applyCentralImpulse(velocityChange);
}


void PhysicModel::moveForward(float speed)
{
    btTransform worldTransform;
    physicsObject->getMotionState()->getWorldTransform(worldTransform);
    btVector3 forwardDir = worldTransform.getBasis().getColumn(2);
    moveForward(speed, forwardDir);
}

void PhysicModel::moveBackward(float speed) { moveForward(-speed); }

void PhysicModel::rotate(float angleDegrees)
{
    btTransform worldTransform;
    physicsObject->getMotionState()->getWorldTransform(worldTransform);
    btQuaternion rotation(btVector3(0, 1, 0), btRadians(angleDegrees));
    worldTransform.setRotation(rotation * worldTransform.getRotation());
    physicsObject->getMotionState()->setWorldTransform(worldTransform);
}

void PhysicModel::updatePosition(glm::vec3 position)
{
    btTransform newTransform;
    newTransform.setOrigin(btVector3(position.x, position.y, position.z));
    btQuaternion currentRotation = physicsObject->getWorldTransform().getRotation();
    newTransform.setRotation(currentRotation);
    physicsObject->setWorldTransform(newTransform);
    physicsObject->getMotionState()->setWorldTransform(newTransform);
};

glm::mat4 PhysicModel::getOpenGLMatrix() {
    glm::mat4 m;
    btTransform transform;
    physicsObject->getMotionState()->getWorldTransform(transform);
    transform.getOpenGLMatrix(glm::value_ptr(m));
    return m;
}

void PhysicModel::applyImpulse(btVector3 impulse) { 
    physicsObject->applyCentralImpulse(impulse); 
}

btVector3 PhysicModel::getCenterOfMassPosition() { 
    return physicsObject->getCenterOfMassPosition(); 
}