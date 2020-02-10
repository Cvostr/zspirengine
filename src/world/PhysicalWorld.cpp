#include "../../headers/world/Physics.h"

PhysicalWorld::PhysicalWorld(PhysicalWorldSettings* settings){
    this->collisionConfiguraton = new btDefaultCollisionConfiguration();
    collisionConfiguraton->setConvexConvexMultipointIterations();

    //use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    this->dispatcher = new btCollisionDispatcher(this->collisionConfiguraton);

    this->broadphase = new btDbvtBroadphase();

    //the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    this->solver = new btSequentialImpulseConstraintSolver;

    this->physic_world = new btDiscreteDynamicsWorld(this->dispatcher, this->broadphase, this->solver, this->collisionConfiguraton);

    this->physic_world->setGravity(btVector3(settings->gravity.X,
                                                     settings->gravity.Y,
                                                     settings->gravity.Z));
}

void PhysicalWorld::addRidigbodyToWorld(btRigidBody* body){
    this->physic_world->addRigidBody(body);
}

void PhysicalWorld::removeRidigbodyFromWorld(btRigidBody* body){
    this->physic_world->removeRigidBody(body);
}

void PhysicalWorld::stepSimulation(float stepSimulation){
    this->physic_world->stepSimulation(stepSimulation);
    physic_world->performDiscreteCollisionDetection();
}

unsigned int PhysicalWorld::getManifolsNum(){
    return static_cast<unsigned int>(dispatcher->getNumManifolds());
}
btPersistentManifold* PhysicalWorld::getManifold(unsigned int i){
    return dispatcher->getManifoldByIndexInternal(i);
}
