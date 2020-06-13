#include "../../headers/engine/loader.h"
#include <thread>
#include <list>
#include <fstream>
#include <iostream>

#define LOADER_QUEUE_SIZE 300

static bool loader_thread_working = true;
static std::string blob_root_directory;
Engine::Loader::LoadRequest* requests[LOADER_QUEUE_SIZE];
int queue_length = 0;

void _LDR_load(Engine::Loader::LoadRequest* req){
    std::cout << "Loading resource " << req->file_path << std::endl;
    //Declare blob loader stream
    std::ifstream stream;
    //Get absolute path to blob file
    std::string file_path = blob_root_directory + req->file_path;
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

void loop(){
    while(loader_thread_working){
        //If there are some queues in pool
        if(queue_length > 0){
            std::cout << "Loading ";
            //Obtain pointer to LoadRequest
            Engine::Loader::LoadRequest* req = requests[LOADER_QUEUE_SIZE - queue_length];
            //Reduce requests pool amount
            queue_length--;
            //Load resource by request
            _LDR_load(req);
        }
    }
}

void Engine::Loader::start(){
    //Set loop condition to true
    loader_thread_working = true;
    //Start thread
    std::thread loader_loop(loop);
    //Send thread to background
    loader_loop.detach();
}

void Engine::Loader::stop(){
    //Finish thread
    loader_thread_working = false;
    //Clear queue
    queue_length = 0;
}

void Engine::Loader::queryLoadingRequest(LoadRequest* req){
    //loader_loop
    requests[LOADER_QUEUE_SIZE - 1 - (queue_length)] = req;
    queue_length++;
}

void Engine::Loader::loadImmideately(LoadRequest* req, std::string* absolute_path){
    _LDR_load(req);
    //Get absolute path to blob file
    std::string file_path = blob_root_directory + req->file_path;
    if(absolute_path != nullptr)
        *absolute_path = file_path;
}

void Engine::Loader::setBlobRootDirectory(std::string& dir){
    blob_root_directory = dir;
}
