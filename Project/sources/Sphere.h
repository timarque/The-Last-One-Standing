#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "PhysicModel.h"
#include "PhysicsEngine.hpp"

#include <iostream>

PhysicModel generatePhysicalSphere(std::string path, float radius, float mass, glm::vec3 position, PhysicsEngine physics) {
    PhysicModel sphere(path);
    btCollisionShape *sphere_shape = new btSphereShape(radius);
    sphere.createPhysicsObject(physics, sphere_shape, mass, btVector3(position.x, position.y, position.z));
    return sphere;
}

PhysicModel generatePhysicalSphere(std::string path, glm::vec3 position, PhysicsEngine physics) {
    return generatePhysicalSphere(path, 0.33, 10, position, physics);
}
