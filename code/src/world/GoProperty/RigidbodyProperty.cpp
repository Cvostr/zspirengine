#include "../../../headers/world/go_properties.h"

Engine::RigidbodyProperty::RigidbodyProperty() {

    mass = 1.0f;
    created = false;
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_RIGIDBODY;
    coll_type = COLLIDER_TYPE::COLLIDER_TYPE_CUBE;

    gravity = ZSVECTOR3(0.f, -10.f, 0.f);
    linearVel = ZSVECTOR3(0.f, 0.f, 0.f);
}

void Engine::RigidbodyProperty::onUpdate(float deltaTime) {
    if (!created) {
        //if uninitialized
        if (init()) {
            //if initialization suucessful
            //then apply some settings
            this->rigidBody->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
            this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));
        }
    }
    else {
        PhysicalProperty::onUpdate(deltaTime);

        Engine::TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
        btVector3 current_pos = rigidBody->getCenterOfMassPosition();
        btQuaternion current_rot = rigidBody->getWorldTransform().getRotation();
        //get current position
        float curX = current_pos.getX();
        float curY = current_pos.getY();
        float curZ = current_pos.getZ();

        float rotX = 0, rotY = 0, rotZ = 0;
        //Convert quaternion to euler
        current_rot.getEulerZYX(rotX, rotY, rotZ);
        //Convert radians to degrees
        rotX = rotX / ZS_PI * 180.0f;
        rotY = rotY / ZS_PI * 180.0f;
        rotZ = rotZ / ZS_PI * 180.0f;

        if (transform->translation != ZSVECTOR3(curX, curY, curZ))
            transform->translation = ZSVECTOR3(curX, curY, curZ);
        if (transform->rotation != ZSVECTOR3(rotX, rotY, -rotZ))
            transform->rotation = ZSVECTOR3(rotX, rotY, -rotZ);
    }
}

void Engine::RigidbodyProperty::setLinearVelocity(ZSVECTOR3 lvel) {
    if (!created) return;
    this->linearVel = lvel;
    this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));
}

void Engine::RigidbodyProperty::onValueChanged() {
    if (!created) return;
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        shape->calculateLocalInertia(mass, localInertia);
    //SET mass to bullet rigidbody
    this->rigidBody->setMassProps(mass, localInertia);
    this->rigidBody->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
    this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));
    //Delete Bullet Shape Object
    delete shape;
    //Update Shape
    updateCollisionShape();
    //Assign new shape to rigid body
    this->rigidBody->setCollisionShape(shape);
}


void Engine::RigidbodyProperty::copyTo(Engine::GameObjectProperty* dest) {
    if (dest->type != PROPERTY_TYPE::GO_PROPERTY_TYPE_RIGIDBODY) return;

    //Do base things
    PhysicalProperty::copyTo(dest);

    RigidbodyProperty* rigi_prop = static_cast<RigidbodyProperty*>(dest);
    rigi_prop->gravity = this->gravity;
    rigi_prop->linearVel = this->linearVel;
    rigi_prop->angularVel = this->angularVel;
}

void Engine::RigidbodyProperty::onObjectDeleted() { //unregister in world
    if (created)
        this->go_link.world_ptr->physical_world->removeRidigbodyFromWorld(this->rigidBody);
}

void Engine::RigidbodyProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    //read collider type
    memcpy(&coll_type, data + offset, sizeof(COLLIDER_TYPE));
    offset += sizeof(COLLIDER_TYPE);
    //Read mass of rigidbody
    memcpy(&mass, data + offset, sizeof(float));
    offset += sizeof(float);
    //Read Gravity vector
    memcpy(&gravity.X, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&gravity.Y, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&gravity.Z, data + offset, sizeof(float));
    offset += sizeof(float);
    //read linear velocity
    memcpy(&linearVel.Z, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&linearVel.Z, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&linearVel.Z, data + offset, sizeof(float));
    offset += sizeof(float);
}