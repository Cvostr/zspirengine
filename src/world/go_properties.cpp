#include "../../headers/world/go_properties.h"
#include "../../headers/world/tile_properties.h"

extern ZSGAME_DATA* game_data;

Engine::GameObjectProperty::GameObjectProperty(){
    type = GO_PROPERTY_TYPE_NONE;
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
        case GO_PROPERTY_TYPE_NODE:{
            NodeProperty* ptr = new NodeProperty;
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

        case GO_PROPERTY_TYPE_COLLIDER:{
            ColliderProperty* ptr = new ColliderProperty;
            _ptr = static_cast<GameObjectProperty*>(ptr);
            break;
        }

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
        case GO_PROPERTY_TYPE_ANIMATION:{
            AnimationProperty* ptr = new AnimationProperty;
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
    mesh_ptr = nullptr;
}
void Engine::MeshProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't transform

    MeshProperty* _dest = static_cast<MeshProperty*>(dest);
    _dest->resource_relpath = resource_relpath;
    _dest->mesh_ptr = mesh_ptr;
}
void Engine::MeshProperty::updateMeshPtr(){
    this->mesh_ptr = game_data->resources->getMeshByLabel(this->resource_relpath);
}

Engine::ScriptGroupProperty::ScriptGroupProperty(){
    this->type = GO_PROPERTY_TYPE_SCRIPTGROUP;
    scr_num = 0;
}
void Engine::ScriptGroupProperty::onUpdate(float deltaTime){  //calls update in scripts
    for(unsigned int script_i = 0; script_i < this->scripts_attached.size(); script_i ++){
        ObjectScript* script_ptr = &this->scripts_attached[script_i]; //Obtain pointer to script
        //if script isn't created, then create it!
        if(!script_ptr->created){
            this->scripts_attached[script_i]._InitScript();
            this->scripts_attached[script_i]._callStart(go_link.updLinkPtr(), go_link.world_ptr);
        }
        script_ptr->_callDraw(deltaTime); //Run onDraw() function in script
    }
}

Engine::ObjectScript* Engine::ScriptGroupProperty::getScriptByName(std::string name){
    for(unsigned int script_i = 0; script_i < static_cast<unsigned int>(scr_num); script_i ++){
        if(!name.compare(scripts_attached[script_i].name))
            return &scripts_attached[script_i];
    }
    return nullptr;
}

Engine::LightsourceProperty::LightsourceProperty(){
    this->type = GO_PROPERTY_TYPE_LIGHTSOURCE;
    transform = nullptr;
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

    isPlaySheduled = false;
    buffer_ptr = nullptr;

    source.Init();
}
void Engine::AudioSourceProperty::onUpdate(float deltaTime){
    if(buffer_ptr->resource_state == STATE_LOADING_PROCESS)
        this->buffer_ptr->load();
    if(buffer_ptr->resource_state == STATE_LOADED && isPlaySheduled){
        audio_start();
        isPlaySheduled = false;
    }


    TransformProperty* transform = go_link.updLinkPtr()->getTransformProperty();

    if(transform->translation != this->last_pos){
        this->source.setPosition(transform->translation);
        this->last_pos = transform->translation;
    }
}
void Engine::AudioSourceProperty::onObjectDeleted(){

}
void Engine::AudioSourceProperty::updateAudioPtr(){
    this->buffer_ptr = game_data->resources->getAudioByLabel(this->resource_relpath);
}

void Engine::AudioSourceProperty::audio_start(){
    if(buffer_ptr == nullptr) return;
    if(buffer_ptr->resource_state == STATE_LOADED){
        //Update source buffer
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
    return this->source.source_gain;
}
float Engine::AudioSourceProperty::getPitch(){
    return this->source.source_pitch;
}
void Engine::AudioSourceProperty::setGain(float gain){
    source.source_gain = gain;
    source.apply_settings();
}
void Engine::AudioSourceProperty::setPitch(float pitch){
    source.source_pitch = pitch;
    source.apply_settings();
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

void Engine::MaterialProperty::onValueChanged(){
    this->material_ptr = game_data->resources->getMaterialByLabel(this->material_path);
}

Engine::NodeProperty::NodeProperty(){
    type = GO_PROPERTY_TYPE_NODE;

    scale = ZSVECTOR3(1.f, 1.f, 1.f);
    translation = ZSVECTOR3(0.f, 0.f, 0.f);
    rotation = ZSQUATERNION(0.f, 0.f, 0.f, 0.f);
}
void Engine::NodeProperty::onPreRender(RenderPipeline* pipeline){

}
void Engine::NodeProperty::copyTo(GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't Node property

    //Do base things
    GameObjectProperty::copyTo(dest);
    NodeProperty* _dest = static_cast<NodeProperty*>(dest);

    _dest->transform_mat = this->transform_mat;
    _dest->node_label = this->node_label;
}

Engine::ColliderProperty::ColliderProperty(){
    type = GO_PROPERTY_TYPE_COLLIDER;
}
void Engine::ColliderProperty::onAddToObject(){

}
void Engine::ColliderProperty::onObjectDeleted(){

} //unregister in world
void Engine::ColliderProperty::copyTo(GameObjectProperty* dest){

}

Engine::GameObject* Engine::GameObject::getChildObjectWithNodeLabel(std::string label){
    //This function works recursively
    //Iterate over all children in current object
    for (unsigned int i = 0; i < this->children.size(); i ++) {
        GameObjectLink* l = &this->children[i];
        Engine::NodeProperty* node_p = l->updLinkPtr()->getPropertyPtr<Engine::NodeProperty>();
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

Engine::AnimationProperty::AnimationProperty(){
    type = GO_PROPERTY_TYPE_ANIMATION;
    this->anim_label = "@none";
    anim_prop_ptr = nullptr;
    Playing = false;
    start_sec = 0;
}

void Engine::AnimationProperty::play(){
    //if user specified animation, then play it!
    if(anim_prop_ptr != nullptr){
        //Send animation to multithreaded loading, if need
        anim_prop_ptr->load();

        start_sec = (static_cast<double>(SDL_GetTicks()) / 1000);
        Playing = true;
    }
}
void Engine::AnimationProperty::stop(){
    Playing = false;
}

void Engine::AnimationProperty::onPreRender(RenderPipeline* pipeline){
    GameObject* obj = go_link.updLinkPtr();

    Engine::Animation* anim_prop_ptr = nullptr;
    //Try to get loading result
    if(this->anim_prop_ptr != nullptr && Playing && this->anim_prop_ptr->resource_state == STATE_LOADING_PROCESS)
        this->anim_prop_ptr->load();

    if(this->anim_prop_ptr != nullptr && Playing && this->anim_prop_ptr->resource_state == STATE_LOADED){
        if(this->anim_prop_ptr != nullptr)
                anim_prop_ptr = this->anim_prop_ptr->animation_ptr;

        //Calcualte current Time
        double curTime = (static_cast<double>(SDL_GetTicks()) / 1000) - this->start_sec;
        //Time in animation ticks
        double Ticks = anim_prop_ptr->TPS * curTime;
        //Calculate current animation time
        double animTime = fmod(Ticks, anim_prop_ptr->duration);

        for(unsigned int channels_i = 0; channels_i < anim_prop_ptr->NumChannels; channels_i ++){
            Engine::AnimationChannel* ch = &anim_prop_ptr->channels[channels_i];
            GameObject* node = obj->getChildObjectWithNodeLabel(ch->bone_name);
            NodeProperty* prop = node->getPropertyPtr<NodeProperty>();

            prop->translation = ch->getPostitionInterpolated(animTime);
            prop->scale = ch->getScaleInterpolated(animTime);
            prop->rotation = ch->getRotationInterpolated(animTime);
        }
    }
    ZSMATRIX4x4 identity_matrix = getIdentity();
    updateNodeTransform(obj, identity_matrix);
}

void Engine::AnimationProperty::updateNodeTransform(GameObject* obj, ZSMATRIX4x4 parent){

    if(!obj) return;
    NodeProperty* prop = obj->getPropertyPtr<NodeProperty>();
    if(!prop) return;
    //Assign base node transform
    prop->abs = prop->transform_mat;

    if(this->anim_prop_ptr != nullptr && Playing && this->anim_prop_ptr->resource_state == STATE_LOADED){
        Engine::AnimationChannel* cha = this->anim_prop_ptr->animation_ptr->getChannelByNodeName(prop->node_label);
        if(cha){
            ZSMATRIX4x4 transl = transpose(getTranslationMat(prop->translation));
            ZSMATRIX4x4 _sca = transpose(getScaleMat(prop->scale));
            ZSMATRIX4x4 rot = (getRotationMat(prop->rotation));
            //Multiply all matrices
            prop->abs = transl * rot * _sca;
        }
    }
    //Apply parent transform
    prop->abs = parent * prop->abs;

    for(unsigned int i = 0; i < obj->children.size(); i ++){
        updateNodeTransform(obj->children[i].updLinkPtr(), prop->abs);
    }

}

void Engine::AnimationProperty::copyTo(GameObjectProperty *dest){
    if(dest->type != this->type) return; //if it isn't animation

    //Do base things
    GameObjectProperty::copyTo(dest);
    AnimationProperty* _dest = static_cast<AnimationProperty*>(dest);

    _dest->anim_label = this->anim_label;
    _dest->anim_prop_ptr = this->anim_prop_ptr;
}

void Engine::AnimationProperty::onValueChanged(){
    updateAnimationPtr();
}

void Engine::AnimationProperty::setAnimation(std::string anim){
    this->anim_label = anim;
    updateAnimationPtr();
}

void Engine::AnimationProperty::updateAnimationPtr(){
    anim_prop_ptr = game_data->resources->getAnimationByLabel(this->anim_label);
}
