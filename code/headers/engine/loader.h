#ifndef LOADER_H
#define LOADER_H

#include <string>

namespace Engine {
    namespace Loader {

        typedef struct LoadRequest{
            unsigned char* data;
            std::string file_path;
            unsigned long offset;
            unsigned int size;
            bool done;
            LoadRequest(){
                done = false;
                size = 0;
                offset = 0;
                data = nullptr;
            }
        }LoadRequest;

        void stop();
        void start();
        void queryLoadingRequest(LoadRequest* req);
        void loadImmideately(LoadRequest* req, std::string* absolute_path = nullptr);

        void setBlobRootDirectory(std::string& dir);
    }
}
#endif // LOADER_H
