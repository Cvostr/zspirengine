#include "../../headers/world/Physics.h"
#include "../../headers/world/go_properties.h"

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
    physic_world->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
}

PhysicalWorld::~PhysicalWorld() {
    //Delete all pointers
    delete physic_world;
    delete solver;
    delete broadphase;
    delete dispatcher;
    delete collisionConfiguraton;
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

void PhysicalWorld::stepSimulation(float stepSimulation){
    if(stepSimulation > 200) return;

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

    transform_offset = ZSVECTOR3(0, 0, 0);

    coll_type = COLLIDER_TYPE::COLLIDER_TYPE_BOX;
    mass = 0;

}

void Engine::PhysicalProperty::copyTo(Engine::GameObjectProperty* dest){

    //Do base things
    GameObjectProperty::copyTo(dest);

    PhysicalProperty* rigi_prop = static_cast<PhysicalProperty*>(dest);
    rigi_prop->mass = this->mass;
    rigi_prop->coll_type = this->coll_type;
    rigi_prop->isCustomPhysicalSize = this->isCustomPhysicalSize;
    rigi_prop->cust_size = this->cust_size;
    rigi_prop->transform_offset = this->transform_offset;
}

void Engine::PhysicalProperty::init(){
    Engine::TransformProperty* transform = go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
    //Set collision shape
    updateCollisionShape();

    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        shape->calculateLocalInertia(mass, localInertia);
    //Declare start transform
    btTransform startTransform;
    startTransform.setIdentity();
    //Set start transform
    startTransform.setOrigin(btVector3( btScalar(transform->abs_translation.X + transform_offset.X),
                                                btScalar(transform->abs_translation.Y + transform_offset.Y),
                                                btScalar(transform->abs_translation.Z + transform_offset.Z)));

    btQuaternion b;
    b.setEuler(transform->abs_rotation.Z, transform->abs_rotation.Y, transform->abs_rotation.X);
    //Set start rotation
    startTransform.setRotation(btQuaternion(transform->abs_rotation.X, transform->abs_rotation.Y, transform->abs_rotation.Z));
    //startTransform.setRotation(b);

     

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
    //Create Object rigidbody instance
    rigidBody = new btRigidBody(cInfo);
    rigidBody->setUserIndex(this->go_link.updLinkPtr()->array_index);
    //add rigidbody to world
    go_link.world_ptr->physical_world->addRidigbodyToWorld(rigidBody);
    
    created = true;
}

void Engine::TriggerProperty::initGhost() {
    Engine::TransformProperty* transform = go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();

    ZSVECTOR3 scale = transform->abs_scale;
    ZSVECTOR3 pos = transform->abs_translation;
    //if Custom Physical Size is checked
    if (isCustomPhysicalSize) {
        scale = cust_size;
    }

    //if uninitialized
    this->shape = new btCapsuleShape(scale.X, scale.Y);
    //Declare start transform
    btTransform startTransform;
    startTransform.setIdentity();
    //Set start transform
    startTransform.setOrigin(btVector3(btScalar(pos.X), btScalar(pos.Y), btScalar(pos.Z)));
    //Set start rotation
    startTransform.setRotation(btQuaternion(transform->abs_rotation.X, transform->abs_rotation.Y, transform->abs_rotation.Z));

    m_ghost = new btGhostObject();
    m_ghost->setCollisionShape(shape);
    m_ghost->setWorldTransform(startTransform);
    go_link.world_ptr->physical_world->addCollisionObjToWorld(m_ghost);

    created = true;
}

void Engine::PhysicalProperty::updateCollisionShape(){
    Engine::TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
    Engine::MeshProperty* mesh = this->go_link.updLinkPtr()->getPropertyPtr<Engine::MeshProperty>();
    Engine::Mesh* m = mesh->mesh_ptr->mesh_ptr;

    ZSVECTOR3 scale = transform->abs_scale;
    if(isCustomPhysicalSize){
        scale = cust_size;
    }

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

            shape = new btConvexHullShape(m->vertices_coord, m->vertices_num, sizeof (float) * 3);
            break;
        }
        case COLLIDER_TYPE::COLLIDER_TYPE_MESH:{

            float* vertices = m->vertices_coord;
            int* indices = reinterpret_cast<int*>(m->indices_arr);

            btTriangleIndexVertexArray* va = new btTriangleIndexVertexArray(m->indices_num / 3,
                                                                            indices,
                                                                            3 * sizeof (int),
                                                                            m->vertices_num,
                                                                            reinterpret_cast<btScalar*>(vertices),
                                                                            sizeof (ZSVECTOR3));
            shape = new btBvhTriangleMeshShape(va, false);
            break;
        }
    }
}

void Engine::PhysicalProperty::onUpdate(float deltaTime){
    if(!rigidBody->isActive())
        rigidBody->activate();
}
