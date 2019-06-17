#include "../../headers/world/World.h"


Engine::GameObjectLink::GameObjectLink(){
    ptr = nullptr;
    world_ptr = nullptr;
}


Engine::GameObject* Engine::GameObjectLink::updLinkPtr(){
    if(world_ptr == nullptr) //If world not defined, exiting
        return nullptr;
    if(this->ptr != nullptr) //if pointer already calculated
        if(this->obj_str_id.compare(ptr->str_id) == 0)
            return ptr;

    this->ptr = world_ptr->getGameObjectByStrId(this->obj_str_id);
    return ptr;
}

bool Engine::GameObjectLink::isEmpty(){
    if (this->world_ptr == nullptr) return true;
    return false;
}

void Engine::GameObjectLink::crack(){
    this->world_ptr = nullptr; //It will now pass isEmpty() check
}


Engine::GameObject::GameObject(){
    props_num = 0;
    alive = true;
    hasParent = false;

    world_ptr = nullptr;

    for(int prop_i = 0; prop_i < OBJ_PROPS_SIZE; prop_i ++){ //iterate over all property pointers and clear them
        properties[prop_i] = nullptr;
    }
}

Engine::GameObjectLink Engine::GameObject::getLinkToThisObject(){
    GameObjectLink link; //Definition of result link
    link.obj_str_id = this->str_id; //Placing string id
    link.world_ptr = this->world_ptr; //Placing world pointer

    link.ptr = world_ptr->getGameObjectByStrId(link.obj_str_id);
    return link;
}

bool Engine::GameObject::addProperty(int property){
    unsigned int props = static_cast<unsigned int>(this->props_num);
    for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
        GameObjectProperty* property_ptr = this->properties[prop_i];
        if(property_ptr->type == property){ //If object already has one
            return false; //Exit function
        }
    }
    GameObjectProperty* _ptr = allocProperty(property);

    _ptr->go_link = this->getLinkToThisObject();
    _ptr->go_link.updLinkPtr();
    _ptr->world_ptr = this->world_ptr; //Assign pointer to world
    this->properties[props_num] = _ptr; //Store property in gameobject
    this->props_num += 1;
    return true;
}

Engine::TransformProperty* Engine::GameObject::getTransformProperty(){
    return static_cast<TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
}

Engine::GameObjectProperty* Engine::GameObject::getPropertyPtrByType(PROPERTY_TYPE type){
    unsigned int props = static_cast<unsigned int>(this->props_num);
    for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
        GameObjectProperty* property_ptr = this->properties[prop_i];
        if(property_ptr->type == type){ //If object already has one
            return property_ptr; //return it
        }
    }
    return nullptr;
}

void Engine::GameObject::onUpdate(int deltaTime){   //calls onUpdate on all properties
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        properties[i]->onUpdate(deltaTime); //and call onUpdate on each property
    }
}

void Engine::GameObject::onPreRender(Engine::RenderPipeline* pipeline){ //calls onPreRender on all properties
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        properties[i]->onPreRender(pipeline); //and call onUpdate on each property
    }
}
