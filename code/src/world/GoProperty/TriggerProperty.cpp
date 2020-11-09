#include "../../../headers/world/go_properties.h"

#define MAX_TRIGGER_OBJS_SIZE 11

Engine::TriggerProperty::TriggerProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_TRIGGER;
    coll_type = COLLIDER_TYPE::COLLIDER_TYPE_CUBE;
    created = false;
    m_ghost = nullptr;

    this->_array = new SmArray<int>(MAX_TRIGGER_OBJS_SIZE);
}

void Engine::TriggerProperty::onUpdate(float deltaTime) {
    GameObject* trigger_obj = this->go_link.updLinkPtr();
    if (created) {
        //Get number of objects, that currently collide
        unsigned int overlaping = m_ghost->getNumOverlappingObjects();
        int* objs_overlap = new int[overlaping];
        //Iterate over all overlapping objects
        for (int i = 0; i < (int)overlaping; i++) {
            btCollisionObject* coll_obj = m_ghost->getOverlappingObject(i);
            //if coll_obj is nullptr, then skip it
            if (coll_obj == nullptr) 
                continue;
            btRigidBody* pRigidBody = static_cast<btRigidBody*>(coll_obj);
            int obj_ind = pRigidBody->getUserIndex();
            objs_overlap[i] = obj_ind;
            GameObject* obj = this->go_link.world_ptr->getGameObjectByArrayIndex(obj_ind);
            //if object is nullptr, then skip it
            if (obj == nullptr) 
                continue;

            if (!_array->isInList(obj_ind)) {
                //Call onTriggerEnter() func on GameObject
                trigger_obj->onTriggerEnter(obj);
                _array->push_back(obj_ind);
            }

            //Call onTrigger() func on GameObject
            trigger_obj->onTrigger(obj);
        }
        //find objects, that were exited from collider
        for (unsigned int i = 0; i < _array->size(); i++) {
            int index = _array->at(i);
            GameObject* obj = this->go_link.world_ptr->getGameObjectByArrayIndex(index);
            bool found = false;
            //Try, to find object in new array
            for (int i = 0; i < (int)overlaping; i++) {
                if (objs_overlap[i] == index)
                    found = true;
            }
            //If object not found, then call onTriggerExit() on object
            if (!found) {
                //Call onTriggerExit() func on GameObject
                trigger_obj->onTriggerExit(obj);
                _array->_delete(index);
            }
        }
    }
}

void Engine::TriggerProperty::onStart() {
    initGhost();
}

void Engine::TriggerProperty::copyTo(Engine::IGameObjectComponent* dest) {
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

void Engine::TriggerProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
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