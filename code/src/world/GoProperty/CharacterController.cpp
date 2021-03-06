#include <world/ObjectsComponents/CharacterController.hpp>

Engine::CharacterControllerProperty::CharacterControllerProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_CHARACTER_CONTROLLER;
    created = false;

    gravity = Vec3(0.f, -10.f, 0.f);
    linearVel = Vec3(0.f, -10.f, 0.f);

    mass = 10;

    width = 1.f;
    height = 1.f;
}

void Engine::CharacterControllerProperty::setLinearVelocity(Vec3 lvel) {
    if (!created) return;
    this->linearVel = lvel;
    this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));
    rigidBody->activate();
}

void Engine::CharacterControllerProperty::copyTo(Engine::IGameObjectComponent* dest) {
    if (dest->type != PROPERTY_TYPE::GO_PROPERTY_TYPE_CHARACTER_CONTROLLER) return;

    //Do base things
    PhysicalProperty::copyTo(dest);

    CharacterControllerProperty* chara = static_cast<CharacterControllerProperty*>(dest);
    chara->width = this->width;
    chara->height = this->height;

}

bool Engine::CharacterControllerProperty::isOnGround() {
    return _isOnGround;
}

void Engine::CharacterControllerProperty::jump(float height) {
    if (isOnGround()) {
        btVector3 d = rigidBody->getWorldTransform().getBasis()[1];
        btScalar magnitude = (btScalar(1.0) / rigidBody->getInvMass()) * height;
        rigidBody->activate(true);
        rigidBody->applyCentralImpulse(d * magnitude);
    }
}

void Engine::CharacterControllerProperty::walk(float x, float z) {
    btVector3 current_vel_l = rigidBody->getLinearVelocity();
    setLinearVelocity(Vec3(x, current_vel_l.y(), z));
}

void Engine::CharacterControllerProperty::onUpdate(float deltaTime) {

    Engine::TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
    btVector3 current_pos = rigidBody->getCenterOfMassPosition();

    //get current position
    float curX = current_pos.getX() - transform_offset.X;
    float curY = current_pos.getY() - transform_offset.Y;
    float curZ = current_pos.getZ() - transform_offset.Z;

    if (transform->translation != Vec3(curX, curY, curZ))
        transform->translation = Vec3(curX, curY, curZ);

    Vec3 pos = Vec3(current_pos.getX(), current_pos.getY(), current_pos.getZ());
    Vec3 target = pos + Vec3(0.f, (-height ) * 1.2f, 0.f);

    ClosestNotMe result(rigidBody);
    go_link.world_ptr->physical_world->rayTest(pos, target, result);
    if (result.hasHit())
    {
        _isOnGround = true;
    }
    else {
        _isOnGround = false;
    }

    btVector3 current_vel_l = rigidBody->getLinearVelocity();
    rigidBody->setLinearVelocity(btVector3(0, current_vel_l.y(), 0));
}

void Engine::CharacterControllerProperty::onStart() {
    Engine::TransformProperty* transform = go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();

    Vec3 scale = transform->abs_scale;
    Vec3 pos = transform->abs_translation;
    //if Custom Physical Size is checked
    if (isCustomPhysicalSize) {
        scale = cust_size;
    }

    //if uninitialized
    this->shape = new btCapsuleShape(width, height);

    btVector3 localInertia(0, 0, 0);
    //Declare start transform
    btTransform startTransform = getBtTransform();

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
    //Create Object rigidbody instance
    rigidBody = new btRigidBody(cInfo);

    rigidBody->setUserIndex(this->go_link.updLinkPtr()->array_index);
    //add rigidbody to world
    go_link.world_ptr->physical_world->addRidigbodyToWorld(rigidBody);
    //Set zero values
    this->rigidBody->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
    this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));

    created = true;
}

void Engine::CharacterControllerProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1; //Skip space
    //Read character sizes
    readBinaryValue(&width, data + offset, offset);
    readBinaryValue(&height, data + offset, offset);

    //Read translation offset vector
    readBinaryValue(&transform_offset.X, data + offset, offset);
    readBinaryValue(&transform_offset.Y, data + offset, offset);
    readBinaryValue(&transform_offset.Z, data + offset, offset);
}

void Engine::CharacterControllerProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    stream->write(reinterpret_cast<char*>(&width), sizeof(float));
    stream->write(reinterpret_cast<char*>(&height), sizeof(float));

    stream->write(reinterpret_cast<char*>(&transform_offset.X), sizeof(float));
    stream->write(reinterpret_cast<char*>(&transform_offset.Y), sizeof(float));
    stream->write(reinterpret_cast<char*>(&transform_offset.Z), sizeof(float));
}

void Engine::CharacterControllerProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    mgr->RegisterObjectType(CHAR_CTRL_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

    mgr->RegisterObjectProperty(CHAR_CTRL_PROP_TYPE_NAME, "float gravity", offsetof(CharacterControllerProperty, gravity));
    mgr->RegisterObjectProperty(CHAR_CTRL_PROP_TYPE_NAME, "float linearVelocity", offsetof(CharacterControllerProperty, linearVel));
    mgr->RegisterObjectMethod(CHAR_CTRL_PROP_TYPE_NAME, "void Jump(float)", asMETHOD(CharacterControllerProperty, jump), asCALL_THISCALL);
    mgr->RegisterObjectMethod(CHAR_CTRL_PROP_TYPE_NAME, "void Walk(float, float)", asMETHOD(CharacterControllerProperty, walk), asCALL_THISCALL);
    mgr->RegisterObjectMethod(CHAR_CTRL_PROP_TYPE_NAME, "bool isOnGround()", asMETHOD(CharacterControllerProperty, isOnGround), asCALL_THISCALL);
}