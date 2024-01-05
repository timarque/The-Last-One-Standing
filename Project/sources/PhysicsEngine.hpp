#ifndef VR_PROJECT_PHYSIC_ENGINE
#define VR_PROJECT_PHYSIC_ENGINE

#include "bullet/btBulletDynamicsCommon.h"


struct PhysicsEngine {
    btBroadphaseInterface *broadphase;
    btDefaultCollisionConfiguration *collisionConfiguration;
    btCollisionDispatcher *dispatcher;
    btSequentialImpulseConstraintSolver *solver;
    btDiscreteDynamicsWorld *dynamicsWorld;

    PhysicsEngine(btVector3 gravity) {
        broadphase = new btDbvtBroadphase();                                                                   // Le big boss de Bullet
        collisionConfiguration = new btDefaultCollisionConfiguration();                              // Configuration de la physique
        dispatcher = new btCollisionDispatcher(collisionConfiguration);                                        // Que faire lorsqu'on a une collision
        solver = new btSequentialImpulseConstraintSolver();                                      // Solveurs pour résoudre les contraintes nécessaires à la physique blablabla vous irez lire la doc
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration); // On configure notre monde avec de la physique
        dynamicsWorld->setGravity(gravity);                                                                            // La gravité
    }

    btDiscreteDynamicsWorld* getWorld() { return dynamicsWorld; }

    void simulate(float deltaTime) { dynamicsWorld->stepSimulation(deltaTime, 10); }

    bool checkCollisions(const std::unique_ptr<btRigidBody>& bullet, const std::unique_ptr<btRigidBody>& tank) {
        btVector3 from = bullet->getWorldTransform().getOrigin();
        btVector3 to = from + bullet->getLinearVelocity().normalized() * 1.f;

        btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
        dynamicsWorld->rayTest(from, to, rayCallback);

        if (rayCallback.hasHit()) {
            const btCollisionObject* hitObject = rayCallback.m_collisionObject;

            // Check if the hit object is the tank
            if (hitObject->getUserIndex() == tank->getUserIndex()) {
                std::cout << "Hit the tank " << tank->getUserIndex() << std::endl;
                return true;
            }
        }

        return false;
    }
};


#endif