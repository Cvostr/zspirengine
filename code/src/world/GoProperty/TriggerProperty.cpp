#include "../../../headers/world/go_properties.h"

Engine::TriggerProperty::TriggerProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_TRIGGER;
    coll_type = COLLIDER_TYPE::COLLIDER_TYPE_CUBE;
    created = false;
    m_ghost = nullptr;
}

void Engine::TriggerProperty::onUpdate(float deltaTime) {
    if (created) {
        //Get number of objects, that currently collide
        unsigned int overlaping = m_ghost->getNumOverlappingObjects();
        //Iterate over all overlapping objects
        for (int i = 0; i < (int)overlaping; i++) {
            btCollisionObject* coll_obj = m_ghost->getOverlappingObject(i);
            if (coll_obj == nullptr) continue;
            btRigidBody* pRigidBody = static_cast<btRigidBody*>(coll_obj);
            int obj_ind = pRigidBody->getUserIndex();
            GameObject* obj = this->go_link.world_ptr->getGameObjectByArrayIndex(obj_ind);
            if (obj == nullptr) continue;
            this->go_link.updLinkPtr()->onTrigger(obj);
        }
    }
}

void Engine::TriggerProperty::onStart() {
    initGhost();
}

void Engine::TriggerProperty::copyTo(Engine::GameObjectProperty* dest) {
    if (dest->type != PROPERTY_TYPE::GO_PROPERTY_TYPE_TRIGGER) return;

    //Do base things
    PhysicalProperty::copyTo(dest);

    TriggerProperty* trigger = static_cast<TriggerProperty*>(dest);
}

void Engine::TriggerProperty::onObjectDeleted() { //unregister in world
    if (created)
        this->go_link.world_ptr->physical_world->removeCollisionObjFromWorld(this->m_ghost);
}

void Engine::TriggerProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 0;
    offset += 1; //Skip space
    //read collider type
    memcpy(&coll_type, data + offset, sizeof(COLLIDER_TYPE));
    offset += sizeof(COLLIDER_TYPE);
    //read isCustomPhysicalSize boolean
    memcpy(&isCustomPhysicalSize, data + offset, sizeof(bool));
    offset += sizeof(bool);
    //Check, if trigger has custom size specified
    if (isCustomPhysicalSize) {
        memcpy(&cust_size.X, data + offset, sizeof(float));
        offset += sizeof(float);
        memcpy(&cust_size.Y, data + offset, sizeof(float));
        offset += sizeof(float);
        memcpy(&cust_size.Z, data + offset, sizeof(float));
        offset += sizeof(float);
        //Load Transform offset vector
        memcpy(&transform_offset.Z, data + offset, sizeof(float));
        offset += sizeof(float);
        memcpy(&transform_offset.Z, data + offset, sizeof(float));
        offset += sizeof(float);
        memcpy(&transform_offset.Z, data + offset, sizeof(float));
        offset += sizeof(float);
    }
}