#include "../../headers/world/World.h"

Engine::World::World(){
    objects.resize(0);
}

Engine::GameObjectLink::GameObjectLink(){
    ptr = nullptr;
    world_ptr = nullptr;
}

Engine::GameObject::GameObject(){

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
                    //object.loadProperty(&world_stream);
                }
            }

            this->objects.push_back(obj);
        }
    }

    stream.close();
}
