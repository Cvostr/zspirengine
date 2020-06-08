#include "../../../headers/world/go_properties.h"

Engine::TerrainProperty::TerrainProperty(){
    type = GO_PROPERTY_TYPE_TERRAIN;

    this->Width = 500;
    this->Length = 500;
    this->MaxHeight = 500;
    
    castShadows = true;
    textures_size = 0;
    grassType_size = 0;

    this->range = 15;
    this->editHeight = 5;
    this->textureid = 1;
    this->vegetableid = 1;

    edit_mode = 1;

    rigidBody = nullptr;
}

TerrainData* Engine::TerrainProperty::getTerrainData(){
    return &data;
}

void Engine::TerrainProperty::copyTo(Engine::GameObjectProperty* dest){
    if(dest->type != this->type) return; //if it isn't script group

    //Do base things
    GameObjectProperty::copyTo(dest);

    Engine::TerrainProperty* _dest = static_cast<Engine::TerrainProperty*>(dest);
    _dest->Width = this->Width;
    _dest->Length = this->Length;
    _dest->MaxHeight = this->MaxHeight;
    _dest->file_label = this->file_label;
    _dest->castShadows = this->castShadows;
    _dest->textures_size = this->textures_size;
    _dest->grassType_size = this->grassType_size;
    //Copying terrain data
    data.copyTo(&_dest->data);
    //Copy textures data
    for(unsigned int t_i = 0; t_i < this->textures.size(); t_i ++)
        _dest->textures.push_back(textures[t_i]);
}

void Engine::TerrainProperty::onUpdate(float deltaTime) {
    if (data.shape == nullptr || data.hasPhysicShapeChanged && data.isCreated()) {
        data.initPhysics();
        Engine::TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();

        //Declare start transform
        btTransform startTransform;
        startTransform.setIdentity();
        //Set start transform
        startTransform.setOrigin(btVector3(btScalar(transform->abs_translation.X), btScalar(transform->abs_translation.Y),
            btScalar(transform->abs_translation.Z)));


        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

        btRigidBody::btRigidBodyConstructionInfo cInfo(0, myMotionState, data.shape, btVector3(0, 0, 0));

        if (rigidBody != nullptr) {
            world_ptr->physical_world->removeRidigbodyFromWorld(rigidBody);
            delete rigidBody;
        }
        rigidBody = new btRigidBody(cInfo);

        rigidBody->setUserIndex(this->go_link.updLinkPtr()->array_index);
        //add rigidbody to world
        go_link.world_ptr->physical_world->addRidigbodyToWorld(rigidBody);

        data.hasPhysicShapeChanged = false;
    }
}