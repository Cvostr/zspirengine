#include "../../../headers/world/go_properties.h"

extern ZSGAME_DATA* game_data;

Engine::TerrainProperty::TerrainProperty(){
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_TERRAIN;

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

Engine::TerrainProperty::~TerrainProperty() {
    textures.clear();
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
    //Check, if physics stuff created
    if (data.shape == nullptr || data.hasPhysicShapeChanged && data.isCreated()) {
        //Init Physic configuration
        data.initPhysics();
        
        //Declare start transform
        Engine::TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
        btTransform startTransform;
        startTransform.setIdentity();
        //Set start transform
        startTransform.setOrigin(btVector3(btScalar(transform->abs_translation.X), btScalar(transform->abs_translation.Y),
            btScalar(transform->abs_translation.Z)));

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

        btRigidBody::btRigidBodyConstructionInfo cInfo(0, myMotionState, data.shape, btVector3(0, 0, 0));
        //if regidbody is already created
        if (rigidBody != nullptr) {
            //Remove RigidBody from world
            world_ptr->physical_world->removeRidigbodyFromWorld(rigidBody);
            //Remove rigidBody
            delete rigidBody;
        }
        //Allocate rigidbody
        rigidBody = new btRigidBody(cInfo);

        rigidBody->setUserIndex(this->go_link.updLinkPtr()->array_index);
        //add rigidbody to world
        go_link.world_ptr->physical_world->addRidigbodyToWorld(rigidBody);
        //Physics objects are created, no need to recreate them in further
        data.hasPhysicShapeChanged = false;
    }
}

void Engine::TerrainProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;

    while (data[offset] != ' ' && data[offset] != '\n') {
        file_label += data[offset];
        offset++;
    }

    offset++;
    //read dimensions
    memcpy(&Width, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&Length, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&MaxHeight, data + offset, sizeof(float));
    offset += sizeof(float);

    memcpy(&castShadows, data + offset, sizeof(bool));
    offset += sizeof(bool);
    memcpy(&textures_size, data + offset, sizeof(int));
    offset += sizeof(int);
    memcpy(&grassType_size, data + offset, sizeof(int));
    offset += sizeof(int);

    ZsResource* terrain_res = game_data->resources->getResource<Engine::ZsResource>(file_label);
    //check, if terrain resource found
    if (terrain_res) {
        terrain_res->request = new Engine::Loader::LoadRequest;
        terrain_res->request->offset = terrain_res->offset;
        terrain_res->request->size = terrain_res->size;
        terrain_res->request->file_path = terrain_res->blob_path;
        loadImmideately(terrain_res->request);
        //Load terrain from readed binary data
        bool result = getTerrainData()->loadFromMemory((const char*)terrain_res->request->data);
        if (result) //if loading sucessstd::cout << "Terrain : Probably, missing terrain file" << file_path;
            getTerrainData()->generateGLMesh();
    }
    offset++;

    //Read textures relative pathes
    for (int texture_i = 0; texture_i < textures_size; texture_i++) {
        HeightmapTexturePair texture_pair;
        //Read texture pair
        texture_pair.diffuse_relpath.clear();
        while (data[offset] != ' ' && data[offset] != '\n') {
            texture_pair.diffuse_relpath += data[offset];
            offset++;
        }
        offset++;
        //Read normal pair
        texture_pair.normal_relpath.clear();    
        while (data[offset] != ' ' && data[offset] != '\n') {
            texture_pair.normal_relpath += data[offset];
            offset++;
        }
        offset++;
        //Push texture to array
        textures.push_back(texture_pair);
    }

    for (int grass_i = 0; grass_i < grassType_size; grass_i++) {
        HeightmapGrass grass;
        //Read grass diffuse texture
        grass.diffuse_relpath.clear();
        while (data[offset] != ' ' && data[offset] != '\n') {
            grass.diffuse_relpath += data[offset];
            offset++;
        }
        //Write grass size
        offset++;
        memcpy(&grass.scale.X, data + offset, sizeof(float));
        offset += sizeof(float);
        memcpy(&grass.scale.Y, data + offset, sizeof(float));
        offset += sizeof(float);
        //Push Grass to array
        getTerrainData()->grass.push_back(grass);
    }
    //Fill grass buffers with transforms
    getTerrainData()->updateGrassBuffers();

    onValueChanged();
}