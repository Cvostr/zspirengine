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
            if (coll_obj == nullptr) 
                continue;
            btRigidBody* pRigidBody = static_cast<btRigidBody*>(coll_obj);
            int obj_ind = pRigidBody->getUserIndex();
            GameObject* obj = this->go_link.world_ptr->getGameObjectByArrayIndex(obj_ind);
            if (obj == nullptr) 
                continue;
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
    unsigned int offset = 1;
    //read collider type
    readBinaryValue<COLLIDER_TYPE>(&coll_type, data + offset, offset);
    //read isCustomPhysicalSize boolean
    readBinaryValue<bool>(&isCustomPhysicalSize, data + offset, offset);
    //Check, if trigger has custom size specified
    if (isCustomPhysicalSize) {
        readBinaryValue(&cust_size.X, data + offset, offset);
        readBinaryValue(&cust_size.Y, data + offset, offset);
        readBinaryValue(&cust_size.Z, data + offset, offset);
        //Load Transform offset vector
        readBinaryValue(&transform_offset.X, data + offset, offset);
        readBinaryValue(&transform_offset.Y, data + offset, offset);
        readBinaryValue(&transform_offset.Z, data + offset, offset);
    }
}

void Engine::TriggerProperty::savePropertyToStream(std::ofstream* stream, GameObject* obj) {
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