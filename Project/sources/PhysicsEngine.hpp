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

};


#endif