#pragma once

#include "../World.hpp"

namespace Engine {
    class NodeProperty : public IGameObjectComponent {
    public:
        std::string node_label;

        Vec3 translation;
        Vec3 scale;
        ZSQUATERNION rotation;
        //Node transform from file
        Mat4 transform_mat;
        //Caclulated node transform
        Mat4 abs;

        void copyTo(IGameObjectComponent* dest);
        // void addPropertyInterfaceToInspector();
        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);

        NodeProperty();
    };
}