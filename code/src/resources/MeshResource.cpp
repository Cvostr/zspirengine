#include "../../headers/engine/resources.h"

Engine::MeshResource::MeshResource() {
    this->resource_type = RESOURCE_TYPE_MESH;
    mesh_ptr = nullptr;
}

void Engine::MeshResource::DrawInstanced(unsigned int inst_num) {
    //Check, if texture already loaded
    if (this->resource_state == RESOURCE_STATE::STATE_LOADED)
        //If loaded, just use it in slot
        this->mesh_ptr->DrawInstanced(inst_num);
    //Otherwise perform texture loading
    if (this->resource_state == RESOURCE_STATE::STATE_NOT_LOADED) {
        request = new Engine::Loader::LoadRequest;
        request->offset = this->offset;
        request->size = this->size;
        request->file_path = this->blob_path;
        Engine::Loader::queryLoadingRequest(request);
        this->resource_state = RESOURCE_STATE::STATE_LOADING_PROCESS;
    }
    if (this->resource_state == RESOURCE_STATE::STATE_LOADING_PROCESS) {
        if (this->request->done) {
            ZS3M::ImportedSceneFile isf;
            isf.loadFromBuffer(reinterpret_cast<char*>(request->data), request->size);

            for (unsigned int i = 0; i < isf.meshes_toRead.size(); i++) {
                Engine::Mesh* mMesh = isf.meshes_toRead[i];
                if (mMesh->mesh_label.compare(resource_label) == false)
                    mesh_ptr = mMesh;
                else {
                    mMesh->Destroy();
                    delete mMesh;
                }
            }

            delete[] request->data;
            delete this->request;
            this->resource_state = RESOURCE_STATE::STATE_LOADED;
        }
    }
}

void Engine::MeshResource::Draw() {
    DrawInstanced(1);
}
void Engine::MeshResource::Release() {
    if (this->resource_state == RESOURCE_STATE::STATE_LOADED) {
        this->mesh_ptr->Destroy();
        this->resource_state = RESOURCE_STATE::STATE_NOT_LOADED;
    }
}

Engine::MeshResource* Engine::ResourceManager::getMeshByLabel(std::string label) {
    for (unsigned int res = 0; res < this->resources.size(); res++) {
        ZsResource* resource_ptr = this->resources[res];
        if (resource_ptr->resource_type == RESOURCE_TYPE_MESH && resource_ptr->resource_label.compare(label) == 0)
            return static_cast<MeshResource*>(resource_ptr);
    }
    return nullptr;
}