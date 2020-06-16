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

Engine::TransformProperty::TransformProperty(){
    this->type = PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM;
    this->scale = ZSVECTOR3(1.f, 1.f, 1.f);
}

void Engine::TransformProperty::updateMatrix(){
    //Variables to store
    ZSVECTOR3 p_translation = ZSVECTOR3(0,0,0);
    ZSVECTOR3 p_scale = ZSVECTOR3(1,1,1);
    ZSVECTOR3 p_rotation = ZSVECTOR3(0,0,0);

    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property
    if(ptr != nullptr && ptr->hasParent){ //if object exist and dependent
        //Get parent's transform property
        TransformProperty* property = static_cast<TransformProperty*>(ptr->parent.updLinkPtr()->getTransformProperty());
        //Calculate parent transform offset
        property->getAbsoluteParentTransform(p_translation, p_scale, p_rotation);

    }

    abs_translation = p_translation + this->translation;
    abs_scale = p_scale * this->scale;
    abs_rotation = this->rotation + p_rotation;

    //Calculate translation matrix
    ZSMATRIX4x4 translation_mat = getTranslationMat(abs_translation);
    //Calculate scale matrix
    ZSMATRIX4x4 scale_mat = getScaleMat(abs_scale);
    //Calculate rotation matrix
    ZSMATRIX4x4 rotation_mat1 = getIdentity();
    getAbsoluteRotationMatrix(rotation_mat1);
    ZSMATRIX4x4 rotation_mat = getRotationMat(abs_rotation);
    //S * R * T
    this->transform_mat = scale_mat * rotation_mat * rotation_mat1 * translation_mat;

}
void Engine::TransformProperty::onValueChanged(){
    if((this->go_link.updLinkPtr()) == nullptr) return;
    //Update transform matrices
    updateMatrix();
    //Get pointer to Base physics component
    PhysicalProperty* phys = static_cast<PhysicalProperty*>(go_link.updLinkPtr()->getPhysicalProperty());

    if(this->go_link.updLinkPtr()->isRigidbody()){
        if(!phys->created) return;
        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3( btScalar(abs_translation.X + phys->transform_offset.X),
                                                    btScalar(abs_translation.Y + phys->transform_offset.Y),
                                                    btScalar(abs_translation.Z + phys->transform_offset.Z)));

        startTransform.setRotation(btQuaternion(abs_rotation.X, abs_rotation.Y, abs_rotation.Z));


        phys->rigidBody->setWorldTransform(startTransform);
        phys->rigidBody->getMotionState()->setWorldTransform(startTransform);

        phys->shape->setLocalScaling(btVector3(btScalar(abs_scale.X),
                                               btScalar(abs_scale.Y),
                                               btScalar(abs_scale.Z)));

    }
}
void Engine::TransformProperty::getAbsoluteParentTransform(ZSVECTOR3& t, ZSVECTOR3& s, ZSVECTOR3& r){
    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property

    t = t + this->translation;
    s = s * this->scale;
    r = r + this->rotation;

    if(ptr->hasParent){
        GameObject* parent_p = ptr->parent.ptr;
        TransformProperty* property = static_cast<TransformProperty*>(parent_p->getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM));
        property->getAbsoluteParentTransform(t, s, r);
    }
}

void Engine::TransformProperty::getAbsoluteRotationMatrix(ZSMATRIX4x4& m){
    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property

    if(ptr == nullptr) return;

    if(ptr->hasParent == true){
        GameObject* parent_p = ptr->parent.ptr;
        TransformProperty* property = static_cast<TransformProperty*>(parent_p->getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM));

        ZSMATRIX4x4 rotation_mat1 = getRotationMat(property->rotation, ptr->getTransformProperty()->translation);
        m = rotation_mat1 * m;
        property->getAbsoluteRotationMatrix(m);
    }
}

void Engine::TransformProperty::setTranslation(ZSVECTOR3 new_translation){
    this->translation = new_translation;
    this->onValueChanged();
}
void Engine::TransformProperty::setScale(ZSVECTOR3 new_scale){
    this->scale = new_scale;
    this->onValueChanged();
}
void Engine::TransformProperty::setRotation(ZSVECTOR3 new_rotation){
    this->rotation = new_rotation;
    this->onValueChanged();
}

void Engine::TransformProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't transform
    //cast pointer and send data
    TransformProperty* _dest = static_cast<TransformProperty*>(dest);
    _dest->translation = translation;
    _dest->scale = scale;
    _dest->rotation = rotation;
    _dest->transform_mat = transform_mat;
}

void Engine::TransformProperty::onPreRender(RenderPipeline* pipeline){
    this->updateMatrix();
    //Send transform matrix to transform buffer
    pipeline->transformBuffer->bind();
    pipeline->transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &transform_mat);
}

Engine::LabelProperty::LabelProperty(){
    this->type = PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL;
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

Engine::ScriptGroupProperty::ScriptGroupProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_SCRIPTGROUP;

    scr_num = 0;
    this->scripts_attached.resize(static_cast<unsigned int>(this->scr_num));
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

Engine::AudioSourceProperty::AudioSourceProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_AUDSOURCE;

    buffer_ptr = nullptr;
    this->resource_relpath = "@none";
    isPlaySheduled = false;

    this->source.source_gain = 1.0f;
    this->source.source_pitch = 1.0f;

    source.Init();
}

void Engine::AudioSourceProperty::onValueChanged(){
    updateAudioPtr();

    this->source.apply_settings();
}

void Engine::AudioSourceProperty::updateAudioPtr(){
    this->buffer_ptr = game_data->resources->getAudioByLabel(resource_relpath);

    if(buffer_ptr == nullptr) return;

    this->source.setAlBuffer(this->buffer_ptr->buffer);
}

void Engine::AudioSourceProperty::onUpdate(float deltaTime){
    if (buffer_ptr == nullptr) return;
    //Poll loading process
    if(buffer_ptr->resource_state == RESOURCE_STATE::STATE_LOADING_PROCESS)
        this->buffer_ptr->load();
    //If loaded, then play
    if(buffer_ptr->resource_state == RESOURCE_STATE::STATE_LOADED && isPlaySheduled){
        audio_start();
        isPlaySheduled = false;
    }

    Engine::TransformProperty* transform = go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
    //if object position changed
    if(transform->translation != this->last_pos){
        //apply new position to openal audio source
        this->source.setPosition(transform->translation);
        this->last_pos = transform->translation;
    }
}

void Engine::AudioSourceProperty::copyTo(Engine::GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't audiosource then exit

    AudioSourceProperty* _dest = static_cast<AudioSourceProperty*>(dest);

    //Do base things
    GameObjectProperty::copyTo(dest);

    _dest->source.Init();
    _dest->resource_relpath = this->resource_relpath;
    _dest->source.source_gain = this->source.source_gain;
    _dest->source.source_pitch = this->source.source_pitch;
    _dest->source.setPosition(this->source.source_pos);
    _dest->buffer_ptr = this->buffer_ptr;
    //_dest->source.setAlBuffer(this->buffer_ptr);
}
void Engine::AudioSourceProperty::setAudioFile(std::string relpath){
    this->resource_relpath = relpath;
    this->updateAudioPtr();
}

void Engine::AudioSourceProperty::setAudioResource(Engine::AudioResource* res) {
    this->buffer_ptr = res;

    if (buffer_ptr == nullptr) return;

    this->resource_relpath = res->resource_label;
    this->source.setAlBuffer(this->buffer_ptr->buffer);
}

void Engine::AudioSourceProperty::audio_start(){
    if(buffer_ptr->resource_state == RESOURCE_STATE::STATE_LOADED){
        //Update source buffer
        if(buffer_ptr == nullptr) return;
        this->source.setAlBuffer(this->buffer_ptr->buffer);
        //play source
        this->source.play();

    }else {
        this->buffer_ptr->load();
        isPlaySheduled = true;
    }
}

void Engine::AudioSourceProperty::audio_stop(){
    this->source.stop();
}

void Engine::AudioSourceProperty::audio_pause(){
    this->source.pause();
}

float Engine::AudioSourceProperty::getGain(){
    return source.source_gain;
}
float Engine::AudioSourceProperty::getPitch(){
    return source.source_pitch;
}
void Engine::AudioSourceProperty::setGain(float gain){
    source.source_gain = gain;
    source.apply_settings();
}
void Engine::AudioSourceProperty::setPitch(float pitch){
    source.source_pitch = pitch;
    source.apply_settings();
}

void Engine::AudioSourceProperty::onObjectDeleted(){
    this->source.stop(); //Stop at first
    this->source.Destroy();
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


void Engine::ColliderProperty::onObjectDeleted(){
    if(created)
        this->go_link.world_ptr->physical_world->removeRidigbodyFromWorld(this->rigidBody);
} //unregister in world

void Engine::ColliderProperty::onUpdate(float deltaTime){
    if(!created)
        init();
}

void Engine::ColliderProperty::copyTo(Engine::GameObjectProperty* dest){
    if(dest->type != PROPERTY_TYPE::GO_PROPERTY_TYPE_COLLIDER) return;

    PhysicalProperty::copyTo(dest);

    ColliderProperty* coll_prop = static_cast<ColliderProperty*>(dest);
}

Engine::TransformProperty* Engine::ColliderProperty::getTransformProperty(){
    return this->go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
}

Engine::ColliderProperty::ColliderProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_COLLIDER;

    coll_type = COLLIDER_TYPE::COLLIDER_TYPE_CUBE;
    created = false;
    mass = 0.0f; //collider is static
}

void Engine::RigidbodyProperty::onUpdate(float deltaTime){
    if(!created){
        //if uninitialized
        init();

        this->rigidBody->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
        this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));
    }
    else{
        PhysicalProperty::onUpdate(deltaTime);

        Engine::TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
        btVector3 current_pos = rigidBody->getCenterOfMassPosition();
        btQuaternion current_rot = rigidBody->getWorldTransform().getRotation();
        //get current position
        float curX = current_pos.getX();
        float curY = current_pos.getY();
        float curZ = current_pos.getZ();

        float rotX = 0, rotY = 0, rotZ = 0;
        //Convert quaternion to euler
        current_rot.getEulerZYX(rotX, rotY, rotZ);
        //Convert radians to degrees
        rotX = rotX / ZS_PI * 180.0f;
        rotY = rotY / ZS_PI * 180.0f;
        rotZ = rotZ / ZS_PI * 180.0f;

        if(transform->translation != ZSVECTOR3(curX, curY, curZ))
            transform->translation = ZSVECTOR3(curX, curY, curZ);
        if(transform->rotation != ZSVECTOR3(rotX, rotY, -rotZ))
            transform->rotation = ZSVECTOR3(rotX, rotY, -rotZ);
    }
}

void Engine::RigidbodyProperty::setLinearVelocity(ZSVECTOR3 lvel){
    if(!created) return;
    this->linearVel = lvel;
    this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));
}

void Engine::RigidbodyProperty::onValueChanged(){
    if(!created) return;
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        shape->calculateLocalInertia(mass, localInertia);
    //SET mass to bullet rigidbody
    this->rigidBody->setMassProps(mass, localInertia);
    this->rigidBody->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
    this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));

    delete shape;
    updateCollisionShape();
    this->rigidBody->setCollisionShape(shape);
}


void Engine::RigidbodyProperty::copyTo(Engine::GameObjectProperty* dest){
    if(dest->type != PROPERTY_TYPE::GO_PROPERTY_TYPE_RIGIDBODY) return;

    //Do base things
    PhysicalProperty::copyTo(dest);

    RigidbodyProperty* rigi_prop = static_cast<RigidbodyProperty*>(dest);
    rigi_prop->gravity = this->gravity;
    rigi_prop->linearVel = this->linearVel;
    rigi_prop->angularVel = this->angularVel;
}

Engine::RigidbodyProperty::RigidbodyProperty(){

    mass = 1.0f;
    created = false;
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_RIGIDBODY;
    coll_type = COLLIDER_TYPE::COLLIDER_TYPE_CUBE;

    gravity = ZSVECTOR3(0.f, -10.f, 0.f);
    linearVel = ZSVECTOR3(0.f, 0.f, 0.f);
}

Engine::CharacterControllerProperty::CharacterControllerProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_CHARACTER_CONTROLLER;
    created = false;

    gravity = ZSVECTOR3(0.f, -10.f, 0.f);
    linearVel = ZSVECTOR3(0.f, -10.f, 0.f);

    mass = 10;
}

Engine::TriggerProperty::TriggerProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_TRIGGER;
    coll_type = COLLIDER_TYPE::COLLIDER_TYPE_CUBE;
    created = false;
    m_ghost = nullptr;
}

void Engine::CharacterControllerProperty::setLinearVelocity(ZSVECTOR3 lvel){
    if(!created) return;
    this->linearVel = lvel;
    this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));
    rigidBody->activate();
}

void Engine::CharacterControllerProperty::copyTo(Engine::GameObjectProperty* dest){
    if(dest->type != PROPERTY_TYPE::GO_PROPERTY_TYPE_CHARACTER_CONTROLLER) return;

    //Do base things
    PhysicalProperty::copyTo(dest);

    CharacterControllerProperty* chara = static_cast<CharacterControllerProperty*>(dest);

}
void Engine::CharacterControllerProperty::onUpdate(float deltaTime){
    if(!created){
        Engine::TransformProperty* transform = go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();

        ZSVECTOR3 scale = transform->abs_scale;
        ZSVECTOR3 pos = transform->abs_translation;
        //if Custom Physical Size is checked
        if(isCustomPhysicalSize){
            scale = cust_size;
        }

        //if uninitialized
        this->shape = new btCapsuleShape(scale.X, scale.Y);

        btVector3 localInertia(0, 0, 0);

        //shape->calculateLocalInertia(mass, localInertia);
        //Declare start transform
        btTransform startTransform;
        startTransform.setIdentity();
        //Set start transform
        startTransform.setOrigin(btVector3( btScalar(pos.X), btScalar(pos.Y), btScalar(pos.Z)));
        //Set start rotation
        startTransform.setRotation(btQuaternion(transform->abs_rotation.X, transform->abs_rotation.Y, transform->abs_rotation.Z));

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

        btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
        //Create Object rigidbody instance
        rigidBody = new btRigidBody(cInfo);

        rigidBody->setUserIndex(this->go_link.updLinkPtr()->array_index);
        //add rigidbody to world
        go_link.world_ptr->physical_world->addRidigbodyToWorld(rigidBody);
        //Set zero values
        this->rigidBody->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
        this->rigidBody->setLinearVelocity(btVector3(linearVel.X, linearVel.Y, linearVel.Z));

         created = true;
    }else{
        Engine::TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
        btVector3 current_pos = rigidBody->getCenterOfMassPosition();

        //get current position
        float curX = current_pos.getX();
        float curY = current_pos.getY();
        float curZ = current_pos.getZ();

        if(transform->translation != ZSVECTOR3(curX, curY, curZ))
            transform->translation = ZSVECTOR3(curX, curY, curZ);

    }
}

void Engine::TriggerProperty::onUpdate(float deltaTime) {
    if (!created) {
        initGhost();
    }
    else {
       //Get number of objects, that currently collide
        unsigned int overlaping = m_ghost->getNumOverlappingObjects();
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

Engine::SkyboxProperty::SkyboxProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_SKYBOX;
}

Engine::ShadowCasterProperty::ShadowCasterProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_SHADOWCASTER;

    initialized = false;

    TextureWidth = 2048;
    TextureHeight = 2048;

    shadow_bias = 0.005f;

    nearPlane = 1.0f;
    farPlane = 75.0f;

    projection_viewport = 20;
}

bool Engine::ShadowCasterProperty::isRenderAvailable(){
    if(!this->active)
        return false;
    if(!this->initialized)
        init();
    if(this->go_link.updLinkPtr() == nullptr)
        return false;
    //Get lightsource property of object
    Engine::LightsourceProperty* light = this->go_link.updLinkPtr()->getPropertyPtr<Engine::LightsourceProperty>();

    if(light == nullptr)
        return false;
        //if light gameobject disabled
    if(!light->isActive())
            return false;
    return true;
}

void Engine::ShadowCasterProperty::copyTo(Engine::GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't script group

    //Do base things
    GameObjectProperty::copyTo(dest);

    ShadowCasterProperty* _dest = static_cast<ShadowCasterProperty*>(dest);
    _dest->farPlane = this->farPlane;
    _dest->nearPlane = this->nearPlane;
    _dest->shadow_bias = this->shadow_bias;
    _dest->TextureWidth = this->TextureWidth;
    _dest->TextureHeight = this->TextureHeight;
    _dest->projection_viewport = this->projection_viewport;
}

void Engine::ShadowCasterProperty::setTextureSize(){
    glDeleteTextures(1, &this->shadowDepthTexture);
    glDeleteFramebuffers(1, &shadowBuffer);

    init();
}

void Engine::ShadowCasterProperty::setTexture(){
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, this->shadowDepthTexture);
}

void Engine::ShadowCasterProperty::onValueChanged(){
    //Update shadowmap texture
    setTextureSize();
}
