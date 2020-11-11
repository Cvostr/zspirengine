#ifndef PHYSICS_H
#define PHYSICS_H

#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
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

#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

#include "../render/Math.hpp"

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
    btGhostPairCallback* GhostCallback;

public:
    unsigned int getManifolsNum();
    btPersistentManifold* getManifold(unsigned int i);

    void stepSimulation(float stepSimulation);
    void addRidigbodyToWorld(btRigidBody* body);
    void removeRidigbodyFromWorld(btRigidBody* body);
    void addCollisionObjToWorld(btCollisionObject* body);
    void removeCollisionObjFromWorld(btCollisionObject* body);
    void rayTest(ZSVECTOR3 pos, ZSVECTOR3 dir, btCollisionWorld::RayResultCallback& callback);

    PhysicalWorld(PhysicalWorldSettings* settings);
    ~PhysicalWorld();
};

class ClosestNotMe : public btCollisionWorld::ClosestRayResultCallback
{
public:
    ClosestNotMe(btRigidBody* me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
    {
        m_me = me;
    }

    virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
    {
        if (rayResult.m_collisionObject == m_me)
            return 1.0;

        return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace
        );
    }
protected:
    btRigidBody* m_me;
};

#endif // PHYSICS_H
