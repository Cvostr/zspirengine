#pragma once

#include "../World.hpp"
#include "../../engine/Resources.hpp"

namespace Engine {
    class MeshProperty : public IGameObjectComponent {
    public:
        std::string resource_relpath; //Relative path to resource
        MeshResource* mesh_ptr; //Pointer to mesh
        bool castShadows;

        std::string rootNodeStr;
        GameObject* skinning_root_node;

        void updateMeshPtr(); //Updates pointer according to resource_relpath
        void updateRootNodePtr();
        void copyTo(IGameObjectComponent* dest);
        void onPreRender(Engine::Renderer* pipeline);
        void setMeshResource(MeshResource* resource);
        bool HasBones();

        //EDITOR STUFF
        void addPropertyInterfaceToInspector();
        void onValueChanged(); //Update mesh    pointer
        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);
        void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

        MeshProperty();
    };

}