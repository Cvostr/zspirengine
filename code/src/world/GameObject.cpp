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
    array_index = 0;
    props_num = 0; //No props by default
    scripts_num = 0; //No scripts by default
    alive = true; //Object exist by default
    hasParent = false; //No parent by default
    active = true;
    IsStatic = false; //Object is dynamic by default

    world_ptr = nullptr;
    label_ptr = nullptr;

    genRandomString(&this->str_id, 15); //Generate random string ID
    properties[0] = nullptr;
    scripts[0] = nullptr;
}

Engine::GameObject::~GameObject() {

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

    if (props == OBJ_PROPS_SIZE)
        return false;

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

bool Engine::GameObject::addScript() {
    ZPScriptProperty* _ptr = static_cast<ZPScriptProperty*>(allocProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT));

    _ptr->go_link = this->getLinkToThisObject();
    _ptr->go_link.updLinkPtr();
    _ptr->world_ptr = this->world_ptr; //Assign pointer to world
    this->scripts[scripts_num] = _ptr; //Store property in gameobject
    this->scripts_num += 1;
    return true;
}

void Engine::GameObject::removeProperty(int index){
    auto prop_ptr = this->properties[index];
    //Call onObjectDeleted() on removing property
    prop_ptr->onObjectDeleted();
    delete this->properties[index];
    //set as deleted
    this->properties[index] = nullptr;
    //Trim properties pointers vector
    for(unsigned int i = static_cast<unsigned int>(index); i < props_num - 1; i ++){
        properties[i] = properties[i + 1];
    }
    //Reduce variable props_num
    props_num -= 1;
}

void Engine::GameObject::removeProperty(Engine::GameObjectProperty* pProp) {
    for (unsigned int i = 0; i < props_num; i++) {
        if (properties[i] == pProp)
            removeProperty(i);
    }
}

void Engine::GameObject::removeScript(int index) {
    auto script_ptr = this->scripts[index];
    //Call onObjectDeleted() on removing property
    script_ptr->onObjectDeleted();
    delete this->scripts[index];
    //set as deleted
    this->scripts[index] = nullptr;
    //Trim properties pointers vector
    for (unsigned int i = static_cast<unsigned int>(index); i < scripts_num - 1; i++) {
        scripts[i] = scripts[i + 1];
    }
    //Reduce variable props_num
    scripts_num -= 1;
}
void Engine::GameObject::removeScript(Engine::GameObjectProperty* pProp) {
    for (unsigned int i = 0; i < scripts_num; i++) {
        if (scripts[i] == pProp)
            removeScript(i);
    }
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
    LabelProperty* label_prop = getLabelProperty(); //Obtain pointer to label property
    label_prop->label = label; //set new name
    label_ptr = &label_prop->label; //update pointer
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

Engine::GameObjectProperty* Engine::GameObject::getPropertyPtrByTypeI(int property){
    unsigned int props = static_cast<unsigned int>(this->props_num);
    for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
        Engine::GameObjectProperty* property_ptr = this->properties[prop_i];
        if((int)property_ptr->type == property){ //If object already has one
            return property_ptr; //return it
        }
    }
    return nullptr;
}

void Engine::GameObject::onStart() {
    for (unsigned int i = 0; i < props_num; i++) { //iterate over all properties
        if (!properties[i]->active) continue; //if property is inactive, then skip it
        properties[i]->onStart(); //and call onStart on each property
    }
    //Work with scripts
    for (unsigned int i = 0; i < scripts_num; i++) { //iterate over all scripts
        if (!scripts[i]->active) continue; //if script is inactive, then skip it
        scripts[i]->onStart(); //and call onStart on each script
    }
}

void Engine::GameObject::onStop() {
    for (unsigned int i = 0; i < props_num; i++) { //iterate over all properties
        if (!properties[i]->active) continue; //if property is inactive, then skip it
        properties[i]->onStop(); //and call onStart on each property
    }
    //Work with scripts
    for (unsigned int i = 0; i < scripts_num; i++) { //iterate over all scripts
        if (!scripts[i]->active) continue; //if script is inactive, then skip it
        scripts[i]->onStop(); //and call onStart on each script
    }
}

void Engine::GameObject::onUpdate(int deltaTime){   //calls onUpdate on all properties
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        if(!properties[i]->active) continue; //if property is inactive, then skip it
        properties[i]->onUpdate(static_cast<float>(deltaTime)); //and call onUpdate on each property
    }
    //Work with scripts
    for (unsigned int i = 0; i < scripts_num; i++) { //iterate over all scripts
        if (!scripts[i]->active) continue; //if script is inactive, then skip it
        scripts[i]->onUpdate(static_cast<float>(deltaTime)); //and call onUpdate on each script
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
    //Work with scripts
    for (unsigned int i = 0; i < scripts_num; i++) { //iterate over all scripts
        if (!scripts[i]->active) continue; //if script is inactive, then skip it
        scripts[i]->onTrigger(obj); //and call onUpdate on each script
    }
}

void Engine::GameObject::onTriggerEnter(GameObject* obj) {
    for (unsigned int i = 0; i < props_num; i++) { //iterate over all properties
        if (!properties[i]->active) continue; //if property is inactive, then skip it
        properties[i]->onTriggerEnter(obj); //and call onUpdate on each property
    }
}
void Engine::GameObject::onTriggerExit(GameObject* obj) {
    for (unsigned int i = 0; i < props_num; i++) { //iterate over all properties
        if (!properties[i]->active) continue; //if property is inactive, then skip it
        properties[i]->onTriggerExit(obj); //and call onUpdate on each property
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
        removeProperty(0);
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

    if(rigid != nullptr) //if rigidbody component exists
        phys = rigid; //then return rigidbody
    else if (coll != nullptr){ //else, if collider component exists
        phys = coll; //return collider 
    }else if(controller != nullptr){ //else, if char controller component exists
        phys = controller; //return character controller
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


void Engine::GameObject::putToSnapshot(GameObjectSnapshot* snapshot) {
    snapshot->props_num = 0;
    snapshot->scripts_num = 0;

    snapshot->parent_link = this->parent;
    snapshot->obj_array_ind = this->array_index;

    this->copyTo(&snapshot->reserved_obj);
    //Copy all properties
    for (unsigned int i = 0; i < this->props_num; i++) {
        Engine::GameObjectProperty* prop_ptr = this->properties[i];
        Engine::GameObjectProperty* new_prop_ptr = Engine::allocProperty(prop_ptr->type);
        prop_ptr->copyTo(new_prop_ptr);
        snapshot->properties[snapshot->props_num] = new_prop_ptr;
        snapshot->props_num += 1;
    }
    for (unsigned int i = 0; i < this->scripts_num; i++) {
        Engine::ZPScriptProperty* script_ptr = static_cast<Engine::ZPScriptProperty*>(this->scripts[i]);
        Engine::ZPScriptProperty* new_script_ptr = static_cast<Engine::ZPScriptProperty*>(
            Engine::allocProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT));
        script_ptr->copyTo(new_script_ptr);
        snapshot->scripts[snapshot->scripts_num] = new_script_ptr;
        snapshot->scripts_num += 1;
    }
    snapshot->children_snapshots.resize(this->children.size());
    //Copy all children links
    for (unsigned int i = 0; i < this->children.size(); i++) {
        snapshot->children.push_back(this->children[i]);
        //Call putToSnapshot() on object
        children[i].ptr->putToSnapshot(&snapshot->children_snapshots[i]);
    }
}

void Engine::GameObject::setMeshSkinningRootNodeRecursively(GameObject* rootNode) {
    Engine::MeshProperty* mesh = getPropertyPtr<Engine::MeshProperty>();
    if (mesh)
        mesh->skinning_root_node = rootNode;

    for (unsigned int ch_i = 0; ch_i < children.size(); ch_i++) {
        GameObject* obj_ptr = children[ch_i].updLinkPtr();
        obj_ptr->setMeshSkinningRootNodeRecursively(rootNode);
    }
}

void Engine::GameObjectSnapshot::clear() {
    //free array of children
    this->children.clear();

    for (unsigned int prop = 0; prop < static_cast<unsigned int>(this->props_num); prop++) {
        auto prop_ptr = this->properties[prop];
        delete prop_ptr;
        prop_ptr = nullptr;
    }
    props_num = 0;

    for (unsigned int child = 0; child < this->children.size(); child++) {
        children_snapshots[child].clear();
    }
    children_snapshots.clear(); //Free snapshot vector
    this->children.clear(); //Free link vector
}

Engine::GameObjectSnapshot::GameObjectSnapshot() {
    props_num = 0;
    scripts_num = 0;
    obj_array_ind = 0;
}