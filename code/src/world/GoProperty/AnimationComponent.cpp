#include <world/ObjectsComponents/AnimationComponent.hpp>
#include <world/ObjectsComponents/NodeComponent.hpp>
#include <game.h>

extern ZSGAME_DATA* game_data;

Engine::AnimationProperty::AnimationProperty() : anim_label("@none"),
                                                 anim_prop_ptr(nullptr),
                                                 Playing(false),
                                                 start_sec(0)
{
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_ANIMATION;
}

void Engine::AnimationProperty::play() {
    if (Playing) return;
    //if user specified animation, then play it!
    if (anim_prop_ptr != nullptr) {
        //Send animation to multithreaded loading, if need
        anim_prop_ptr->load();

        start_sec = (static_cast<double>(SDL_GetTicks()) / 1000);
        Playing = true;
    }
}
void Engine::AnimationProperty::stop() {
    Playing = false;
}

void Engine::AnimationProperty::onPreRender(Renderer* pipeline) {
    GameObject* obj = go_link.updLinkPtr();

    Engine::Animation* anim_prop_ptr = nullptr;
    //Try to get loading result
    if (this->anim_prop_ptr != nullptr && Playing && this->anim_prop_ptr->resource_state == RESOURCE_STATE::STATE_LOADING_PROCESS)
        this->anim_prop_ptr->load();

    if (this->anim_prop_ptr != nullptr && Playing && this->anim_prop_ptr->resource_state == RESOURCE_STATE::STATE_LOADED) {
        if (this->anim_prop_ptr != nullptr)
            anim_prop_ptr = this->anim_prop_ptr->animation_ptr;

        //Calcualte current Time
        double curTime = (static_cast<double>(SDL_GetTicks()) / 1000) - this->start_sec;
        //Time in animation ticks
        double Ticks = anim_prop_ptr->TPS * curTime;
        //Calculate current animation time
        double animTime = fmod(Ticks, anim_prop_ptr->duration);
        //Iterate over all animation channels
        for (unsigned int channels_i = 0; channels_i < anim_prop_ptr->NumChannels; channels_i++) {
            Engine::AnimationChannel* ch = &anim_prop_ptr->channels[channels_i];
            GameObject* node = obj->getChildObjectWithNodeLabel(ch->bone_name);
            //Check, if that gameobject exists
            if (node) {
                //if exists
                NodeProperty* prop = node->getPropertyPtr<NodeProperty>();
                prop->translation = ch->getPostitionInterpolated(animTime);
                prop->scale = ch->getScaleInterpolated(animTime);
                prop->rotation = ch->getRotationInterpolated(animTime);
            }
        }
    }
    Mat4 identity_matrix = getIdentity();
    updateNodeTransform(obj, identity_matrix);
}

void Engine::AnimationProperty::updateNodeTransform(GameObject* obj, Mat4 parent) {

    if (!obj) return;
    NodeProperty* prop = obj->getPropertyPtr<NodeProperty>();
    if (!prop) return;
    //Assign base node transform
    prop->abs = prop->transform_mat;
    //Are we need to play animation
    if (this->anim_prop_ptr != nullptr && Playing && this->anim_prop_ptr->resource_state == RESOURCE_STATE::STATE_LOADED) {
        //Try to find anim channel by label of node
        Engine::AnimationChannel* cha = this->anim_prop_ptr->animation_ptr->getChannelByNodeName(prop->node_label);
        //if channel is found
        if (cha) {
            Mat4 transl = transpose(getTranslationMat(prop->translation));
            Mat4 _sca = transpose(getScaleMat(prop->scale));
            Mat4 rot = (getRotationMat(prop->rotation));
            //Multiply all matrices
            prop->abs = transl * rot * _sca;
        }
    }
    //Apply parent transform
    prop->abs = parent * prop->abs;
    //Go deeper in tree
    for (size_t i = 0; i < obj->mChildren.size(); i++) {
        updateNodeTransform(obj->mChildren[i].updLinkPtr(), prop->abs);
    }
}

void Engine::AnimationProperty::copyTo(IGameObjectComponent* dest) {
    if (dest->type != this->type) return; //if it isn't animation

    //Do base things
    IGameObjectComponent::copyTo(dest);
    AnimationProperty* _dest = static_cast<AnimationProperty*>(dest);

    _dest->anim_label = this->anim_label;
    _dest->anim_prop_ptr = this->anim_prop_ptr;
}

void Engine::AnimationProperty::setAnimationResource(Engine::AudioResource* anim) {
    this->anim_label = anim->resource_label;
    updateAnimationPtr();
}

void Engine::AnimationProperty::updateAnimationPtr() {
    anim_prop_ptr = game_data->resources->getAnimationByLabel(this->anim_label);
}

void Engine::AnimationProperty::onValueChanged() {
    updateAnimationPtr();
}

void Engine::AnimationProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    //Read animation clip name
    readString(anim_label, data, offset);
    //update pointer to resource
    updateAnimationPtr();
}

void Engine::AnimationProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    stream->writeString(anim_label);
}

void Engine::AnimationProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    mgr->RegisterObjectType(ANIM_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

    mgr->RegisterObjectMethod(ANIM_PROP_TYPE_NAME, "void Play()", asMETHOD(AnimationProperty, play), asCALL_THISCALL);
    mgr->RegisterObjectMethod(ANIM_PROP_TYPE_NAME, "void Stop()", asMETHOD(AnimationProperty, stop), asCALL_THISCALL);
    mgr->RegisterObjectMethod(ANIM_PROP_TYPE_NAME, "void setAnimation(AnimationResource@)", asMETHOD(AnimationProperty, setAnimationResource), asCALL_THISCALL);
}
