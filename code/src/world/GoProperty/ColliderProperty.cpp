#include "../../../headers/world/go_properties.h"

extern ZSGAME_DATA* game_data;

void Engine::ColliderProperty::onObjectDeleted() {
    if (created)
        this->go_link.world_ptr->physical_world->removeRidigbodyFromWorld(this->rigidBody);
} //unregister in world

void Engine::ColliderProperty::onUpdate(float deltaTime) {
    if (!created)
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
    memcpy(&coll_type, data + offset, sizeof(COLLIDER_TYPE));
    offset += sizeof(COLLIDER_TYPE);
    //read isCustomPhysicalSize boolean
    memcpy(&isCustomPhysicalSize, data + offset, sizeof(bool));
    offset += sizeof(bool);

    if (isCustomPhysicalSize) {
        //Read custom Size vector
        memcpy(&cust_size.X, data + offset, sizeof(float));
        offset += sizeof(float);
        memcpy(&cust_size.Y, data + offset, sizeof(float));
        offset += sizeof(float);
        memcpy(&cust_size.Z, data + offset, sizeof(float));
        offset += sizeof(float);
        //Read custom translation vector
        memcpy(&transform_offset.X, data + offset, sizeof(float));
        offset += sizeof(float);
        memcpy(&transform_offset.X, data + offset, sizeof(float));
        offset += sizeof(float);
        memcpy(&transform_offset.X, data + offset, sizeof(float));
        offset += sizeof(float);
    }
}