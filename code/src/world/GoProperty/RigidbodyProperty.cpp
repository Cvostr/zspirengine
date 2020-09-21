#include "../../../headers/world/go_properties.h"

Engine::RigidbodyProperty::RigidbodyProperty() {

    mass = 1.0f;
    created = false;
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_RIGIDBODY;
    coll_type = COLLIDER_TYPE::COLLIDER_TYPE_CUBE;

    gravity = ZSVECTOR3(0.f, -10.f, 0.f);
    linearVel = ZSVECTOR3(0.f, 0.f, 0.f);
}

void Engine::RigidbodyProperty::onStart() {
    if (init()) {
        //if initialization suucessful
        //then apply some settings
        this->rigidBody->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
        this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));
    }
}

void Engine::RigidbodyProperty::onUpdate(float deltaTime) {
    if (created) {

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

void Engine::RigidbodyProperty::setLinearVelocity(ZSVECTOR3& lvel) {
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

void Engine::RigidbodyProperty::applyCentralImpulse(ZSVECTOR3& imp) {
    if (!created) return;
    rigidBody->applyCentralImpulse(btVector3(imp.X, imp.Y, imp.Z));
}
void Engine::RigidbodyProperty::applyCentralForce(ZSVECTOR3& v) {
    if (!created) return;
    rigidBody->applyCentralForce(btVector3(v.X, v.Y, v.Z));
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
    readBinaryValue<COLLIDER_TYPE>(&coll_type, data + offset, offset);
    //Read mass of rigidbody
    readBinaryValue<float>(&mass, data + offset, offset);
    //Read Gravity vector
    readBinaryValue<float>(&gravity.X, data + offset, offset);
    readBinaryValue<float>(&gravity.Y, data + offset, offset);
    readBinaryValue<float>(&gravity.Z, data + offset, offset);
    //read linear velocity
    readBinaryValue<float>(&linearVel.X, data + offset, offset);
    readBinaryValue<float>(&linearVel.Y, data + offset, offset);
    readBinaryValue<float>(&linearVel.Z, data + offset, offset);
}

void Engine::RigidbodyProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    //write collider type
    stream->writeBinaryValue(&coll_type);
    //write isTrigger boolean
    stream->writeBinaryValue(&mass);
    //write gravity
    stream->writeBinaryValue(&gravity.X);
    stream->writeBinaryValue(&gravity.Y);
    stream->writeBinaryValue(&gravity.Z);
    //write linear velocity
    stream->writeBinaryValue(&linearVel.X);
    stream->writeBinaryValue(&linearVel.Y);
    stream->writeBinaryValue(&linearVel.Z);
}

void Engine::RigidbodyProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    mgr->RegisterObjectType(RIGIDBODY_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

    mgr->RegisterObjectProperty(RIGIDBODY_PROP_TYPE_NAME, "float mass", offsetof(RigidbodyProperty, mass));
    mgr->RegisterObjectMethod(RIGIDBODY_PROP_TYPE_NAME, "void setLinearVelocity(Vec3)", asMETHOD(RigidbodyProperty, setLinearVelocity), asCALL_THISCALL);
    mgr->RegisterObjectMethod(RIGIDBODY_PROP_TYPE_NAME, "void applyCentralImpulse(Vec3)", asMETHOD(RigidbodyProperty, applyCentralImpulse), asCALL_THISCALL);
    mgr->RegisterObjectMethod(RIGIDBODY_PROP_TYPE_NAME, "void applyCentralForce(Vec3)", asMETHOD(RigidbodyProperty, applyCentralForce), asCALL_THISCALL);
}