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

std::string Engine::GameObject::getLabel(){
    return *this->label_ptr;
}

void Engine::GameObject::setLabel(std::string label){
    this->getLabelProperty()->label = (label);
}

void Engine::GameObject::setActive(bool active){
    this->active = active;
}

Engine::TransformProperty* Engine::GameObject::getTransformProperty(){
    return static_cast<TransformProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
}

Engine::LabelProperty* Engine::GameObject::getLabelProperty(){
    return static_cast<LabelProperty*>(getPropertyPtrByType(GO_PROPERTY_TYPE_LABEL));
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
        if(!properties[i]->active) continue; //if property is inactive, then skip it
        properties[i]->onUpdate(deltaTime); //and call onUpdate on each property
    }
}

void Engine::GameObject::onPreRender(Engine::RenderPipeline* pipeline){ //calls onPreRender on all properties
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        if(!properties[i]->active) continue; //if property is inactive, then skip it
        properties[i]->onPreRender(pipeline); //and call onUpdate on each property
    }
}

void Engine::GameObject::onRender(RenderPipeline* pipeline){
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        if(!properties[i]->active) continue; //if property is inactive, then skip it
        properties[i]->onRender(pipeline); //and call onUpdate on each property
    }
}

void Engine::GameObject::copyTo(GameObject* dest){
    dest->alive = this->alive;
    dest->active = this->active;
    dest->hasParent = this->hasParent;
    dest->parent = this->parent;
    dest->str_id = this->str_id;
}

void Engine::GameObject::clearAll(){
    unsigned int props_num = static_cast<unsigned int>(this->props_num);

    for(unsigned int prop_i = 0; prop_i < props_num; prop_i ++){ //Walk through all children an remove them
        //Obtain pointer to property
        GameObjectProperty* prop_ptr = properties[prop_i];
        prop_ptr->onObjectDeleted(); //Call on object deletion
        delete prop_ptr; //Destroy property
    }
    this->props_num = 0; //Set property counter to zero
    children.clear();

}

void Engine::GameObject::trimChildrenArray(){
    for (unsigned int i = 0; i < children.size(); i ++) { //Iterating over all objects
        if(children[i].isEmpty() == true){ //If object marked as deleted
            for (unsigned int obj_i = i + 1; obj_i < children.size(); obj_i ++) { //Iterate over all next chidren
                children[obj_i - 1] = children[obj_i]; //Move it to previous place
            }
            children.resize(children.size() - 1); //Reduce vector length
        }
    }
}
