#pragma once
#include <string>
#include <threading/Thread.hpp>
#include <threading/Mutex.hpp>

#define LOADER_QUEUE_SIZE 300

namespace Engine {
    namespace Loader {

        typedef struct LoadRequest{
            unsigned char* data;
            std::string file_path;
            uint64_t offset;
            unsigned int size;
            bool done;
            LoadRequest(){
                done = false;
                size = 0;
                offset = 0;
                data = nullptr;
            }
        }LoadRequest;

        class Loader : public Thread {
        private:
            Mutex* mMutex;
            std::string mBlobRootDirectory;
            Engine::Loader::LoadRequest* requests[LOADER_QUEUE_SIZE];
            int queue_length;
        public:
            void SetBlobRootDirectory(std::string Dir) { mBlobRootDirectory = Dir; }
            std::string GetBlobRootDirectory() { return mBlobRootDirectory; }
            void queryLoadingRequest(LoadRequest* req);
            void THRFunc();

            Loader() :
                mMutex(new Mutex),
                queue_length(0)
            {
            
            }
        };

        void stop();
        void start();
        void queryLoadingRequest(LoadRequest* req);
        void loadImmideately(LoadRequest* req, std::string* absolute_path = nullptr);

        void setBlobRootDirectory(std::string& dir);
    }
}