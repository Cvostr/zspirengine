#ifndef PHYSICS_H
#define PHYSICS_H

#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btBox2dShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btTriangleShape.h>
#include <LinearMath/btDefaultMotionState.h>

#include "../render/zs-math.h"

typedef struct PhysicalWorldSettings{
    ZSVECTOR3 gravity;

    PhysicalWorldSettings(){
        gravity = ZSVECTOR3(0, -10.0f, 0);
    }
}PhysicalWorldSettings;

class PhysicalWorld{
private:
    btDefaultCollisionConfiguration* collisionConfiguraton;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* broadphase;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* physic_world;

public:
    void stepSimulation(float stepSimulation);
    void addRidigbodyToWorld(btRigidBody* body);
    void removeRidigbodyFromWorld(btRigidBody* body);
    PhysicalWorld(PhysicalWorldSettings* settings);
};

#endif // PHYSICS_H
