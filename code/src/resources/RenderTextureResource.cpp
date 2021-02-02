#include "../../headers/engine/Resources.hpp"

using namespace Engine;

RenderTextureResource::RenderTextureResource() {
	this->resource_type = RESOURCE_TYPE_RENDER_TEXTURE_BUFFER;
	loadInstantly = true;
}
RenderTextureResource::~RenderTextureResource() {
	delete mFramebuffer;
}

void RenderTextureResource::load() {

    if (this->resource_state == RESOURCE_STATE::STATE_NOT_LOADED) {
        request = new Engine::Loader::LoadRequest;
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;
        loadImmideately(request);
        this->resource_state = RESOURCE_STATE::STATE_LOADED;
        //Clear string data, first
        parseData(request->data);

        this->size = request->size;
        delete[] request->data;
        delete this->request;
    }

	mFramebuffer = allocFramebuffer(512, 512);

	mFramebuffer->AddTexture(TextureFormat::FORMAT_RGBA);
}

void RenderTextureResource::parseData(unsigned char* data) {
    std::string prefix;
    unsigned int iter = 0;

    while (true) {
        prefix.clear();
        //Read prefix
        if (iter >= size)
            break;
        while (data[iter] != ' ' && data[iter] != '\n') {
            if (data[iter] != '\0')
                prefix += data[iter];
            iter++;
        }

        if (prefix.compare("END") == 0) { //If end reached
            break; //Then end this infinity loop
        }
        if (prefix.compare("WIDTH") == 0) { //If end reached

        }
        if (prefix.compare("HEIGHT") == 0) { //If end reached

        }
    }
}