#include "../../headers/engine/loader.h"
#include <thread>
#include <list>
#include <fstream>

static bool working = true;
static std::string blob_root_directory;
std::list<Engine::Loader::LoadRequest*> requests;

void loop(){
    while(working){
        if(requests.size() > 0){
            Engine::Loader::LoadRequest* req = requests.front();

            std::ifstream stream;
            std::string file_path = blob_root_directory + req->file_path;
            stream.open(file_path, std::ofstream::binary);
            stream.seekg(static_cast<long>(req->offset));
            stream.read(reinterpret_cast<char*>(req->data), req->size);
            req->done = true;
            requests.remove(req);
        }
    }
}

void Engine::Loader::start(){
    std::thread loader_loop(loop);
    loader_loop.detach();
    working = true;
}

void Engine::Loader::stop(){
    working = false;
}

void Engine::Loader::queryLoadingRequest(LoadRequest* req){
    //loader_loop
    requests.push_back(req);
}

void Engine::Loader::setBlobRootDirectory(std::string dir){
    blob_root_directory = dir;
}
