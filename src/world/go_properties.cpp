#include "../../headers/world/go_properties.h"
#include "../../headers/world/tile_properties.h"

Engine::GameObjectProperty::GameObjectProperty(){
    type = GO_PROPERTY_TYPE_NONE;
    active = true;
}
void Engine::GameObjectProperty::setActive(bool active){
    this->active = active;
}
void Engine::GameObjectProperty::copyTo(GameObjectProperty* dest){

}
void Engine::GameObjectProperty::onUpdate(float deltaTime){

}
void Engine::GameObjectProperty::onPreRender(RenderPipeline* pipeline){

}
void Engine::GameObjectProperty::onRender(RenderPipeline* pipeline){

}
void Engine::GameObjectProperty::onObjectDeleted(){

}
Engine::GameObjectProperty::~GameObjectProperty(){

}

Engine::GameObjectProperty* Engine::GameObject::allocProperty(int type){
    GameObjectProperty* _ptr = nullptr;
    switch (type) {
        case GO_PROPERTY_TYPE_TRANSFORM:{ //If type is transfrom
            _ptr = static_cast<GameObjectProperty*>(new TransformProperty); //Allocation of transform in heap
            break;
        }
        case GO_PROPERTY_TYPE_LABEL:{
            LabelProperty* ptr = new LabelProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_MESH:{
            MeshProperty* ptr = new MeshProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }

        case GO_PROPERTY_TYPE_LIGHTSOURCE:{
            LightsourceProperty* ptr = new LightsourceProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }

        case GO_PROPERTY_TYPE_SCRIPTGROUP:{
            ScriptGroupProperty* ptr = new ScriptGroupProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }

        case GO_PROPERTY_TYPE_AUDSOURCE:{
            AudioSourceProperty* ptr = new AudioSourceProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }

        case GO_PROPERTY_TYPE_MATERIAL:{
            MaterialProperty* ptr = new MaterialProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        /*
        case GO_PROPERTY_TYPE_COLLIDER:{
            ColliderProperty* ptr = new ColliderProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        */
        case GO_PROPERTY_TYPE_TILE_GROUP:{
            TileGroupProperty* ptr = new TileGroupProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
        case GO_PROPERTY_TYPE_TILE:{
            TileProperty* ptr = new TileProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }
    }
    return _ptr;
}

Engine::TransformProperty::TransformProperty(){
    this->type = GO_PROPERTY_TYPE_TRANSFORM;
}

void Engine::TransformProperty::updateMat(){
    //Variables to store
    ZSVECTOR3 p_translation = ZSVECTOR3(0,0,0);
    ZSVECTOR3 p_scale = ZSVECTOR3(1,1,1);
    ZSVECTOR3 p_rotation = ZSVECTOR3(0,0,0);

    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property
    if(ptr != nullptr && ptr->hasParent){ //if object exist and dependent
        //Get parent's transform property
        TransformProperty* property = static_cast<TransformProperty*>(ptr->parent.updLinkPtr()->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
        //Calculate parent transform offset
        property->getAbsoluteParentTransform(p_translation, p_scale, p_rotation);

    }

    //Calculate translation matrix
    ZSMATRIX4x4 translation_mat = getTranslationMat(p_translation + this->translation);
    //Calculate scale matrix
    ZSMATRIX4x4 scale_mat = getScaleMat(p_scale + this->scale);
    //Calculate rotation matrix
    ZSMATRIX4x4 rotation_mat1 = getIdentity();
    getAbsoluteRotationMatrix(rotation_mat1);

    abs_translation = p_translation + this->translation;
    abs_scale = p_scale + this->scale;
    abs_rotation = this->rotation + p_rotation;

    ZSMATRIX4x4 rotation_mat = getRotationMat(this->rotation);
    //S * R * T
    this->transform_mat = scale_mat * rotation_mat * rotation_mat1 * translation_mat;
}

void Engine::TransformProperty::getAbsoluteParentTransform(ZSVECTOR3& t, ZSVECTOR3& s, ZSVECTOR3& r){
    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property

    t = t + this->translation;
    s = s * this->scale;
    r = r + this->rotation;

    if(ptr->hasParent){
        GameObject* parent_p = ptr->parent.ptr;
        TransformProperty* property = static_cast<TransformProperty*>(parent_p->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
        property->getAbsoluteParentTransform(t, s, r);
    }
}

void Engine::TransformProperty::getAbsoluteRotationMatrix(ZSMATRIX4x4& m){
    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property

    if(ptr == nullptr) return;

    if(ptr->hasParent == true){
        GameObject* parent_p = ptr->parent.ptr;
        TransformProperty* property = static_cast<TransformProperty*>(parent_p->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));

        ZSMATRIX4x4 rotation_mat1 = getRotationMat(property->rotation, ptr->getTransformProperty()->translation);
        m = rotation_mat1 * m;
        property->getAbsoluteRotationMatrix(m);
    }
}

void Engine::TransformProperty::setTranslation(ZSVECTOR3 new_translation){
    this->translation = new_translation;
    this->updateMat();
}
void Engine::TransformProperty::setScale(ZSVECTOR3 new_scale){
    this->scale = new_scale;
    this->updateMat();
}
void Engine::TransformProperty::setRotation(ZSVECTOR3 new_rotation){
    this->rotation = new_rotation;
    this->updateMat();
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
    this->updateMat();
}

Engine::LabelProperty::LabelProperty(){
    this->type = GO_PROPERTY_TYPE_LABEL;
}


Engine::MeshProperty::MeshProperty(){
    this->type = GO_PROPERTY_TYPE_MESH;
}
void Engine::MeshProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't transform

    MeshProperty* _dest = static_cast<MeshProperty*>(dest);
    _dest->resource_relpath = resource_relpath;
    _dest->mesh_ptr = mesh_ptr;
}
void Engine::MeshProperty::updateMeshPtr(){
    this->mesh_ptr = go_link.world_ptr->getResourceManager()->getMeshByLabel(this->resource_relpath);
}

Engine::ScriptGroupProperty::ScriptGroupProperty(){
    this->type = GO_PROPERTY_TYPE_SCRIPTGROUP;
}
void Engine::ScriptGroupProperty::wakeUp(){   //on scene startup
    for(unsigned int script_i = 0; script_i < static_cast<unsigned int>(scr_num); script_i ++){

    }
}
void Engine::ScriptGroupProperty::onUpdate(float deltaTime){  //calls update in scripts
    for(unsigned int script_i = 0; script_i < this->scripts_attached.size(); script_i ++){
        ObjectScript* script_ptr = &this->scripts_attached[script_i]; //Obtain pointer to script

        if(!script_ptr->created){
            this->scripts_attached[script_i]._InitScript();
            this->scripts_attached[script_i]._callStart(go_link.updLinkPtr(), go_link.world_ptr);
        }
        script_ptr->_callDraw(deltaTime); //Run onDraw() function in script
    }
}

Engine::LightsourceProperty::LightsourceProperty(){
    this->type = GO_PROPERTY_TYPE_LIGHTSOURCE;
}
void Engine::LightsourceProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't transform

    LightsourceProperty* _dest = static_cast<LightsourceProperty*>(dest);
    _dest->color = color;
    _dest->intensity = intensity;
    _dest->range = range;
    _dest->light_type = light_type;
}

void Engine::LightsourceProperty::updTransformPtr(){
    if(transform == nullptr){
        transform = this->go_link.updLinkPtr()->getTransformProperty();
    }
}

void Engine::LightsourceProperty::onPreRender(RenderPipeline* pipeline){
    updTransformPtr();

    this->last_pos = transform->abs_translation; //Store old value
    this->last_rot = transform->abs_rotation;
    this->direction = _getDirection(last_rot.X, last_rot.Y, last_rot.Z);

    pipeline->addLight(static_cast<void*>(this)); //put light pointer to vector
}

Engine::AudioSourceProperty::AudioSourceProperty(){
    this->type = GO_PROPERTY_TYPE_AUDSOURCE;

    this->source.source_gain = 1.0f;
    this->source.source_pitch = 1.0f;

    source.Init();
}
void Engine::AudioSourceProperty::onUpdate(float deltaTime){

}
void Engine::AudioSourceProperty::onObjectDeleted(){

}
void Engine::AudioSourceProperty::updateAudioPtr(){
    //if(buffer_ptr->resource_state == STATE_LOADED)
       this->buffer_ptr = go_link.world_ptr->getResourceManager()->getAudioByLabel(this->resource_relpath);
    //else {
        //this->buffer_ptr->load();
    //}
}

void Engine::AudioSourceProperty::audio_start(){
    this->source.play();
}
void Engine::AudioSourceProperty::audio_stop(){
    this->source.stop();
}

void Engine::AudioSourceProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't transform

    AudioSourceProperty* _dest = static_cast<AudioSourceProperty*>(dest);

    _dest->source.Init();
    _dest->resource_relpath = this->resource_relpath;
    _dest->source.source_gain = this->source.source_gain;
    _dest->source.source_pitch = this->source.source_pitch;
    _dest->source.setPosition(this->source.source_pos);
    _dest->buffer_ptr = this->buffer_ptr;
}

Engine::MaterialProperty::MaterialProperty(){
    type = GO_PROPERTY_TYPE_MATERIAL;
}

void Engine::MaterialProperty::copyTo(GameObjectProperty* dest){

}
void Engine::MaterialProperty::onRender(RenderPipeline* pipeline){

}
