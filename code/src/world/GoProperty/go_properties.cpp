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
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_SCRIPTGROUP:{
            ScriptGroupProperty* ptr = new ScriptGroupProperty;
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

Engine::MeshProperty::MeshProperty(){
    this->type = PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH;
    mesh_ptr = nullptr;

    castShadows = true;

    this->skinning_root_node = nullptr;
    this->rootNodeStr = "@none";
}
void Engine::MeshProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't transform

    MeshProperty* _dest = static_cast<MeshProperty*>(dest);
    _dest->resource_relpath = resource_relpath;
    _dest->mesh_ptr = mesh_ptr;
    _dest->castShadows = castShadows;
    _dest->rootNodeStr = rootNodeStr;
}
void Engine::MeshProperty::updateMeshPtr(){
    this->mesh_ptr = game_data->resources->getMeshByLabel(this->resource_relpath);
}

void Engine::MeshProperty::setMeshResource(MeshResource* resource) {
    this->resource_relpath = resource->resource_label;
    updateMeshPtr();
}

void Engine::MeshProperty::onRender(Engine::RenderPipeline* pipeline){
    if(this->skinning_root_node == nullptr && mesh_ptr->mesh_ptr->hasBones())
        skinning_root_node = go_link.world_ptr->getGameObjectByLabel(this->rootNodeStr);
}

void Engine::MeshProperty::onValueChanged(){
    updateMeshPtr();
}

void Engine::MeshProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;

    while (data[offset] != ' ' && data[offset] != '\n') {
        resource_relpath += data[offset];
        offset++;
    }
    offset++;

    rootNodeStr.clear();
    //Read Root Node label
    while (data[offset] != ' ' && data[offset] != '\n') {
        rootNodeStr += data[offset];
        offset++;
    }
    //Pointer will now point to mesh resource
    updateMeshPtr();

    offset++;
    //Read castShadows bool
    memcpy(&castShadows, data + offset, sizeof(bool));
    offset += sizeof(bool);
}

Engine::ScriptGroupProperty::ScriptGroupProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_SCRIPTGROUP;

    scr_num = 0;
    this->scripts_attached.resize(static_cast<unsigned int>(this->scr_num));
}

Engine::ScriptGroupProperty::~ScriptGroupProperty() {
    scripts_attached.clear();
    path_names.clear();
}

void Engine::ScriptGroupProperty::onUpdate(float deltaTime){
    for(unsigned int script_i = 0; script_i < this->scripts_attached.size(); script_i ++){
        Engine::ObjectScript* script_ptr = &this->scripts_attached[script_i]; //Obtain pointer to script
        //if script isn't created, then create it.
        if(!script_ptr->created){
            script_ptr->script_content.clear();
            script_ptr->script_content = game_data->resources->getScriptByLabel(script_ptr->name)->script_content;
            script_ptr->_InitScript();
            script_ptr->_callStart(go_link.updLinkPtr(), go_link.world_ptr);
        }

        script_ptr->_callDraw(deltaTime); //Run onDraw() function in script
    }
}

void Engine::ScriptGroupProperty::copyTo(Engine::GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't script group

    //Do base things
    GameObjectProperty::copyTo(dest);

    ScriptGroupProperty* _dest = static_cast<ScriptGroupProperty*>(dest);
    _dest->scr_num = this->scr_num;

    //resize data vectors
    _dest->scripts_attached.resize(static_cast<unsigned int>(scr_num));
    _dest->path_names.resize(static_cast<unsigned int>(scr_num));
    //Copy data
    for(unsigned int script_i = 0; script_i < static_cast<unsigned int>(scr_num); script_i ++){
        _dest->scripts_attached[script_i] = this->scripts_attached[script_i];
        _dest->path_names[script_i] = this->path_names[script_i];
    }
}


void Engine::ScriptGroupProperty::shutdown(){
    //Iterate over all scripts and call _DestroyScript() on each
    for(unsigned int script_i = 0; script_i < static_cast<unsigned int>(scr_num); script_i ++){
        this->scripts_attached[script_i]._DestroyScript();
    }
}

Engine::ObjectScript* Engine::ScriptGroupProperty::getScriptByName(std::string name){
    for(unsigned int script_i = 0; script_i < static_cast<unsigned int>(scr_num); script_i ++){
        if(!name.compare(scripts_attached[script_i].name))
            return &scripts_attached[script_i];
    }
    return nullptr;
}

void Engine::ScriptGroupProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    //Read scripts number
    memcpy(&scr_num, data + offset, sizeof(int));
    offset += sizeof(int) + 1;
    //resize arrays
    scripts_attached.resize(static_cast<unsigned int>(scr_num));
    //iterate over all scripts and read their path
    for (unsigned int script_w_i = 0; script_w_i < static_cast<unsigned int>(scr_num); script_w_i++) {
        std::string script_path;
        //Read script path
        while (data[offset] != ' ' && data[offset] != '\n') {
            script_path += data[offset];
            offset++;
        }
        //Writing name (file path)
        scripts_attached[script_w_i].name = script_path;
        ScriptResource* res = game_data->resources->getScriptByLabel(script_path);
    }
}

void Engine::LightsourceProperty::copyTo(Engine::GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't Lightsource, then exit

    //Do base things
    GameObjectProperty::copyTo(dest);

    LightsourceProperty* _dest = static_cast<LightsourceProperty*>(dest);
    _dest->color = color;
    _dest->intensity = intensity;
    _dest->range = range;
    _dest->light_type = light_type;
}

void Engine::LightsourceProperty::onPreRender(Engine::RenderPipeline* pipeline){
    Engine::TransformProperty* transform_prop = go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();

    pipeline->addLight(static_cast<void*>(this)); //put light pointer to vector

    //check, if light transformation changed
    if((this->last_pos != transform_prop->abs_translation || this->last_rot != transform_prop->rotation)){
        ZSVECTOR3* rot_vec_ptr = &transform_prop->rotation;
        this->direction = _getDirection(rot_vec_ptr->X, rot_vec_ptr->Y, rot_vec_ptr->Z);
        //store new transform values
        this->last_pos = transform_prop->abs_translation;
        this->last_rot = transform_prop->abs_rotation;
    }
}

Engine::LightsourceProperty::LightsourceProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_LIGHTSOURCE;
    active = true;
    light_type = LIGHTSOURCE_TYPE::LIGHTSOURCE_TYPE_DIRECTIONAL; //base type is directional

    intensity = 1.0f; //base light instensity is 1
    range = 10.0f;
    spot_angle = 12.5f;
}

void Engine::LightsourceProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    //Read Type of Light Source
    memcpy(&light_type, data + offset, sizeof(LIGHTSOURCE_TYPE));
    offset += sizeof(LIGHTSOURCE_TYPE);
    //Read Intensity of source
    memcpy(&intensity, data + offset, sizeof(float));
    offset += sizeof(float);
    //Read Range of source
    memcpy(&range, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&spot_angle, data + offset, sizeof(float));
    offset += sizeof(float);

    float cl_r;
    float cl_g;
    float cl_b;
    //Read light color
    memcpy(&cl_r, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&cl_g, data + offset, sizeof(float));
    offset += sizeof(float); 
    memcpy(&cl_b, data + offset, sizeof(float));
    offset += sizeof(float);

    color = ZSRGBCOLOR(cl_r, cl_g, cl_b);
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
        for (int i = 0; i < overlaping; i++) {
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
