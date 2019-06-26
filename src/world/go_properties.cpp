#include "../../headers/world/go_properties.h"
#include "../../headers/world/tile_properties.h"

Engine::GameObjectProperty::GameObjectProperty(){
    type = GO_PROPERTY_TYPE_NONE;
    active = true;
}

void Engine::GameObjectProperty::copyTo(GameObjectProperty* dest){

}
void Engine::GameObjectProperty::onUpdate(float deltaTime){

}
void Engine::GameObjectProperty::onPreRender(RenderPipeline* pipeline){

}
void Engine::GameObjectProperty::onRender(RenderPipeline* pipeline){

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
        /*
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

void Engine::TransformProperty::onPreRender(RenderPipeline* pipeline){

}

Engine::LabelProperty::LabelProperty(){
    this->type = GO_PROPERTY_TYPE_LABEL;
}


Engine::MeshProperty::MeshProperty(){
    this->type = GO_PROPERTY_TYPE_MESH;
}

void Engine::MeshProperty::updateMeshPtr(){
    this->mesh_ptr = go_link.world_ptr->getResourceManager()->getMeshByLabel(this->resource_relpath);
}

Engine::ScriptGroupProperty::ScriptGroupProperty(){
    this->type = GO_PROPERTY_TYPE_SCRIPTGROUP;
}
void Engine::ScriptGroupProperty::wakeUp(){   //on scene startup

}
void Engine::ScriptGroupProperty::onUpdate(float deltaTime){  //calls update in scripts

}

Engine::LightsourceProperty::LightsourceProperty(){
    this->type = GO_PROPERTY_TYPE_LIGHTSOURCE;
}
void Engine::LightsourceProperty::onPreRender(RenderPipeline* pipeline){

}

Engine::AudioSourceProperty::AudioSourceProperty(){
    this->type = GO_PROPERTY_TYPE_AUDSOURCE;
}
void Engine::AudioSourceProperty::onUpdate(float deltaTime){

}
