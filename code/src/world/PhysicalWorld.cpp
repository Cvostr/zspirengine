#include "../../headers/world/Physics.h"

#include "../../headers/world/ObjectsComponents/PhysicalComponent.hpp"
#include "../../headers/world/ObjectsComponents/TriggerComponent.hpp"
#include "../../headers/world/ObjectsComponents/MeshComponent.hpp"

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
    GhostCallback = new btGhostPairCallback();

    physic_world->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(GhostCallback);
}

PhysicalWorld::~PhysicalWorld() {
    //Delete all pointers
    delete physic_world;
    delete solver;
    delete broadphase;
    delete dispatcher;
    delete collisionConfiguraton;
    delete GhostCallback;
}

void PhysicalWorld::addRidigbodyToWorld(btRigidBody* body){
    this->physic_world->addRigidBody(body);
}

void PhysicalWorld::removeRidigbodyFromWorld(btRigidBody* body){
    this->physic_world->removeRigidBody(body);
}

void PhysicalWorld::addCollisionObjToWorld(btCollisionObject* body) {
    this->physic_world->addCollisionObject(body);
}
void PhysicalWorld::removeCollisionObjFromWorld(btCollisionObject* body) {
    this->physic_world->removeCollisionObject(body);
}

void PhysicalWorld::rayTest(Vec3 pos, Vec3 dir, btCollisionWorld::RayResultCallback& callback) {
    btVector3 bvpos = btVector3(pos.X, pos.Y, pos.Z);
    btVector3 bvdir = btVector3(dir.X, dir.Y, dir.Z);
    physic_world->rayTest(bvpos, bvdir, callback);
}

void* PhysicalWorld::RayTestFirstObject(Vec3 pos, Vec3 dir, float far) {
    dir.Normalize();
    btVector3 from = btVector3(pos.X, pos.Y, pos.Z);
    btVector3 to = btVector3(dir.X * far, dir.Y * far, dir.Z * far) + from;

    btCollisionWorld::ClosestRayResultCallback  allResults(from, to);
    allResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

    physic_world->rayTest(from, to, allResults);
    if (allResults.hasHit())
        return (allResults.m_collisionObject->getUserPointer());
    else
        return nullptr;
}

void PhysicalWorld::stepSimulation(float stepSimulation){
    //Simulate physics
    this->physic_world->stepSimulation(stepSimulation);
    physic_world->performDiscreteCollisionDetection();
}

unsigned int PhysicalWorld::getManifolsNum(){
    return static_cast<unsigned int>(dispatcher->getNumManifolds());
}
btPersistentManifold* PhysicalWorld::getManifold(unsigned int i){
    return dispatcher->getManifoldByIndexInternal(i);
}

Engine::PhysicalProperty::PhysicalProperty(){
    created = false;
    isCustomPhysicalSize = false;

    rigidBody = nullptr;
    shape = nullptr;
    //Set base values to custom transform vectors
    transform_offset = Vec3(0.f, 0.f, 0.f);
    cust_size = Vec3(1.f, 1.f, 1.f);

    coll_type = COLLIDER_TYPE::COLLIDER_TYPE_BOX;
    mass = 0;
}

void Engine::PhysicalProperty::copyTo(Engine::IGameObjectComponent* dest){

    //Do base things
    IGameObjectComponent::copyTo(dest);

    PhysicalProperty* rigi_prop = static_cast<PhysicalProperty*>(dest);
    rigi_prop->mass = this->mass;
    rigi_prop->coll_type = this->coll_type;
    rigi_prop->isCustomPhysicalSize = this->isCustomPhysicalSize;
    rigi_prop->cust_size = this->cust_size;
    rigi_prop->transform_offset = this->transform_offset;
}

bool Engine::PhysicalProperty::init(){
    Engine::TransformProperty* transform = go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
    //Set collision shape
    if (!updateCollisionShape())
        return false;

    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        shape->calculateLocalInertia(mass, localInertia);
    //Declare start transform
    btTransform startTransform = getBtTransform();

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
    //Create Object rigidbody instance
    rigidBody = new btRigidBody(cInfo);
    rigidBody->setUserIndex(this->go_link.updLinkPtr()->array_index);
    rigidBody->setUserPointer(go_link.updLinkPtr());
    //add rigidbody to world
    go_link.world_ptr->physical_world->addRidigbodyToWorld(rigidBody);
    //Set CREATED flag
    created = true;
    return true;
}

void Engine::TriggerProperty::initGhost() {
    Engine::TransformProperty* transform = go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();

    Vec3 pos = transform->abs_translation;
    //if uninitialized
    if (!updateCollisionShape())
        return;
    //Declare start transform
    btTransform startTransform;
    startTransform.setIdentity();
    //Set start transform
    startTransform.setOrigin(btVector3(btScalar(pos.X), btScalar(pos.Y), btScalar(pos.Z)));
    //Set start rotation
    startTransform.setRotation(btQuaternion(transform->abs_rotation.X, transform->abs_rotation.Y, transform->abs_rotation.Z));
    //Create Ghost Object
    m_ghost = new btGhostObject();
    m_ghost->setCollisionFlags(m_ghost->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    m_ghost->setCollisionShape(shape);
    m_ghost->setUserIndex(this->go_link.updLinkPtr()->array_index);
    m_ghost->setUserPointer(go_link.updLinkPtr());
    m_ghost->setWorldTransform(startTransform);
    go_link.world_ptr->physical_world->addCollisionObjToWorld(m_ghost);

    created = true;
}

btTransform Engine::PhysicalProperty::getBtTransform() {
    Engine::TransformProperty* transform = go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();

    Vec3 pos = transform->abs_translation;
    Vec3 rot = transform->abs_rotation;

    btTransform startTransform;
    startTransform.setIdentity();
    //Set start transform
    startTransform.setOrigin(btVector3(btScalar(pos.X + transform_offset.X),
                                       btScalar(pos.Y + transform_offset.Y),
                                       btScalar(pos.Z + transform_offset.Z)));
    //Set start rotation
    btQuaternion b;
    b.setEuler(transform->abs_rotation.Y, transform->abs_rotation.X, transform->abs_rotation.Z);
    startTransform.setRotation(b);

    return startTransform;
}

bool Engine::PhysicalProperty::updateCollisionShape(){
    Engine::TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
    transform->updateMatrix();
    Engine::MeshProperty* mesh = this->go_link.updLinkPtr()->getPropertyPtr<Engine::MeshProperty>();
    Engine::Mesh* m = mesh->mesh_ptr->mesh_ptr;

    Vec3 scale = transform->abs_scale;
    //if size is overrided
    if(isCustomPhysicalSize){
        scale = cust_size;
    }

    bool result = true;

    switch(coll_type){
        case COLLIDER_TYPE::COLLIDER_TYPE_NONE: {
            break;
        }
        case COLLIDER_TYPE::COLLIDER_TYPE_CUBE: {
            shape = new btBoxShape(btVector3(btScalar(scale.X),
                                         btScalar(scale.Y),
                                         btScalar(scale.Z)));
            break;
        }
        case COLLIDER_TYPE::COLLIDER_TYPE_BOX: {
            shape = new btBox2dShape(btVector3(btScalar(scale.X),
                                         btScalar(scale.Y),
                                         0));
            break;
        }
        case COLLIDER_TYPE::COLLIDER_TYPE_SPHERE:{
            shape = new btSphereShape(btScalar(scale.Y));
            break;
        }
        case COLLIDER_TYPE::COLLIDER_TYPE_CONVEX_HULL:{
            if (m->vertices_coord == nullptr) {
                result = false;
                break;
            }
            shape = new btConvexHullShape(m->vertices_coord, m->mVerticesNum, sizeof (float) * 3);
            break;
        }
        case COLLIDER_TYPE::COLLIDER_TYPE_MESH:{

            if (m->vertices_coord == nullptr) {
                result = false;
                break;
            }

            float* vertices = m->vertices_coord;
            int* indices = reinterpret_cast<int*>(m->indices_arr);

            btTriangleIndexVertexArray* va = new btTriangleIndexVertexArray(m->mIndicesNum / 3,
                                                                            indices,
                                                                            3 * sizeof (int),
                                                                            m->mVerticesNum,
                                                                            reinterpret_cast<btScalar*>(vertices),
                                                                            sizeof (Vec3));
            shape = new btBvhTriangleMeshShape(va, false);
            break;
        }
    }
    return result;
}

void Engine::PhysicalProperty::onUpdate(float deltaTime){
    if(!rigidBody->isActive())
        rigidBody->activate();
}

