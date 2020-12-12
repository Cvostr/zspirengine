#pragma once

#include "../World.hpp"

namespace Engine {

    class MaterialProperty : public Engine::IGameObjectComponent {
    private:
        std::string group_label;
    public:
        //Pointer to picked material
        Material* material_ptr;
        //Path to material fil
        std::string material_path;
        //Draw shadows on object
        bool receiveShadows;

        void setMaterial(Material* mat);
        void _setMaterial(MaterialResource* mat);
        void setMaterial(std::string path);
        void addPropertyInterfaceToInspector();
        void onValueChanged();
        void copyTo(Engine::IGameObjectComponent* dest);
        void onRender(Engine::Renderer* pipeline);

        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);

        MaterialProperty();
    };
}