#include "../../headers/world/World.h"

Engine::World::World(ResourceManager* manager){
    objects.resize(0);

    this->manager = manager;
}


Engine::GameObject* Engine::World::getGameObjectByStrId(std::string id){
    unsigned int objs_num = static_cast<unsigned int>(this->objects.size());
    for(unsigned int obj_it = 0; obj_it < objs_num; obj_it ++){ //Iterate over all objs in scene
        GameObject* obj_ptr = &this->objects[obj_it]; //Get pointer to checking object
        if(obj_ptr->str_id.compare(id) == 0) //if labels are same
            return obj_ptr; //Return founded object
    }
    return nullptr; //if we haven't found one
}

Engine::GameObject* Engine::World::addObject(GameObject obj){
    unsigned int free_index = this->objects.size();
    for(unsigned int obj_i = 0; obj_i < this->objects.size(); obj_i ++){
        GameObject* obj_ptr = &this->objects[obj_i];
        if(obj_ptr->alive == false){
            free_index = static_cast<int>(obj_i);
        }
    }

    if(free_index == objects.size()){ //all indeces are busy
        this->objects.push_back(obj);
    }else{
        this->objects[free_index] = obj;
    }

    GameObject* obj_ptr = &this->objects[free_index];

    obj_ptr->world_ptr = this;

    return obj_ptr;
}

void Engine::World::loadFromFile(std::string file){
    std::ifstream stream;
    stream.open(file, std::ifstream::binary); //open world file in binary mode

    std::string test_header;
    stream >> test_header; //Read header
    if(test_header.compare("ZSP_SCENE") != 0) //If it isn't zspire scene
        return; //Go out, we have nothing to do
    stream.seekg(1, std::ofstream::cur);
    int version = 0; //define version on world file
    int objs_num = 0; //define number of objects
    stream.read(reinterpret_cast<char*>(&version), sizeof(int)); //reading version
    stream.read(reinterpret_cast<char*>(&objs_num),     sizeof(int)); //reading objects count

    while(!stream.eof()){ //until file is over
        std::string prefix;
        stream >> prefix; //define and read prefix

        if(prefix.compare("G_OBJECT") == 0){ //if it is game object
            GameObject obj;
            obj.world_ptr = this; //Assign pointer to world
            stream >> obj.str_id;

            stream.seekg(1, std::ofstream::cur);
            stream.read(reinterpret_cast<char*>(&obj.render_type), sizeof(int)); //read render type

            //Then do the same sh*t, iterate until "G_END" came up
            while(true){
                stream >> prefix; //Read prefix
                if(prefix.compare("G_END") == 0){ //If end reached
                    break; //Then end this infinity loop
                }
                if(prefix.compare("G_CHI") == 0) { //Ops, it is chidren header
                    unsigned int amount;

                    stream.seekg(1, std::ofstream::cur);
                    stream.read(reinterpret_cast<char*>(&amount), sizeof(int));

                    for(unsigned int ch_i = 0; ch_i < amount; ch_i ++){ //Iterate over all written children to file
                        std::string child_str_id;
                        stream >> child_str_id; //Reading child string id

                        GameObjectLink link;
                        link.world_ptr = this; //Setting world pointer
                        link.obj_str_id = child_str_id; //Setting string ID
                        obj.children.push_back(link); //Adding to object
                    }

                }
                if(prefix.compare("G_PROPERTY") == 0){ //We found an property, zaeb*s'
                    obj.loadProperty(&stream);
                }
            }

            this->addObject(obj);
        }
    }
    //Now iterate over all objects and set depencies
    for(unsigned int obj_i = 0; obj_i < this->objects.size(); obj_i ++){
        GameObject* obj_ptr = &this->objects[obj_i];
        for(unsigned int chi_i = 0; chi_i < obj_ptr->children.size(); chi_i ++){ //Now iterate over all children
            GameObjectLink* child_ptr = &obj_ptr->children[chi_i];
            GameObject* child_go_ptr = child_ptr->updLinkPtr();
            child_go_ptr->parent = obj_ptr->getLinkToThisObject();
            child_go_ptr->hasParent = true;
        }
    }

    stream.close();
}

