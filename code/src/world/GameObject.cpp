#include "../../headers/world/World.hpp"
#include "../../headers/world/go_properties.h"
#include "../../headers/misc/misc.h"

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

Engine::GameObject::GameObject() : array_index(0),
                                   props_num(0),
                                   scripts_num(0),
                                   alive(true),
                                   mActive(true),
                                   hasParent(false), 
                                   IsStatic(false),
                                   mWorld(nullptr){

    genRandomString(&this->str_id, 15); //Generate random string ID
    mComponents[0] = nullptr;
    mScripts[0] = nullptr;
}

Engine::GameObject::~GameObject() {

}

Engine::GameObjectLink Engine::GameObject::getLinkToThisObject(){
    GameObjectLink link; //Definition of result link
    link.obj_str_id = this->str_id; //Placing string id
    link.world_ptr = this->mWorld; //Placing world pointer

    link.ptr = mWorld->getGameObjectByStrId(link.obj_str_id);
    return link;
}

bool Engine::GameObject::addProperty(PROPERTY_TYPE property){
    unsigned int props = static_cast<unsigned int>(this->props_num);

    if (props == OBJ_PROPS_SIZE)
        return false;

    for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
        IGameObjectComponent* property_ptr = this->mComponents[prop_i];
        if(property_ptr->type == property){ //If object already has one
            return false; //Exit function
        }
    }
    IGameObjectComponent* _ptr = allocProperty(property);

    _ptr->go_link = this->getLinkToThisObject();
    _ptr->go_link.updLinkPtr();
    _ptr->mWorld = this->mWorld; //Assign pointer to world
    this->mComponents[props_num] = _ptr; //Store property in gameobject
    this->props_num += 1;
    return true;
}

bool Engine::GameObject::addScript() {
    ZPScriptProperty* _ptr = static_cast<ZPScriptProperty*>(allocProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT));

    _ptr->go_link = this->getLinkToThisObject();
    _ptr->go_link.updLinkPtr();
    _ptr->mWorld = this->mWorld; //Assign pointer to world
    this->mScripts[scripts_num] = _ptr; //Store property in gameobject
    this->scripts_num += 1;
    return true;
}

void Engine::GameObject::removeProperty(int index){
    auto prop_ptr = this->mComponents[index];
    //Call onObjectDeleted() on removing property
    prop_ptr->onObjectDeleted();
    delete this->mComponents[index];
    //set as deleted
    this->mComponents[index] = nullptr;
    //Trim properties pointers vector
    for(unsigned int i = static_cast<unsigned int>(index); i < props_num - 1; i ++){
        mComponents[i] = mComponents[i + 1];
    }
    //Reduce variable props_num
    props_num -= 1;
}

void Engine::GameObject::removeProperty(Engine::IGameObjectComponent* pProp) {
    for (unsigned int i = 0; i < props_num; i++) {
        if (mComponents[i] == pProp)
            removeProperty(i);
    }
}

void Engine::GameObject::removeScript(int index) {
    auto script_ptr = this->mScripts[index];
    //Call onObjectDeleted() on removing property
    script_ptr->onObjectDeleted();
    delete this->mScripts[index];
    //set as deleted
    this->mScripts[index] = nullptr;
    //Trim properties pointers vector
    for (unsigned int i = static_cast<unsigned int>(index); i < scripts_num - 1; i++) {
        mScripts[i] = mScripts[i + 1];
    }
    //Reduce variable props_num
    scripts_num -= 1;
}
void Engine::GameObject::removeScript(Engine::IGameObjectComponent* pProp) {
    for (unsigned int i = 0; i < scripts_num; i++) {
        if (mScripts[i] == pProp)
            removeScript(i);
    }
}

asIScriptObject* Engine::GameObject::getScriptObjectWithName(const std::string& name) {
    for (unsigned int script_i = 0; script_i < scripts_num; script_i++) {
        ZPScriptProperty* script = static_cast<ZPScriptProperty*>(mScripts[script_i]);
        if (script->getScript()->getClassName().compare(name) == 0) {
            //script->getScript()->getMainClassPtr()->AddRef();
            return script->getScript()->getMainClassPtr();
        }
    }
    return nullptr;
}

void Engine::GameObject::addChildObject(GameObject* obj, bool updTransform) {
    GameObjectLink _link = obj->getLinkToThisObject();
    _link.updLinkPtr(); //Calculating object pointer
    _link.ptr->hasParent = true; //Object now has a parent (if it has't before)
    //Fill link to parent with information
    _link.ptr->mParent.obj_str_id = this->getLinkToThisObject().obj_str_id; //Assigning pointer to new parent
    _link.ptr->mParent.world_ptr = this->getLinkToThisObject().world_ptr;
    _link.ptr->mParent.updLinkPtr();

    //Updating child's transform
    //Now check, if it is possible
    TransformProperty* Pobj_transform = getTransformProperty();
    TransformProperty* Cobj_transform = obj->getTransformProperty();
    //Check, If both objects have Transform property
    if (updTransform && Pobj_transform != nullptr && Cobj_transform != nullptr) { //If both objects have mesh property
        //if so, then add parent transform to child
        ZSVECTOR3 p_translation = ZSVECTOR3(0, 0, 0);
        ZSVECTOR3 p_scale = ZSVECTOR3(1, 1, 1);
        ZSVECTOR3 p_rotation = ZSVECTOR3(0, 0, 0);
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
    this->mChildren.push_back(_link);
}

void Engine::GameObject::addChildObject(GameObjectLink link, bool updTransform){
    addChildObject(link.updLinkPtr(), updTransform);
}

void Engine::GameObject::removeChildObject(GameObject* obj) {
    size_t children_am = mChildren.size(); //get children amount
    for (size_t i = 0; i < children_am; i++) { //Iterate over all children in object
        GameObjectLink* link_ptr = &mChildren[i];
        if (obj->str_id.compare(link_ptr->obj_str_id) == 0) { //if str_id in requested link compares to iteratable link
            mChildren[i].crack(); //Make link broken

            //Updating child's transform
            //Now check, if it is possible
            TransformProperty* Pobj_transform = getTransformProperty();
            TransformProperty* Cobj_transform = obj->getTransformProperty();
            //Check, If both objects have Transform property
            if (Pobj_transform != nullptr && Cobj_transform != nullptr) {
                //if so, then add parent transform to child
                ZSVECTOR3 p_translation = ZSVECTOR3(0, 0, 0);
                ZSVECTOR3 p_scale = ZSVECTOR3(1, 1, 1);
                ZSVECTOR3 p_rotation = ZSVECTOR3(0, 0, 0);
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

void Engine::GameObject::removeChildObject(GameObjectLink link){
    removeChildObject(link.updLinkPtr());
}

std::string* Engine::GameObject::getLabelPtr() {
    return &getLabelProperty()->label;
}

const std::string& Engine::GameObject::getLabel() {
    return getLabelProperty()->label;
}

void Engine::GameObject::setLabel(const std::string& label){
    LabelProperty* label_prop = getLabelProperty(); //Obtain pointer to label property
    label_prop->label = label; //set new name
}

bool Engine::GameObject::isActive() {
    bool isParentActive = true;
    if(hasParent && mActive)
        if (!mParent.updLinkPtr()->isActive())
            isParentActive = false;

    return mActive && isParentActive;
}

Engine::GameObject* Engine::GameObject::getChild(unsigned int index) {
    return mChildren[index].updLinkPtr();
}

Engine::TransformProperty* Engine::GameObject::getTransformProperty(){
    return getPropertyPtr<TransformProperty>();
}

Engine::LabelProperty* Engine::GameObject::getLabelProperty(){
    return static_cast<LabelProperty*>(getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL));
}

Engine::IGameObjectComponent* Engine::GameObject::getPropertyPtrByType(PROPERTY_TYPE type){
    unsigned int props = static_cast<unsigned int>(this->props_num);
    for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
        IGameObjectComponent* property_ptr = this->mComponents[prop_i];
        if(property_ptr->type == type){ //If object already has one
            return property_ptr; //return it
        }
    }
    return nullptr;
}

Engine::IGameObjectComponent* Engine::GameObject::getPropertyPtrByTypeI(int property){
    unsigned int props = static_cast<unsigned int>(this->props_num);
    for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
        Engine::IGameObjectComponent* property_ptr = this->mComponents[prop_i];
        if((int)property_ptr->type == property){ //If object already has one
            return property_ptr; //return it
        }
    }
    return nullptr;
}

void Engine::GameObject::onStart() {
    for (unsigned int i = 0; i < props_num; i++) { //iterate over all properties
        if (!mComponents[i]->active) continue; //if property is inactive, then skip it
        mComponents[i]->onStart(); //and call onStart on each property
    }
    //Work with scripts
    for (unsigned int i = 0; i < scripts_num; i++) { //iterate over all scripts
        if (!mScripts[i]->active) continue; //if script is inactive, then skip it
        mScripts[i]->onStart(); //and call onStart on each script
    }
}

void Engine::GameObject::onStop() {
    for (unsigned int i = 0; i < props_num; i++) { //iterate over all properties
        if (!mComponents[i]->active) continue; //if property is inactive, then skip it
        mComponents[i]->onStop(); //and call onStart on each property
    }
    //Work with scripts
    for (unsigned int i = 0; i < scripts_num; i++) { //iterate over all scripts
        if (!mScripts[i]->active) continue; //if script is inactive, then skip it
        mScripts[i]->onStop(); //and call onStart on each script
    }
}

void Engine::GameObject::onUpdate(int deltaTime){   //calls onUpdate on all properties
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        if(!mComponents[i]->active) continue; //if property is inactive, then skip it
        mComponents[i]->onUpdate(static_cast<float>(deltaTime)); //and call onUpdate on each property
    }
    //Work with scripts
    for (unsigned int i = 0; i < scripts_num; i++) { //iterate over all scripts
        if (!mScripts[i]->active) continue; //if script is inactive, then skip it
        mScripts[i]->onUpdate(static_cast<float>(deltaTime)); //and call onUpdate on each script
    }
}

void Engine::GameObject::onPreRender(Engine::RenderPipeline* pipeline){ //calls onPreRender on all properties
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        if(!mComponents[i]->active) continue; //if property is inactive, then skip it
        mComponents[i]->onPreRender(pipeline); //and call onUpdate on each property
    }
}

void Engine::GameObject::onRender(RenderPipeline* pipeline){
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        if(!mComponents[i]->active) continue; //if property is inactive, then skip it
        mComponents[i]->onRender(pipeline); //and call onUpdate on each property
    }
}

void Engine::GameObject::onTrigger(GameObject* obj){
    for(unsigned int i = 0; i < props_num; i ++){ //iterate over all properties
        if(!mComponents[i]->active) continue; //if property is inactive, then skip it
        mComponents[i]->onTrigger(obj); //and call onUpdate on each property
    }
    //Work with scripts
    for (unsigned int i = 0; i < scripts_num; i++) { //iterate over all scripts
        if (!mScripts[i]->active) continue; //if script is inactive, then skip it
        mScripts[i]->onTrigger(obj); //and call onUpdate on each script
    }
}

void Engine::GameObject::onTriggerEnter(GameObject* obj) {
    for (unsigned int i = 0; i < props_num; i++) { //iterate over all properties
        if (!mComponents[i]->active) continue; //if property is inactive, then skip it
        mComponents[i]->onTriggerEnter(obj); //and call onUpdate on each property
    }
    //Work with scripts
    for (unsigned int i = 0; i < scripts_num; i++) { //iterate over all scripts
        if (!mScripts[i]->active) continue; //if script is inactive, then skip it
        mScripts[i]->onTriggerEnter(obj); //and call onUpdate on each script
    }
}
void Engine::GameObject::onTriggerExit(GameObject* obj) {
    for (unsigned int i = 0; i < props_num; i++) { //iterate over all properties
        if (!mComponents[i]->active) continue; //if property is inactive, then skip it
        mComponents[i]->onTriggerExit(obj); //and call onUpdate on each property
    }
    //Work with scripts
    for (unsigned int i = 0; i < scripts_num; i++) { //iterate over all scripts
        if (!mScripts[i]->active) continue; //if script is inactive, then skip it
        mScripts[i]->onTriggerExit(obj); //and call onUpdate on each script
    }
}

void Engine::GameObject::copyTo(GameObject* dest){
    dest->array_index = this->array_index;
    dest->alive = this->alive;
    dest->mActive = this->mActive;
    dest->hasParent = this->hasParent;
    dest->mParent = this->mParent;
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
    mChildren.clear();
    hasParent = false;
    alive = false;
    mActive = false;
}

void Engine::GameObject::trimChildrenArray(){
    for (unsigned int i = 0; i < mChildren.size(); i ++) { //Iterating over all objects
        if(mChildren[i].isEmpty() == true){ //If object marked as deleted
            for (unsigned int obj_i = i + 1; obj_i < mChildren.size(); obj_i ++) { //Iterate over all next chidren
                mChildren[obj_i - 1] = mChildren[obj_i]; //Move it to previous place
            }
            mChildren.resize(mChildren.size() - 1); //Reduce vector length
        }
    }
}

int Engine::GameObject::getAliveChildrenAmount() {
    int result = 0;
    for (unsigned int chi_i = 0; chi_i < mChildren.size(); chi_i++) { //Now iterate over all children
        Engine::GameObjectLink* child_ptr = &mChildren[chi_i];
        if (!child_ptr->isEmpty()) //if it points to something
            result += 1;
    }
    return result;
}

bool Engine::GameObject::hasMesh(){
    Engine::MeshProperty* mesh_prop = getPropertyPtr<MeshProperty>();
    if(mesh_prop != nullptr){
        if(!mesh_prop->isActive()) return false;
        if(mesh_prop->mesh_ptr != nullptr) return true;
    }
    return false;
}

bool Engine::GameObject::hasTerrain(){
    Engine::TerrainProperty* terrain = getPropertyPtr<Engine::TerrainProperty>();
    if(terrain != nullptr){
        if(!terrain->isActive()) return false;
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

Engine::PhysicalProperty* Engine::GameObject::getPhysicalProperty() {
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

Engine::GameObject* Engine::GameObject::getChildObjectWithNodeLabel(const std::string& label){
    //This function works recursively
    //Iterate over all children in current object
    for (unsigned int i = 0; i < this->mChildren.size(); i ++) {
        Engine::GameObjectLink* l = &this->mChildren[i];
        Engine::GameObject* ChildPtr = l->updLinkPtr();
        if (ChildPtr == nullptr)
            continue;
        Engine::NodeProperty* node_p = ChildPtr->getPropertyPtr<Engine::NodeProperty>();
        if (node_p == nullptr)
            continue;
        //if node's name match
        if(!node_p->node_label.compare(label))
            //Then return object with this node
            return l->updLinkPtr();
        //call function from this child
        GameObject* obj_ch = l->updLinkPtr()->getChildObjectWithNodeLabel(label);
        if(obj_ch != nullptr) 
            return obj_ch;

    }
    return nullptr;
}

void Engine::GameObject::putToSnapshot(GameObjectSnapshot* snapshot) {
    //set base variables
    snapshot->props_num = 0;
    snapshot->scripts_num = 0;
    snapshot->parent_link = this->mParent;
    snapshot->obj_array_ind = this->array_index;
    //Copy Object
    this->copyTo(&snapshot->reserved_obj);
    //Copy all properties
    for (unsigned int i = 0; i < this->props_num; i++) {
        Engine::IGameObjectComponent* prop_ptr = this->mComponents[i];
        Engine::IGameObjectComponent* new_prop_ptr = Engine::allocProperty(prop_ptr->type);
        prop_ptr->copyTo(new_prop_ptr);
        snapshot->properties[snapshot->props_num] = new_prop_ptr;
        snapshot->props_num += 1;
    }
    for (unsigned int i = 0; i < this->scripts_num; i++) {
        Engine::ZPScriptProperty* script_ptr = static_cast<Engine::ZPScriptProperty*>(this->mScripts[i]);
        Engine::ZPScriptProperty* new_script_ptr = static_cast<Engine::ZPScriptProperty*>(
            Engine::allocProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT));
        script_ptr->copyTo(new_script_ptr);
        snapshot->mScripts[snapshot->scripts_num] = new_script_ptr;
        snapshot->scripts_num += 1;
    }
    snapshot->children_snapshots.resize(this->mChildren.size());
    //Copy all children links
    for (size_t i = 0; i < this->mChildren.size(); i++) {
        snapshot->children.push_back(this->mChildren[i]);
        //Call putToSnapshot() on object
        mChildren[i].ptr->putToSnapshot(&snapshot->children_snapshots[i]);
    }
}

void Engine::GameObject::setMeshSkinningRootNodeRecursively(GameObject* rootNode) {
    Engine::MeshProperty* mesh = getPropertyPtr<Engine::MeshProperty>();
    if (mesh)
        mesh->skinning_root_node = rootNode;

    for (size_t ch_i = 0; ch_i < mChildren.size(); ch_i++) {
        GameObject* obj_ptr = mChildren[ch_i].updLinkPtr();
        obj_ptr->setMeshSkinningRootNodeRecursively(rootNode);
    }
}

const BoundingBox3& Engine::GameObject::getBoundingBox() {
    if (!hasMesh())
        return BoundingBox3();

    mBoundingBox = getPropertyPtr<MeshProperty>()->mesh_ptr->mesh_ptr->mBoundingBox;
    mBoundingBox.ApplyTransform(getTransformProperty()->transform_mat);
    
    for (size_t i = 0; i < getChildrenNum(); i++) {
        BoundingBox3 child_bb = getChild(i)->getBoundingBox();
        mBoundingBox.Extend(child_bb);
    }

    return mBoundingBox;
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

    for (size_t child = 0; child < this->children.size(); child++) {
        children_snapshots[child].clear();
    }
    children_snapshots.clear(); //Free snapshot vector
    this->children.clear(); //Free link vector
}
Engine::GameObjectSnapshot::GameObjectSnapshot() : props_num(0), scripts_num(0), obj_array_ind(0) {}