#include "../../headers/world/World.h"

Engine::World::World(){
    objects.resize(0);
}

Engine::GameObject::GameObject(){

}

void Engine::World::loadFromFile(std::string file){
    std::ifstream stream;
    stream.open(file, std::ifstream::binary);

    std::string test_header;
    stream >> test_header; //Read header
    if(test_header.compare("ZSP_SCENE") != 0) //If it isn't zspire scene
        return; //Go out, we have nothing to do
    stream.seekg(1, std::ofstream::cur);
    int version = 0;
    int objs_num = 0;
    stream.read(reinterpret_cast<char*>(&version), sizeof(int)); //reading version
    stream.read(reinterpret_cast<char*>(&objs_num), sizeof(int)); //reading objects count

    while(!stream.eof()){
        std::string prefix;
        stream >> prefix;

        if(prefix.compare("G_OBJECT") == 0){
            GameObject obj;
            stream >> obj.str_id;

            this->objects.push_back(obj);
        }
    }

    stream.close();
}
