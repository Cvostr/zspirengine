#pragma once

#include "../World.hpp"
#include "../../engine/Resources.hpp"

namespace Engine {
    class AnimationProperty : public Engine::IGameObjectComponent {
    private:
    public:
        bool Playing;
        double start_sec;

        Engine::AnimationResource* anim_prop_ptr;
        std::string anim_label;

        void addPropertyInterfaceToInspector();
        void onPreRender(Engine::Renderer* pipeline);
        void onValueChanged();
        void copyTo(Engine::IGameObjectComponent* dest);

        void play();
        void stop();

        void setAnimationResource(Engine::AudioResource* anim);

        void updateAnimationPtr();
        void updateNodeTransform(GameObject* obj, Mat4 parent);

        void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);
        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);

        AnimationProperty();
    };
}