#include "../../headers/engine/loader.h"
#include <thread>
#include <list>
#include <fstream>

static bool loader_thread_working = true;
static std::string blob_root_directory;
std::list<Engine::Loader::LoadRequest*> requests;

void loop(){
    while(loader_thread_working){
        if(requests.size() > 0){
            Engine::Loader::LoadRequest* req = requests.front();

            std::ifstream stream;
            std::string file_path = blob_root_directory + req->file_path;
            if(req->size > 0){
                stream.open(file_path, std::ofstream::binary);
                stream.seekg(static_cast<long>(req->offset));
            }else{
                stream.open(file_path, std::ofstream::binary | std::ofstream::ate);
                req->size = static_cast<unsigned int>(stream.tellg());
                stream.seekg(0);
            }
            req->data = new unsigned char[req->size];
            stream.read(reinterpret_cast<char*>(req->data), req->size);
            stream.close();
            req->done = true;
            requests.remove(req);
        }
    }
}

void Engine::Loader::start(){
    std::thread loader_loop(loop);
    loader_loop.detach();
    loader_thread_working = true;
}

void Engine::Loader::stop(){
    loader_thread_working = false;
    requests.clear();
}

void Engine::Loader::queryLoadingRequest(LoadRequest* req){
    //loader_loop
    requests.push_back(req);
}

void Engine::Loader::loadImmideately(LoadRequest* req){
    std::ifstream stream;
    std::string file_path = blob_root_directory + req->file_path;
    stream.open(file_path, std::ofstream::binary);
    stream.seekg(static_cast<long>(req->offset));
    stream.read(reinterpret_cast<char*>(req->data), req->size);
    stream.close();
}

void Engine::Loader::setBlobRootDirectory(std::string& dir){
    blob_root_directory = dir;
}
