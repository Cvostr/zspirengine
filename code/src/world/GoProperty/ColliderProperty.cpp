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

void Engine::ColliderProperty::copyTo(Engine::GameObjectProperty* dest) {
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

void Engine::ColliderProperty::savePropertyToStream(std::ofstream* stream, GameObject* obj) {
    //write collider type
    stream->write(reinterpret_cast<char*>(&coll_type), sizeof(COLLIDER_TYPE));
    //boolean if collider has custom size and transform
    stream->write(reinterpret_cast<char*>(&isCustomPhysicalSize), sizeof(bool));
    if (isCustomPhysicalSize) {
        stream->write(reinterpret_cast<char*>(&cust_size.X), sizeof(float));
        stream->write(reinterpret_cast<char*>(&cust_size.Y), sizeof(float));
        stream->write(reinterpret_cast<char*>(&cust_size.Z), sizeof(float));
        stream->write(reinterpret_cast<char*>(&transform_offset.X), sizeof(float));
        stream->write(reinterpret_cast<char*>(&transform_offset.Y), sizeof(float));
        stream->write(reinterpret_cast<char*>(&transform_offset.Z), sizeof(float));
    }
}