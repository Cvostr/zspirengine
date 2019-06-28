#include "../../../headers/world/go_properties.h"
#include "../../../headers/world/tile_properties.h"

void Engine::GameObject::loadProperty(std::ifstream* world_stream){
    PROPERTY_TYPE type;
    world_stream->seekg(1, std::ofstream::cur); //Skip space
    world_stream->read(reinterpret_cast<char*>(&type), sizeof(int));
    //Spawn new property with readed type
    addProperty(type);
    GameObjectProperty* prop_ptr = getPropertyPtrByType(type); //get created property
    world_stream->read(reinterpret_cast<char*>(&prop_ptr->active), sizeof(bool));
    //since more than 1 properties same type can't be on one gameobject
    switch(type){
        case GO_PROPERTY_TYPE_LABEL :{
            std::string label;
            *world_stream >> label;
            LabelProperty* lptr = static_cast<LabelProperty*>(prop_ptr);
            this->label_ptr = &lptr->label; //Making GameObjects's pointer to string in label property
            lptr->label = label; //Write loaded string
            break;
        }
        case GO_PROPERTY_TYPE_TRANSFORM :{
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

            t_ptr->updateMat(); //After everything is loaded, update matrices
            break;
        }
        case GO_PROPERTY_TYPE_MESH :{
            std::string rel_path;
            *world_stream >> rel_path;
            MeshProperty* lptr = static_cast<MeshProperty*>(prop_ptr);
            lptr->resource_relpath = rel_path; //Write loaded mesh relative path
            lptr->updateMeshPtr(); //Pointer will now point to mesh resource

            break;
        }
        case GO_PROPERTY_TYPE_LIGHTSOURCE:{
            LightsourceProperty* ptr = static_cast<LightsourceProperty*>(prop_ptr);
            world_stream->seekg(1, std::ofstream::cur);

            world_stream->read(reinterpret_cast<char*>(&ptr->light_type), sizeof(LIGHTSOURCE_TYPE));
            world_stream->read(reinterpret_cast<char*>(&ptr->intensity), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&ptr->range), sizeof(float));

            float cl_r;
            float cl_g;
            float cl_b;

            world_stream->read(reinterpret_cast<char*>(&cl_r), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&cl_g), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&cl_b), sizeof(float));
            ptr->color = ZSRGBCOLOR(cl_r, cl_g, cl_b);

            break;
        }
        case GO_PROPERTY_TYPE_TILE:{
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

        case GO_PROPERTY_TYPE_AUDSOURCE:{
            std::string rel_path;

            AudioSourceProperty* lptr = static_cast<AudioSourceProperty*>(prop_ptr);

            *world_stream >> lptr->resource_relpath;
            if(rel_path.compare("@none") != 0){
                lptr->updateAudioPtr(); //Pointer will now point to mesh resource
            }
            world_stream->seekg(1, std::ofstream::cur);
            //Load settings
            world_stream->read(reinterpret_cast<char*>(&lptr->source.source_gain), sizeof(float));
            world_stream->read(reinterpret_cast<char*>(&lptr->source.source_pitch), sizeof(float));
            //lptr->source.apply_settings(); //Apply settings to openal

            break;
        }
    }
}
