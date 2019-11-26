#include "../../headers/engine/resources.h"
#include <fstream>
#include "../../headers/engine.h"

extern ZSpireEngine* engine_ptr;

Engine::ZsResource::ZsResource(){
    this->resource_type = RESOURCE_TYPE_NONE;
    this->resource_state = STATE_NOT_LOADED;
    size = 0;
    loadInstantly = false;
}

Engine::ZsResource::~ZsResource(){

}

void Engine::ZsResource::Release(){

}

Engine::ResourceManager::ResourceManager(){
    MeshResource* plane_resource = new MeshResource;
    plane_resource->resource_state = STATE_LOADED;
    plane_resource->rel_path = "@plane";
    plane_resource->resource_label = plane_resource->rel_path;
    plane_resource->mesh_ptr = Engine::getPlaneMesh2D();
    this->resources.push_back(plane_resource);

    MeshResource* cube_resource = new MeshResource;
    cube_resource->resource_state = STATE_LOADED;
    cube_resource->rel_path = "@cube";
    cube_resource->resource_label = cube_resource->rel_path;
    cube_resource->mesh_ptr = Engine::getCubeMesh3D();
    this->resources.push_back(cube_resource);

    MeshResource* sphere_resource = new MeshResource;
    sphere_resource->resource_state = STATE_LOADED;
    sphere_resource->rel_path = "@sphere";
    sphere_resource->resource_label = sphere_resource->rel_path;
    sphere_resource->mesh_ptr = Engine::getSphereMesh();
    this->resources.push_back(sphere_resource);
}

void Engine::ResourceManager::clear(){
    for(unsigned int i = 0; i < this->resources.size(); i ++){
        this->resources[i]->Release();
    }
    this->resources.clear();
}

Engine::ResourceManager::~ResourceManager(){
    clear();
}

//Push manually configured ZsResource from outside
void Engine::ResourceManager::pushResource(ZsResource* resource){
    switch(resource->resource_type){
        case RESOURCE_TYPE_NONE:{
            break;
        }
        case RESOURCE_TYPE_TEXTURE:{
            Engine::TextureResource* texture_ptr = static_cast<Engine::TextureResource*>(resource);
            texture_ptr->texture_ptr = allocTexture();
            break;
        }
        case RESOURCE_TYPE_MESH:{

            Engine::MeshResource* mesh_ptr = static_cast<Engine::MeshResource*>(resource);
            mesh_ptr->mesh_ptr = allocateMesh();
            break;
        }
        case RESOURCE_TYPE_AUDIO:{
            //resource_ptr = new Engine::AudioResource;
            break;
        }
        case RESOURCE_TYPE_SCRIPT:{
            //resource_ptr = new Engine::ScriptResource;
            break;
        }
        case RESOURCE_TYPE_MATERIAL:{
            //resource_ptr = new Engine::MaterialResource;
            break;
        }
        case RESOURCE_TYPE_ANIMATION:{
            //resource_ptr = new Engine::AnimationResource;
            static_cast<Engine::AnimationResource*>(resource)->animation_ptr = new Engine::Animation;
            break;
        }
    }

    this->resources.push_back(resource);
}

void Engine::ResourceManager::loadResourcesTable(std::string resmap_path){
    std::ifstream file_stream;
    file_stream.open(resmap_path, std::ifstream::binary); //open resources map file

    std::string prefix;

    while(!file_stream.eof()){
        file_stream >> prefix;

        if(prefix.compare("_END") == 0){
            return;
        }

        if(prefix.compare("entry") == 0){
            ZsResource resource;

            //read relative path
            file_stream >> resource.rel_path >> resource.resource_label >> resource.blob_path;

            file_stream.seekg(1, std::ofstream::cur); //Skip space
            //start byte
            file_stream.read(reinterpret_cast<char*>(&resource.offset), sizeof(uint64_t));
            //resource size
            file_stream.read(reinterpret_cast<char*>(&resource.size), sizeof(unsigned int));
            //reading resource type
            file_stream.read(reinterpret_cast<char*>(&resource.resource_type), sizeof(RESOURCE_TYPE));
            file_stream.seekg(1, std::ofstream::cur); //Skip space

            ZsResource* resource_ptr = nullptr;

            switch(resource.resource_type){
                case RESOURCE_TYPE_NONE:{
                    break;
                }
                case RESOURCE_TYPE_TEXTURE:{
                    resource_ptr = new Engine::TextureResource;

                    Engine::TextureResource* texture_ptr = static_cast<Engine::TextureResource*>(resource_ptr);
                    texture_ptr->texture_ptr = allocTexture();
                    break;
                }
                case RESOURCE_TYPE_MESH:{
                    resource_ptr = new MeshResource;

                    Engine::MeshResource* mesh_ptr = static_cast<Engine::MeshResource*>(resource_ptr);
                    mesh_ptr->mesh_ptr = allocateMesh();
                    break;
                }
                case RESOURCE_TYPE_AUDIO:{
                    resource_ptr = new Engine::AudioResource;
                    break;
                }
                case RESOURCE_TYPE_SCRIPT:{
                    resource_ptr = new Engine::ScriptResource;
                    break;
                }
                case RESOURCE_TYPE_MATERIAL:{
                    resource_ptr = new Engine::MaterialResource;
                    break;
                }
                case RESOURCE_TYPE_ANIMATION:{
                    resource_ptr = new Engine::AnimationResource;
                    static_cast<Engine::AnimationResource*>(resource_ptr)->animation_ptr = new Engine::Animation;
                    break;
                }
            }
            resource_ptr->blob_path = resource.blob_path;
            resource_ptr->offset = resource.offset;
            resource_ptr->rel_path = resource.rel_path;
            resource_ptr->resource_label = resource.resource_label;
            resource_ptr->size = resource.size;

            if(resource_ptr->loadInstantly)
                static_cast<Engine::ScriptResource*>(resource_ptr)->load();

            this->resources.push_back(resource_ptr);
        }
    }
    file_stream.close();
}

Engine::ZsResource* Engine::ResourceManager::getResourceByIndex(unsigned int index){
    return resources[index];
}

unsigned int Engine::ResourceManager::getResourcesSize(){
    return resources.size();
}
Engine::TextureResource::TextureResource(){
    this->resource_type = RESOURCE_TYPE_TEXTURE;
    texture_ptr = nullptr;
}

void Engine::TextureResource::Use(int slot){
    //Check, if texture already loaded
    if(this->resource_state == STATE_LOADED)
        //If loaded, just use it in slot
        this->texture_ptr->Use(slot);
    //Otherwise perform texture loading
    if(this->resource_state == STATE_NOT_LOADED){
        request = new Engine::Loader::LoadRequest;
        request->isBlob = true;
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;
        Engine::Loader::queryLoadingRequest(request);
        this->resource_state = STATE_LOADING_PROCESS;
    }
    //Check, if texture reading process finished
    if(this->resource_state == STATE_LOADING_PROCESS){
        if(this->request->done){
            this->texture_ptr->LoadDDSTextureFromBuffer(request->data);
            delete[] request->data;
            delete this->request;
            this->resource_state = STATE_LOADED;
        }
    }
}

void Engine::TextureResource::Release(){
    if(this->resource_state == STATE_LOADED){
        this->texture_ptr->Destroy();
        this->resource_state = STATE_NOT_LOADED;
    }
}

Engine::MeshResource::MeshResource(){
    this->resource_type = RESOURCE_TYPE_MESH;
    mesh_ptr = nullptr;
}

void Engine::MeshResource::Draw(){
    //Check, if texture already loaded
    if(this->resource_state == STATE_LOADED)
        //If loaded, just use it in slot
        this->mesh_ptr->Draw();
    //Otherwise perform texture loading
    if(this->resource_state == STATE_NOT_LOADED){
        request = new Engine::Loader::LoadRequest;
        request->isBlob = true;
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;
        Engine::Loader::queryLoadingRequest(request);
        this->resource_state = STATE_LOADING_PROCESS;
    }
    if(this->resource_state == STATE_LOADING_PROCESS){
        if(this->request->done){
            ZS3M::ImportedSceneFile isf;
            isf.loadFromBuffer(reinterpret_cast<char*>(request->data), request->size);

            for(unsigned int i = 0; i < isf.meshes_toWrite.size(); i ++){
                Engine::Mesh* mMesh = isf.meshes_toWrite[i];
                if(mMesh->mesh_label.compare(resource_label) == false)
                    mesh_ptr = mMesh;
            }

            delete[] request->data;
            delete this->request;
            this->resource_state = STATE_LOADED;
        }
    }
}
void Engine::MeshResource::Release(){
    if(this->resource_state == STATE_LOADED){
        this->mesh_ptr->Destroy();
        this->resource_state = STATE_NOT_LOADED;
    }
}

Engine::AudioResource::AudioResource(){
    this->resource_type = RESOURCE_TYPE_AUDIO;
    buffer = new SoundBuffer;
}

void Engine::AudioResource::load(){
    if(this->resource_state == STATE_NOT_LOADED){
        request = new Engine::Loader::LoadRequest;
        request->isBlob = true;
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;
        Engine::Loader::queryLoadingRequest(request);
        this->resource_state = STATE_LOADING_PROCESS;
    }
    if(this->resource_state == STATE_LOADING_PROCESS){
        if(this->request->done){
            this->buffer->loadBufferWAV(request->data);
            delete[] request->data;
            delete this->request;
            this->resource_state = STATE_LOADED;
        }
    }
}

void Engine::AudioResource::Release(){
    if(this->resource_state == STATE_LOADED){
        this->buffer->Destroy();
        this->resource_state = STATE_NOT_LOADED;
    }
}

Engine::ScriptResource::ScriptResource(){
    this->resource_type = RESOURCE_TYPE_SCRIPT;
    loadInstantly = true;
}

Engine::MaterialResource::MaterialResource(){
    this->resource_type = RESOURCE_TYPE_MATERIAL;
    material = nullptr;
}

Engine::AnimationResource::AnimationResource(){
    this->resource_type = RESOURCE_TYPE_ANIMATION;
    animation_ptr = nullptr;
}


void Engine::ScriptResource::load(){
    if(this->resource_state == STATE_NOT_LOADED){
        request = new Engine::Loader::LoadRequest;
        request->isBlob = true;
        request->data = new unsigned char[this->size];
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;
        loadImmideately(request);
        this->resource_state = STATE_LOADED;

        for(unsigned int i = 0; i < this->size; i ++){
            this->script_content.push_back(static_cast<char>(request->data[i]));
        }
    }
}

Engine::TextureResource* Engine::ResourceManager::getTextureByLabel(std::string label){
    for(unsigned int res = 0; res < this->resources.size(); res ++){
        ZsResource* resource_ptr = this->resources[res];
        if(resource_ptr->resource_type == RESOURCE_TYPE_TEXTURE && resource_ptr->rel_path.compare(label) == 0)
            return static_cast<TextureResource*>(resource_ptr);
    }
    return nullptr;
}

Engine::MeshResource* Engine::ResourceManager::getMeshByLabel(std::string label){
    for(unsigned int res = 0; res < this->resources.size(); res ++){
        ZsResource* resource_ptr = this->resources[res];
        if(resource_ptr->resource_type == RESOURCE_TYPE_MESH && resource_ptr->resource_label.compare(label) == 0)
            return static_cast<MeshResource*>(resource_ptr);
    }
    return nullptr;
}
Engine::AudioResource* Engine::ResourceManager::getAudioByLabel(std::string label){
    for(unsigned int res = 0; res < this->resources.size(); res ++){
        ZsResource* resource_ptr = this->resources[res];
        if(resource_ptr->resource_type == RESOURCE_TYPE_AUDIO && resource_ptr->resource_label.compare(label) == 0)
            return static_cast<AudioResource*>(resource_ptr);
    }
    return nullptr;
}
Engine::ScriptResource* Engine::ResourceManager::getScriptByLabel(std::string label){
    for(unsigned int res = 0; res < this->resources.size(); res ++){
        ZsResource* resource_ptr = this->resources[res];
        if(resource_ptr->resource_type == RESOURCE_TYPE_SCRIPT && resource_ptr->resource_label.compare(label) == 0)
            return static_cast<ScriptResource*>(resource_ptr);
    }
    return nullptr;
}

Engine::Mesh* Engine::allocateMesh(unsigned int size){
    Engine::Mesh* result = nullptr;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            result = new _ogl_Mesh[size];
            break;
        }
        case VULKAN : {
            result = new _vk_Mesh[size];
            break;
        }
    }
    return result;
}

Engine::Mesh* Engine::allocateMesh(){
    Engine::Mesh* result = nullptr;
    switch(engine_ptr->engine_info->graphicsApi){
        case OGL32 : {
            result = new _ogl_Mesh;
            break;
        }
        case VULKAN : {
            result = new _vk_Mesh;
            break;
        }
    }
    return result;
}
