#pragma once

#include <string>
#include "../World.hpp"
#include "../../engine/Resources.hpp"
#include "../../audio/SoundSource.hpp"
#include "../../game.h"

namespace Engine {

    class AudioSourceProperty : public IGameObjectComponent {
    private:
        bool isPlaySheduled;
    public:
        std::string resource_relpath; //Relative path to resource
        AudioResource* buffer_ptr;
        SoundSource source;

        Vec3 last_pos;

        void addPropertyInterfaceToInspector();
        void onValueChanged(); //Update soud buffer pointer and send source props
        void onUpdate(float deltaTime);
        void onObjectDeleted();
        void copyTo(IGameObjectComponent* dest);

        void setAudioResource(AudioResource* res);
        void updateAudioPtr();
        void audio_start();
        void audio_pause();
        void audio_stop();

        float getGain();
        float getPitch();
        void setGain(float gain);
        void setPitch(float pitch);

        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);
        void bindObjectPropertyToAngel(AGScriptMgr* mgr);

        AudioSourceProperty();
    };

}