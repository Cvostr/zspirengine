#include "../../../headers/world/go_properties.h"
#include "../../../headers/world/tile_properties.h"

extern ZSGAME_DATA* game_data;

Engine::GameObjectProperty::GameObjectProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_NONE;
    active = true;
    world_ptr = nullptr;
}
void Engine::GameObjectProperty::setActive(bool active){
    this->active = active;
}
bool Engine::GameObjectProperty::isActive(){
    return active && go_link.updLinkPtr()->active;
}
void Engine::GameObjectProperty::copyTo(GameObjectProperty* dest){
    dest->active = this->active;
    dest->world_ptr = this->world_ptr;
}
void Engine::GameObjectProperty::onStart() {

}
void Engine::GameObjectProperty::onStop() {

}
void Engine::GameObjectProperty::onUpdate(float deltaTime){

}
void Engine::GameObjectProperty::onPreRender(RenderPipeline* pipeline){

}
void Engine::GameObjectProperty::onRender(RenderPipeline* pipeline){

}
void Engine::GameObjectProperty::onAddToObject(){

}
void Engine::GameObjectProperty::onObjectDeleted(){

}
void Engine::GameObjectProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {

}
void Engine::GameObjectProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {

}
void Engine::GameObjectProperty::onTrigger(Engine::GameObject* obj){
    assert(obj);
}

void Engine::GameObjectProperty::addPropertyInterfaceToInspector(){
}

void Engine::GameObjectProperty::onValueChanged(){

}

Engine::GameObjectProperty::~GameObjectProperty(){

}

Engine::GameObjectProperty* Engine::GameObject::allocProperty(PROPERTY_TYPE type){
    GameObjectProperty* _ptr = nullptr;
    PROPERTY_TYPE _type = static_cast<PROPERTY_TYPE>(type);
    switch (_type) {
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM:{ //If type is transfrom
            _ptr = static_cast<GameObjectProperty*>(new TransformProperty); //Allocation of transform in heap
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL:{
            LabelProperty* ptr = new LabelProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH:{
            _ptr = static_cast<GameObjectProperty*>(new MeshProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE:{
            _ptr = static_cast<GameObjectProperty*>(new NodeProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_LIGHTSOURCE:{
            _ptr = static_cast<GameObjectProperty*>(new LightsourceProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT:{
            ZPScriptProperty* ptr = new ZPScriptProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_AUDSOURCE:{
            _ptr = static_cast<GameObjectProperty*>(new AudioSourceProperty);
            break;
        }

        case PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL:{
            _ptr = static_cast<GameObjectProperty*>(new MaterialProperty);
            break;
        }

        case PROPERTY_TYPE::GO_PROPERTY_TYPE_COLLIDER:{
            _ptr = static_cast<GameObjectProperty*>(new ColliderProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_RIGIDBODY:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::RigidbodyProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_CHARACTER_CONTROLLER:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::CharacterControllerProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE_GROUP:{
            _ptr = static_cast<GameObjectProperty*>(new TileGroupProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE:{
            _ptr = static_cast<GameObjectProperty*>(new TileProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_ANIMATION:{
            _ptr = static_cast<GameObjectProperty*>(new AnimationProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_SKYBOX:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::SkyboxProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_SHADOWCASTER:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::ShadowCasterProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TERRAIN:{
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::TerrainProperty);
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRIGGER: {
            _ptr = static_cast<Engine::GameObjectProperty*>(new Engine::TriggerProperty);
            break;
        }
    }
    return _ptr;
}

void Engine::bindOPropertyScriptingToAngel(AGScriptMgr* mgr) {

    
}

Engine::LabelProperty::LabelProperty(){
    this->type = PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL;
}

Engine::LabelProperty::~LabelProperty() {

}

void Engine::LabelProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    std::string label;
    while (data[offset] != ' ' && data[offset] != '\n') {
        label += data[offset];
        offset++;
    }
    obj->label_ptr = &this->label; //Making GameObjects's pointer to string in label property
    this->label = label; //Write loaded string
}

Engine::MaterialProperty::MaterialProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL;

    receiveShadows = true;
    this->material_ptr = nullptr;
}

void Engine::MaterialProperty::copyTo(Engine::GameObjectProperty* dest){
    //MaterialShaderProperty
    if(dest->type != PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL) return;

    //Do base things
    GameObjectProperty::copyTo(dest);

    MaterialProperty* mat_prop = static_cast<MaterialProperty*>(dest);
    mat_prop->material_path = this->material_path;
    mat_prop->material_ptr = this->material_ptr;
    mat_prop->group_label = this->group_label;
}

void Engine::MaterialProperty::setMaterial(Material* mat){
    this->material_ptr = mat;
    this->material_path = mat->file_path;
    this->group_label = mat->group_ptr->groupCaption;
}

void Engine::MaterialProperty::setMaterial(std::string path){
    Material* newmat_ptr = game_data->resources->getMaterialByLabel(path)->material;
    setMaterial(newmat_ptr);
}

void Engine::MaterialProperty::_setMaterial(MaterialResource* mat) {
    setMaterial(mat->material);
}

void Engine::MaterialProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    //Read material path
    while (data[offset] != ' ' && data[offset] != '\n') {
        material_path += data[offset];
        offset++;
    }
    //get material by label
    material_ptr = game_data->resources->getMaterialByLabel(material_path)->material; //find it and process

    offset++;
    //Read receiveShadows boolean
    memcpy(&receiveShadows, data + offset, sizeof(bool));
}

Engine::NodeProperty::NodeProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE;

    scale = ZSVECTOR3(1.f, 1.f, 1.f);
    translation = ZSVECTOR3(0.f, 0.f, 0.f);
    rotation = ZSQUATERNION(0.f, 0.f, 0.f, 0.f);
}

void Engine::NodeProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't Node property

    //Do base things
    GameObjectProperty::copyTo(dest);
    NodeProperty* _dest = static_cast<NodeProperty*>(dest);

    _dest->transform_mat = this->transform_mat;
    _dest->node_label = this->node_label;
}

void Engine::NodeProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    while (data[offset] != ' ' && data[offset] != '\n') {
        node_label += data[offset];
        offset++;
    }
    //Skip 1 byte
    offset++;
    //Now read node matrix
    for (unsigned int m_i = 0; m_i < 4; m_i++) {
        for (unsigned int m_j = 0; m_j < 4; m_j++) {
            float* m_v = &transform_mat.m[m_i][m_j];
            memcpy(m_v, data + offset, sizeof(float));
            offset += sizeof(float);
        }
    }
}

Engine::TriggerProperty::TriggerProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_TRIGGER;
    coll_type = COLLIDER_TYPE::COLLIDER_TYPE_CUBE;
    created = false;
    m_ghost = nullptr;
}

void Engine::TriggerProperty::onUpdate(float deltaTime) {
    if (!created) {
        initGhost();
    }
    else {
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

Engine::SkyboxProperty::SkyboxProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_SKYBOX;
}
