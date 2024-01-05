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
    btTransform transform = getTransform();
    btVector3 position = transform.getOrigin();
    btQuaternion rotation = transform.getRotation();
    for (Mesh mesh : meshes)
        mesh.setTransform(position.x(), position.y(), position.z(), rotation.w(), rotation.x(), rotation.y(), rotation.z());
}

glm::vec3 PhysicModel::getPosition() {
    btTransform worldTransform = getTransform();
    float y = static_cast<btBoxShape *>(physicsObject->getCollisionShape())->getHalfExtentsWithoutMargin().y();
    btVector3 position = worldTransform.getOrigin();
    glm::vec3 modelCoordinates(position.x(), position.y() - y, position.z());
    return modelCoordinates;
}

glm::mat4 PhysicModel::getRotation() {
    btTransform worldTransform = getTransform();
    btQuaternion rotation = worldTransform.getRotation();
    glm::mat4 modelRotation(glm::quat(rotation.w(), rotation.x(), rotation.y(), rotation.z()));
    return modelRotation;
}

glm::mat4 PhysicModel::getModelMatrix(glm::vec3 scale) {
    return glm::translate(glm::mat4(1.0f), getPosition()) * getRotation();
}

void PhysicModel::moveForward(float speed, glm::vec3 forward_dir)
{
    moveForward(speed, btVector3(forward_dir.x, forward_dir.y, forward_dir.z));
}

void PhysicModel::moveForward(float speed, btVector3 forward_dir)
{
    btVector3 currentVelocity = physicsObject->getLinearVelocity();
    btVector3 desiredVelocity = speed * forward_dir ;
    btVector3 velocityChange = desiredVelocity - currentVelocity;
    physicsObject->activate();
    physicsObject->applyCentralImpulse(velocityChange);
}


void PhysicModel::moveForward(float speed)
{
    btTransform worldTransform = getTransform();
    btVector3 forwardDir = worldTransform.getBasis().getColumn(2);
    moveForward(speed, forwardDir);
}

void PhysicModel::moveBackward(float speed) { moveForward(-speed); }

void PhysicModel::rotate(float angleDegrees)
{
    btTransform worldTransform = getTransform();
    btQuaternion rotation(btVector3(0, 1, 0), btRadians(angleDegrees));
    worldTransform.setRotation(rotation * worldTransform.getRotation());
    physicsObject->setWorldTransform(worldTransform);
}

void PhysicModel::updatePosition(glm::vec3 position)
{
    btTransform newTransform;
    newTransform.setOrigin(btVector3(position.x, position.y, position.z));
    btQuaternion currentRotation = physicsObject->getWorldTransform().getRotation();
    newTransform.setRotation(currentRotation);
    physicsObject->setWorldTransform(newTransform);
    physicsObject->setWorldTransform(newTransform);
};

glm::mat4 PhysicModel::getOpenGLMatrix() {
    glm::mat4 m;
    btTransform transform = getTransform();
    transform.getOpenGLMatrix(glm::value_ptr(m));
    return m;
}

void PhysicModel::applyImpulse(btVector3 impulse) { 
    physicsObject->applyCentralImpulse(impulse); 
}

btVector3 PhysicModel::getCenterOfMassPosition() { 
    return physicsObject->getCenterOfMassPosition(); 
}