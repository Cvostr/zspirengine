#include "../../headers/world/World.h"
#include "../../headers/world/go_properties.h"
#include "../../headers/misc/randomg.h"

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
    props_num = 0; //No props by default
    alive = true; //Object exist by default
    hasParent = false; //No parent by default
    active = true;
    IsStatic = false; //Object is dynamic by default

    world_ptr = nullptr;
    label_ptr = nullptr;

    genRandomString(&this->str_id, 15); //Generate random string ID

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

bool Engine::GameObject::addProperty(PROPERTY_TYPE property){
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

void Engine::GameObject::removeProperty(int index){
    GameObjectProperty* prop_ptr = static_cast<GameObjectProperty*>(this->properties[index]);
    //Call onObjectDeleted() on removing property
    prop_ptr->onObjectDeleted();
    //set as deleted
    this->properties[index] = nullptr;
    //Trim properties pointers vector
    for(unsigned int i = static_cast<unsigned int>(index); i < props_num - 1; i ++){
        properties[i] = properties[i + 1];
    }
    //Reduce variable props_num
    props_num -= 1;
}


void Engine::GameObject::addChildObject(GameObjectLink link, bool updTransform){
    GameObjectLink _link = link;
    _link.updLinkPtr(); //Calculating object pointer
    _link.ptr->hasParent = true; //Object now has a parent (if it has't before)
    _link.ptr->parent.obj_str_id = this->getLinkToThisObject().obj_str_id; //Assigning pointer to new parent
    _link.ptr->parent.world_ptr = this->getLinkToThisObject().world_ptr;
    _link.ptr->parent.updLinkPtr();

    //Updating child's transform
    //Now check, if it is possible
    TransformProperty* Pobj_transform = getTransformProperty();
    TransformProperty* Cobj_transform = link.ptr->getTransformProperty();
    //Check, If both objects have Transform property
    if(updTransform && Pobj_transform != nullptr && Cobj_transform != nullptr){ //If both objects have mesh property
        //if so, then add parent transform to child
        ZSVECTOR3 p_translation = ZSVECTOR3(0,0,0);
        ZSVECTOR3 p_scale = ZSVECTOR3(1,1,1);
        ZSVECTOR3 p_rotation = ZSVECTOR3(0,0,0);
        //get absolute transform of parent
        Pobj_transform->getAbsoluteParentTransform(p_translation, p_scale, p_rotation); //Collecting transforms
        //Add parent transform to child
        Cobj_transform->translation = Cobj_transform->translation - p_translation;
        Cobj_transform->scale = Cobj_transform->scale / p_scale;
        Cobj_transform->rotation = Cobj_transform->rotation - p_rotation;
        //Update transform matrix
        Cobj_transform->updateMatrix(); //Update transform matrix
    }
    //Add child to children vector
    this->children.push_back(_link);
}

void Engine::GameObject::removeChildObject(GameObjectLink link){
    unsigned int children_am = static_cast<unsigned int>(children.size()); //get children amount
    for(unsigned int i = 0; i < children_am; i++){ //Iterate over all children in object
        GameObjectLink* link_ptr = &children[i];
        if(link.obj_str_id.compare(link_ptr->obj_str_id) == 0){ //if str_id in requested link compares to iteratable link
            GameObject* ptr = children[i].updLinkPtr();
            children[i].crack(); //Make link broken

            //Updating child's transform
            //Now check, if it is possible
            TransformProperty* Pobj_transform = getTransformProperty();
            TransformProperty* Cobj_transform = ptr->getTransformProperty();
            //Check, If both objects have Transform property
            if(Pobj_transform != nullptr && Cobj_transform != nullptr){ 
                //if so, then add parent transform to child
                ZSVECTOR3 p_translation = ZSVECTOR3(0,0,0);
                ZSVECTOR3 p_scale = ZSVECTOR3(1,1,1);
                ZSVECTOR3 p_rotation = ZSVECTOR3(0,0,0);
                //get absolute transform of parent
                Pobj_transform->getAbsoluteParentTransform(p_translation, p_scale, p_rotation);
                //Add parent transform to child
                Cobj_transform->translation = Cobj_transform->translation + p_translation;
                Cobj_transform->scale = Cobj_transform->scale * p_scale;
                Cobj_transform->rotation = Cobj_transform->rotation + Pobj_transform->rotation;
                //Update transform matrix
                Cobj_transform->updateMatrix(); 
            }
        }
    }
    //Remove broken link from vector
    trimChildrenArray(); 
}

std::string Engine::GameObject::getLabel(){
    return getLabelProperty()->label;
}

void Engine::GameObject::setLabel(std::string label){
    *this->label_ptr = label;
}

void Engine::GameObject::setActive(bool active){
    this->active = active;
}

Engine::TransformProperty* Engine::GameObject::getTransformProperty(){
    return getPropertyPtr<TransformProperty>();
}

Engine::LabelProperty* Engine::GameObject::getLabelProperty(){
    return static_cast<LabelProperty*>(getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL));
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

Engine::GameObjectProperty* Engine::GameObject::getPropertyPtrByTypeI(PROPERTY_TYPE property){
    unsigned int props = static_cast<unsigned int>(this->props_num);
    for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
        Engine::GameObjectProperty* property_ptr = this->properties[prop_i];
        if(property_ptr->type == property){ //If object already has one
            return property_ptr; //return it
        }
    }
    return nullptr;
}

void Engine::GameObject::onUpdate(int deltaTime){   //calls onUpdate on all properties
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        if(!properties[i]->active) continue; //if property is inactive, then skip it
        properties[i]->onUpdate(static_cast<float>(deltaTime)); //and call onUpdate on each property
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

void Engine::GameObject::onTrigger(GameObject* obj){
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        if(!properties[i]->active) continue; //if property is inactive, then skip it
        properties[i]->onTrigger(obj); //and call onUpdate on each property
    }
}

void Engine::GameObject::copyTo(GameObject* dest){
    dest->array_index = this->array_index;
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
        removeProperty(prop_i);
    }
    this->props_num = 0; //Set property counter to zero
    //Clear vector of children
    children.clear();
    hasParent = false;
    alive = false;
    active = false;
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

int Engine::GameObject::getAliveChildrenAmount() {
    int result = 0;
    for (unsigned int chi_i = 0; chi_i < children.size(); chi_i++) { //Now iterate over all children
        Engine::GameObjectLink* child_ptr = &children[chi_i];
        if (!child_ptr->isEmpty()) //if it points to something
            result += 1;
    }
    return result;
}

bool Engine::GameObject::hasMesh(){
    Engine::MeshProperty* mesh_prop = getPropertyPtr<MeshProperty>();
    if(mesh_prop != nullptr){
        if(!mesh_prop->active) return false;
        if(mesh_prop->mesh_ptr != nullptr) return true;
    }
    return false;
}

bool Engine::GameObject::hasTerrain(){
    Engine::TerrainProperty* terrain = getPropertyPtr<Engine::TerrainProperty>();
    if(terrain != nullptr){
        if(!terrain->active) return false;
        return true;
    }
    return false;
}

void Engine::GameObject::DrawMesh(RenderPipeline* pipeline) {
    Engine::MeshProperty* mesh_prop = static_cast<Engine::MeshProperty*>(this->getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH));
    Engine::TerrainProperty* terrain_prop = getPropertyPtr<Engine::TerrainProperty>();
    //Draw default mesh
    if (mesh_prop != nullptr) mesh_prop->mesh_ptr->Draw();
    if (terrain_prop != nullptr) terrain_prop->DrawMesh(pipeline);
}

void Engine::GameObject::DrawMeshInstanced(RenderPipeline* pipeline, unsigned int inst_num) {
    Engine::MeshProperty* mesh_prop = static_cast<Engine::MeshProperty*>(this->getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH));
    Engine::TerrainProperty* terrain_prop = getPropertyPtr<Engine::TerrainProperty>();
    //Draw default mesh
    if (mesh_prop != nullptr) mesh_prop->mesh_ptr->DrawInstanced(inst_num);
    if (terrain_prop != nullptr) terrain_prop->DrawMesh(pipeline);
}

bool Engine::GameObject::isRigidbody(){
    if(getPropertyPtr<RigidbodyProperty>() != nullptr || getPropertyPtr<ColliderProperty>() != nullptr)
        return true;

    return false;
}

void* Engine::GameObject::getPhysicalProperty(){
    ColliderProperty* coll = getPropertyPtr<ColliderProperty>();
    RigidbodyProperty* rigid = getPropertyPtr<RigidbodyProperty>();
    CharacterControllerProperty* controller = getPropertyPtr<CharacterControllerProperty>();
    TriggerProperty* trigger = getPropertyPtr<TriggerProperty>();
    //Variable to store result
    PhysicalProperty* phys = nullptr;

    if(rigid != nullptr)
        phys = rigid;
    else if (coll != nullptr){
        phys = coll;
    }else if(controller != nullptr){
        phys = controller;
    }else {
        phys = trigger;
    }

    return phys;
}

Engine::GameObject* Engine::GameObject::getChildObjectWithNodeLabel(std::string label){
    //This function works recursively
    //Iterate over all children in current object
    for (unsigned int i = 0; i < this->children.size(); i ++) {
        Engine::GameObjectLink* l = &this->children[i];
        Engine::NodeProperty* node_p = (l)->updLinkPtr()->getPropertyPtr<Engine::NodeProperty>();
        if (node_p == nullptr)
            continue;
        //if node's name match
        if(!node_p->node_label.compare(label))
            //Then return object with this node
            return l->updLinkPtr();
        //call function from this child
        GameObject* obj_ch = l->updLinkPtr()->getChildObjectWithNodeLabel(label);
        if(obj_ch != nullptr) return obj_ch;

    }
    return nullptr;
}

