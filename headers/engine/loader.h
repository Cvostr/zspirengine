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
            bool isBlob;
            LoadRequest(){
                done = false;
                isBlob = false;
            }
        }LoadRequest;

        void stop();
        void start();
        void queryLoadingRequest(LoadRequest* req);
        void loadImmideately(LoadRequest* req);

        void setBlobRootDirectory(std::string& dir);
    }
}
#endif // LOADER_H
