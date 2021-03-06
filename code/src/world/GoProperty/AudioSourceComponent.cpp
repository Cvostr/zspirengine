#include <world/ObjectsComponents/AudioSourceComponent.hpp>

extern ZSGAME_DATA* game_data;

Engine::AudioSourceProperty::AudioSourceProperty() :
    buffer_ptr(nullptr),
    resource_relpath("@none"),
    isPlaySheduled(false)
{
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_AUDSOURCE;
}

void Engine::AudioSourceProperty::onValueChanged() {
    updateAudioPtr();

    this->source.apply_settings();
}

void Engine::AudioSourceProperty::updateAudioPtr() {
    this->buffer_ptr = game_data->resources->getAudioByLabel(resource_relpath);

    if (buffer_ptr == nullptr) return;

    this->source.setAlBuffer(this->buffer_ptr->buffer);
}

void Engine::AudioSourceProperty::onUpdate(float deltaTime) {
    if (buffer_ptr == nullptr) return;
    //Poll loading process
    if (buffer_ptr->resource_state == RESOURCE_STATE::STATE_LOADING_PROCESS)
        this->buffer_ptr->load();
    //If loaded, then play
    if (buffer_ptr->resource_state == RESOURCE_STATE::STATE_LOADED && isPlaySheduled) {
        audio_start();
        isPlaySheduled = false;
    }

    Engine::TransformProperty* transform = go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();
    //if object position changed
    if (transform->translation != this->last_pos) {
        //apply new position to openal audio source
        this->source.setPosition(transform->translation);
        this->last_pos = transform->translation;
    }
}

void Engine::AudioSourceProperty::copyTo(Engine::IGameObjectComponent* dest) {
    if (dest->type != this->type) return; //if it isn't audiosource then exit

    AudioSourceProperty* _dest = static_cast<AudioSourceProperty*>(dest);

    //Do base things
    IGameObjectComponent::copyTo(dest);

    _dest->source.Init();
    _dest->resource_relpath = this->resource_relpath;
    _dest->source.source_gain = this->source.source_gain;
    _dest->source.source_pitch = this->source.source_pitch;
    _dest->source.setPosition(this->source.source_pos);
    _dest->buffer_ptr = this->buffer_ptr;
    //_dest->source.setAlBuffer(this->buffer_ptr);
}

void Engine::AudioSourceProperty::setAudioResource(Engine::AudioResource* res) {
    this->buffer_ptr = res;

    if (buffer_ptr == nullptr) return;

    this->resource_relpath = res->resource_label;
    this->source.setAlBuffer(this->buffer_ptr->buffer);
}

void Engine::AudioSourceProperty::audio_start() {
    //Check, if resource exists
    if (buffer_ptr != nullptr) {
        //resource exists
        //Check, if resource loaded
        if (buffer_ptr->resource_state == RESOURCE_STATE::STATE_LOADED) {
            //resource loaded
            //Update source buffer
            this->source.setAlBuffer(this->buffer_ptr->buffer);
            //play source
            this->source.play();
        }
        else {
            //resource isn't loaded
            this->buffer_ptr->load();
            isPlaySheduled = true;
        }
    }
}

void Engine::AudioSourceProperty::audio_stop() {
    this->source.stop();
}

void Engine::AudioSourceProperty::audio_pause() {
    this->source.pause();
}

float Engine::AudioSourceProperty::getGain() {
    return source.source_gain;
}
float Engine::AudioSourceProperty::getPitch() {
    return source.source_pitch;
}
void Engine::AudioSourceProperty::setGain(float gain) {
    source.source_gain = gain;
    source.apply_settings();
}
void Engine::AudioSourceProperty::setPitch(float pitch) {
    source.source_pitch = pitch;
    source.apply_settings();
}

void Engine::AudioSourceProperty::onObjectDeleted() {
    this->source.stop(); //Stop at first
    this->source.Destroy();
}

void Engine::AudioSourceProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;

    readString(resource_relpath, data, offset);

    if (resource_relpath.compare("@none") != 0) {
        updateAudioPtr(); //Pointer will now point to mesh resource
    }
    
    //Load settings
    readBinaryValue(&source.source_gain, data + offset, offset);
    readBinaryValue(&source.source_pitch, data + offset, offset);

    source.apply_settings(); //Apply settings to openal
}

void Engine::AudioSourceProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    if (resource_relpath.empty()) //check if object has no audioclip
        stream->writeString("@none");
    else
        stream->writeString(resource_relpath);

    stream->writeBinaryValue(&source.source_gain);
    stream->writeBinaryValue(&source.source_pitch);
    stream->writeBinaryValue(&source.looped);
}

void Engine::AudioSourceProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    mgr->RegisterObjectType(AUDSOURCE_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
    
    mgr->RegisterObjectMethod(AUDSOURCE_PROP_TYPE_NAME, "void Play()", asMETHOD(AudioSourceProperty, audio_start), asCALL_THISCALL);
    mgr->RegisterObjectMethod(AUDSOURCE_PROP_TYPE_NAME, "void Stop()", asMETHOD(AudioSourceProperty, audio_stop), asCALL_THISCALL);
    mgr->RegisterObjectMethod(AUDSOURCE_PROP_TYPE_NAME, "void Pause()", asMETHOD(AudioSourceProperty, audio_pause), asCALL_THISCALL);
    mgr->RegisterObjectMethod(AUDSOURCE_PROP_TYPE_NAME, "float getGain()", asMETHOD(AudioSourceProperty, getGain), asCALL_THISCALL);
    mgr->RegisterObjectMethod(AUDSOURCE_PROP_TYPE_NAME, "float getPitch()", asMETHOD(AudioSourceProperty, getPitch), asCALL_THISCALL);
    mgr->RegisterObjectMethod(AUDSOURCE_PROP_TYPE_NAME, "void setPitch(float)", asMETHOD(AudioSourceProperty, setPitch), asCALL_THISCALL);
    mgr->RegisterObjectMethod(AUDSOURCE_PROP_TYPE_NAME, "void setGain(float)", asMETHOD(AudioSourceProperty, setGain), asCALL_THISCALL);
    mgr->RegisterObjectMethod(AUDSOURCE_PROP_TYPE_NAME, "void setAudio(AudioResource@)", asMETHOD(AudioSourceProperty, setAudioResource), asCALL_THISCALL);
}