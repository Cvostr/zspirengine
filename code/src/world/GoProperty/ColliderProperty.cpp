#include "../../../headers/world/go_properties.h"

extern ZSGAME_DATA* game_data;

void Engine::ColliderProperty::onObjectDeleted() {
    if (created)
        this->go_link.world_ptr->physical_world->removeRidigbodyFromWorld(this->rigidBody);
} //unregister in world

void Engine::ColliderProperty::onUpdate(float deltaTime) {

}

void Engine::ColliderProperty::onStart() {
    init();
}

void Engine::ColliderProperty::copyTo(Engine::IGameObjectComponent* dest) {
    if (dest->type != PROPERTY_TYPE::GO_PROPERTY_TYPE_COLLIDER) return;

    PhysicalProperty::copyTo(dest);

    ColliderProperty* coll_prop = static_cast<ColliderProperty*>(dest);
}

Engine::TransformProperty* Engine::ColliderProperty::getTransformProperty() {
    return this->go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
}

Engine::ColliderProperty::ColliderProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_COLLIDER;

    coll_type = COLLIDER_TYPE::COLLIDER_TYPE_CUBE;
    created = false;
    mass = 0.0f; //collider is static
}

void Engine::ColliderProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    //read collider type
    readBinaryValue<COLLIDER_TYPE>(&coll_type, data + offset, offset);
    //read isCustomPhysicalSize boolean
    readBinaryValue<bool>(&isCustomPhysicalSize, data + offset, offset);

    if (isCustomPhysicalSize) {
        //Read custom Size vector
        readBinaryValue<float>(&cust_size.X, data + offset, offset);
        readBinaryValue<float>(&cust_size.Y, data + offset, offset);
        readBinaryValue<float>(&cust_size.Z, data + offset, offset);
        //Read custom translation vector
        readBinaryValue<float>(&transform_offset.X, data + offset, offset);
        readBinaryValue<float>(&transform_offset.Y, data + offset, offset);
        readBinaryValue<float>(&transform_offset.Z, data + offset, offset);
    }
}

void Engine::ColliderProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    //write collider type
    stream->writeBinaryValue(&coll_type);
    //boolean if collider has custom size and transform
    stream->writeBinaryValue(&isCustomPhysicalSize);
    if (isCustomPhysicalSize) {
        stream->writeBinaryValue(&cust_size.X);
        stream->writeBinaryValue(&cust_size.Y);
        stream->writeBinaryValue(&cust_size.Z);

        stream->writeBinaryValue(&transform_offset.X);
        stream->writeBinaryValue(&transform_offset.Y);
        stream->writeBinaryValue(&transform_offset.Z);
    }
}

void Engine::ColliderProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    mgr->RegisterObjectType(COLLIDER_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

    mgr->RegisterObjectProperty(COLLIDER_PROP_TYPE_NAME, "float mass", offsetof(ColliderProperty, mass));
}