#include "../../headers/engine/BackgroundLoader.hpp"
#include <thread>
#include <list>
#include <fstream>
#include <iostream>
#include "../../headers/threading/Mutex.hpp"

Engine::Loader::Loader _Loader;

void _LDR_load(Engine::Loader::LoadRequest* req){
    std::cout << "Loading resource " << req->file_path << std::endl;
    //Declare blob loader stream
    std::ifstream stream;
    //Get absolute path to blob file
    std::string file_path = _Loader.GetBlobRootDirectory() + req->file_path;
    //Check, if size explicitly specified
    if(req->size > 0){
        //if size explicitly specified
        stream.open(file_path, std::ofstream::binary);
        stream.seekg(static_cast<long>(req->offset));
    }else{
        //if size explicitly specified, then try to load file
        //and get file size
        stream.open(file_path, std::ofstream::binary | std::ofstream::ate);
        req->size = static_cast<unsigned int>(stream.tellg());
        stream.seekg(0);
    }

    if (!stream.is_open()) {
        std::cout << "Error loading resource " << req->file_path << std::endl;
    }

    req->data = new unsigned char[req->size];
    stream.read(reinterpret_cast<char*>(req->data), req->size);
    stream.close();
    req->done = true;
}

void Engine::Loader::Loader::THRFunc() {
    while (mShouldRun) {
        //If there are some queues in pool
        if (queue_length > 0) {
            //Lock mutex in this thread
            mMutex->Lock();
            //Obtain pointer to LoadRequest
            Engine::Loader::LoadRequest* req = requests[LOADER_QUEUE_SIZE - queue_length];
            //Reduce requests pool amount
            queue_length--;
            //unlock thread
            mMutex->Release();
            //Load resource by request
            _LDR_load(req);
        }
    }
}

void Engine::Loader::Loader::queryLoadingRequest(LoadRequest* req) {
    mMutex->Lock();
    requests[LOADER_QUEUE_SIZE - 1 - (queue_length)] = req;
    queue_length++;
    mMutex->Release();
}

void Engine::Loader::start(){
    _Loader.Run();
}

void Engine::Loader::stop(){
    _Loader.Stop();
}

void Engine::Loader::queryLoadingRequest(LoadRequest* req){
    _Loader.queryLoadingRequest(req);
}

void Engine::Loader::loadImmideately(LoadRequest* req, std::string* absolute_path){
    _LDR_load(req);
    //Get absolute path to blob file
    std::string file_path = _Loader.GetBlobRootDirectory() + req->file_path;
    if(absolute_path != nullptr)
        *absolute_path = file_path;
}

void Engine::Loader::setBlobRootDirectory(std::string& dir){
    _Loader.SetBlobRootDirectory(dir);
}