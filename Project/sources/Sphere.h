#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"

#include <iostream>

Model generateSphere(std::string path, float radius, float mass, glm::vec3 position, btDiscreteDynamicsWorld* world) {
    Model sphere(path);
    btCollisionShape *sphere_shape = new btSphereShape(radius);
    sphere.createPhysicsObject(world, sphere_shape, mass, btVector3(position.x, position.y, position.z));
    return sphere;
}

Model generateSphere(std::string path, glm::vec3 position, btDiscreteDynamicsWorld* world) {
    return generateSphere(path, 0.33, 10, position, world);
}