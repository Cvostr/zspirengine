#include "../../../headers/world/go_properties.h"
#include "../../../headers/world/tile_properties.h"

extern ZSpireEngine* engine_ptr;
extern ZSGAME_DATA* game_data;

void Engine::GameObject::loadProperty(std::ifstream* world_stream){
  /*  PROPERTY_TYPE type;
    world_stream->seekg(1, std::ofstream::cur); //Skip space
    world_stream->read(reinterpret_cast<char*>(&type), sizeof(int));
    //Spawn new property with readed type
    addProperty(type);
    GameObjectProperty* prop_ptr = getPropertyPtrByType(type); //get created property
    world_stream->read(reinterpret_cast<char*>(&prop_ptr->active), sizeof(bool));
    //since more than 1 properties same type can't be on one gameobject
    switch(type){
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL :{
            std::string label;
            *world_stream >> label;
            LabelProperty* lptr = static_cast<LabelProperty*>(prop_ptr);
            this->label_ptr = &lptr->label; //Making GameObjects's pointer to string in label property
            lptr->label = label; //Write loaded string
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM :{
            world_stream->seekg(1, std::ofstream::cur); //Skip space
            TransformProperty* t_ptr = static_cast<TransformProperty*>(prop_ptr);
            world_stream->read(reinterpret_cast<char*>(&t_ptr->translation.X), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->translation.Y), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->translation.Z), sizeof(float));

            world_stream->read(reinterpret_cast<char*>(&t_ptr->scale.X), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->scale.Y), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->scale.Z), sizeof(float));

            world_stream->read(reinterpret_cast<char*>(&t_ptr->rotation.X), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->rotation.Y), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&t_ptr->rotation.Z), sizeof(float));

            t_ptr->updateMatrix(); //After everything is loaded, update matrices
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH :{

            MeshProperty* lptr = static_cast<MeshProperty*>(prop_ptr);
            //Read mesh resource label
            *world_stream >> lptr->resource_relpath;
            //Read Root Node label
            *world_stream >> lptr->rootNodeStr;
            //Pointer will now point to mesh resource
            lptr->updateMeshPtr(); 

            world_stream->seekg(1, std::ofstream::cur);
            //Read castShadows bool
            world_stream->read(reinterpret_cast<char*>(&lptr->castShadows), sizeof(bool));
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_LIGHTSOURCE:{
            LightsourceProperty* ptr = static_cast<LightsourceProperty*>(prop_ptr);
            world_stream->seekg(1, std::ofstream::cur);

            world_stream->read(reinterpret_cast<char*>(&ptr->light_type), sizeof(LIGHTSOURCE_TYPE));
            world_stream->read(reinterpret_cast<char*>(&ptr->intensity), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->range), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->spot_angle), sizeof(float));

            float cl_r;
            float cl_g;
            float cl_b;

            world_stream->read(reinterpret_cast<char*>(&cl_r), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&cl_g), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&cl_b), sizeof(float));
            ptr->color = ZSRGBCOLOR(cl_r, cl_g, cl_b);

            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE:{
            TileProperty* lptr = static_cast<TileProperty*>(prop_ptr);
            //read textures relative pathes
            *world_stream >> lptr->diffuse_relpath;
            *world_stream >> lptr->transparent_relpath;
            //set pointers to textures
            lptr->updTexturePtr();

            world_stream->seekg(1, std::ofstream::cur);
            world_stream->read(reinterpret_cast<char*>(&lptr->anim_property.isAnimated), sizeof(bool));
            if(lptr->anim_property.isAnimated){ //if animated, then write animation properties
                world_stream->read(reinterpret_cast<char*>(&lptr->anim_property.framesX), sizeof(int));
                world_stream->read(reinterpret_cast<char*>(&lptr->anim_property.framesY), sizeof(int));
            }

            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_ANIMATION: {
            Engine::AnimationProperty* ptr = static_cast<Engine::AnimationProperty*>(prop_ptr);
            //Read animation clip name
            *world_stream >> ptr->anim_label;
            ptr->updateAnimationPtr();
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE: {
            Engine::NodeProperty* ptr = static_cast<Engine::NodeProperty*>(prop_ptr);
            //Read node name
            *world_stream >> ptr->node_label;
            //Skip 1 byte
            world_stream->seekg(1, std::ofstream::cur);
            //Now read node matrix
            for (unsigned int m_i = 0; m_i < 4; m_i++) {
                for (unsigned int m_j = 0; m_j < 4; m_j++) {
                    float* m_v = &ptr->transform_mat.m[m_i][m_j];
                    world_stream->read(reinterpret_cast<char*>(m_v), sizeof(float));
                }
            }
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_AUDSOURCE:{
            AudioSourceProperty* lptr = static_cast<AudioSourceProperty*>(prop_ptr);

            *world_stream >> lptr->resource_relpath;
            if(lptr->resource_relpath.compare("@none") != 0){
                lptr->updateAudioPtr(); //Pointer will now point to mesh resource
            }
            world_stream->seekg(1, std::ofstream::cur);
            //Load settings
            world_stream->read(reinterpret_cast<char*>(&lptr->source.source_gain), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&lptr->source.source_pitch), sizeof(float));
            lptr->source.apply_settings(); //Apply settings to openal

            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_SCRIPTGROUP:{
            ScriptGroupProperty* ptr = static_cast<ScriptGroupProperty*>(prop_ptr);
            world_stream->seekg(1, std::ofstream::cur);
            //Read scripts number
            world_stream->read(reinterpret_cast<char*>(&ptr->scr_num), sizeof(int));
            world_stream->seekg(1, std::ofstream::cur);
            //resize arrays
            ptr->scripts_attached.resize(static_cast<unsigned int>(ptr->scr_num));
            //iterate over all scripts and read their path
            for(unsigned int script_w_i = 0; script_w_i < static_cast<unsigned int>(ptr->scr_num); script_w_i ++){
                std::string script_path;
                *world_stream >> script_path;
                //Writing name (file path)
                ptr->scripts_attached[script_w_i].name = script_path;
                ScriptResource* res = game_data->resources->getScriptByLabel(script_path);
            }
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_SHADOWCASTER:{
            Engine::ShadowCasterProperty* ptr = static_cast<Engine::ShadowCasterProperty*>(prop_ptr);
            world_stream->seekg(1, std::ofstream::cur);
            //write collider type
            world_stream->read(reinterpret_cast<char*>(&ptr->TextureWidth), sizeof(int));
            world_stream->read(reinterpret_cast<char*>(&ptr->TextureHeight), sizeof(int));
            world_stream->read(reinterpret_cast<char*>(&ptr->shadow_bias), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->nearPlane), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->farPlane), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->projection_viewport), sizeof(float));
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_COLLIDER:{
            Engine::ColliderProperty* ptr = static_cast<Engine::ColliderProperty*>(prop_ptr);
            world_stream->seekg(1, std::ofstream::cur);
            //read collider type
            world_stream->read(reinterpret_cast<char*>(&ptr->coll_type), sizeof(COLLIDER_TYPE));
            //read isCustomPhysicalSize boolean
            world_stream->read(reinterpret_cast<char*>(&ptr->isCustomPhysicalSize), sizeof(bool));
            if(ptr->isCustomPhysicalSize){
                world_stream->read(reinterpret_cast<char*>(&ptr->cust_size.X), sizeof(float));
                world_stream->read(reinterpret_cast<char*>(&ptr->cust_size.Y), sizeof(float));
                world_stream->read(reinterpret_cast<char*>(&ptr->cust_size.Z), sizeof(float));

                world_stream->read(reinterpret_cast<char*>(&ptr->transform_offset.X), sizeof(float));
                world_stream->read(reinterpret_cast<char*>(&ptr->transform_offset.Y), sizeof(float));
                world_stream->read(reinterpret_cast<char*>(&ptr->transform_offset.Z), sizeof(float));
            }

            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TRIGGER: {
            Engine::TriggerProperty* ptr = static_cast<Engine::TriggerProperty*>(prop_ptr);
            world_stream->seekg(1, std::ofstream::cur);
            //read collider type
            world_stream->read(reinterpret_cast<char*>(&ptr->coll_type), sizeof(COLLIDER_TYPE));
            //read isCustomPhysicalSize boolean
            world_stream->read(reinterpret_cast<char*>(&ptr->isCustomPhysicalSize), sizeof(bool));
            if (ptr->isCustomPhysicalSize) {
                world_stream->read(reinterpret_cast<char*>(&ptr->cust_size.X), sizeof(float));
                world_stream->read(reinterpret_cast<char*>(&ptr->cust_size.Y), sizeof(float));
                world_stream->read(reinterpret_cast<char*>(&ptr->cust_size.Z), sizeof(float));

                world_stream->read(reinterpret_cast<char*>(&ptr->transform_offset.X), sizeof(float));
                world_stream->read(reinterpret_cast<char*>(&ptr->transform_offset.Y), sizeof(float));
                world_stream->read(reinterpret_cast<char*>(&ptr->transform_offset.Z), sizeof(float));
            }

            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_RIGIDBODY:{
            Engine::RigidbodyProperty* ptr = static_cast<Engine::RigidbodyProperty*>(prop_ptr);
            world_stream->seekg(1, std::ofstream::cur);
            //read collider type
            world_stream->read(reinterpret_cast<char*>(&ptr->coll_type), sizeof(COLLIDER_TYPE));
            world_stream->read(reinterpret_cast<char*>(&ptr->mass), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->gravity.X), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->gravity.Y), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->gravity.Z), sizeof(float));
            //read linear velocity
            world_stream->read(reinterpret_cast<char*>(&ptr->linearVel.X), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->linearVel.Y), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->linearVel.Z), sizeof(float));

            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL:{
            MaterialProperty* ptr = static_cast<MaterialProperty*>(prop_ptr);
            //reading path
            *world_stream >> ptr->material_path;

            ptr->material_ptr = game_data->resources->getMaterialByLabel(ptr->material_path)->material; //find it and process

            world_stream->seekg(1, std::ofstream::cur);
            world_stream->read(reinterpret_cast<char*>(&ptr->receiveShadows), sizeof(bool));

            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_CHARACTER_CONTROLLER:{
            Engine::CharacterControllerProperty* ptr = static_cast<Engine::CharacterControllerProperty*>(prop_ptr);
            world_stream->seekg(1, std::ofstream::cur); //Skip space
            world_stream->read(reinterpret_cast<char*>(&ptr->width), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->height), sizeof(float));

            world_stream->read(reinterpret_cast<char*>(&ptr->transform_offset.X), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->transform_offset.Y), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->transform_offset.Z), sizeof(float));
            break;
        }
        case PROPERTY_TYPE::GO_PROPERTY_TYPE_TERRAIN: {
            Engine::TerrainProperty* ptr = static_cast<Engine::TerrainProperty*>(prop_ptr);
            *world_stream >> ptr->file_label; //Write material relpath
            world_stream->seekg(1, std::ofstream::cur);
            //read dimensions
            world_stream->read(reinterpret_cast<char*>(&ptr->Width), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->Length), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->MaxHeight), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->castShadows), sizeof(bool));
            world_stream->read(reinterpret_cast<char*>(&ptr->textures_size), sizeof(int));
            world_stream->read(reinterpret_cast<char*>(&ptr->grassType_size), sizeof(int));

            ZsResource* terrain_res = game_data->resources->getResource<Engine::ZsResource>(ptr->file_label);
            //check, if terrain resource found
            if (terrain_res) {
                terrain_res->request = new Engine::Loader::LoadRequest;
                terrain_res->request->offset = terrain_res->offset;
                terrain_res->request->size = terrain_res->size;
                terrain_res->request->file_path = terrain_res->blob_path;
                loadImmideately(terrain_res->request);
                //Load terrain from readed binary data
                bool result = ptr->getTerrainData()->loadFromMemory((const char*)terrain_res->request->data);
                if (result) //if loading sucessstd::cout << "Terrain : Probably, missing terrain file" << file_path;
                    ptr->getTerrainData()->generateGLMesh();
            }
            world_stream->seekg(1, std::ofstream::cur);

            //Read textures relative pathes
            for (int texture_i = 0; texture_i < ptr->textures_size; texture_i++) {
                HeightmapTexturePair texture_pair;
                //Read texture pair
                *world_stream >> texture_pair.diffuse_relpath >> texture_pair.normal_relpath; //Write material relpath

                ptr->textures.push_back(texture_pair);
            }

            for (int grass_i = 0; grass_i < ptr->grassType_size; grass_i++) {
                HeightmapGrass grass;
                //Write grass diffuse texture
                *world_stream >> grass.diffuse_relpath;
                //Write grass size
                world_stream->seekg(1, std::ofstream::cur);
                world_stream->read(reinterpret_cast<char*>(&grass.scale.X), sizeof(float));
                world_stream->read(reinterpret_cast<char*>(&grass.scale.Y), sizeof(float));

                ptr->getTerrainData()->grass.push_back(grass);
            }
            //Fill grass buffers with transforms
            ptr->getTerrainData()->updateGrassBuffers();
            
            ptr->onValueChanged();

            break;
        }
    }*/
}
