#include "../../../headers/world/go_properties.h"

extern ZSGAME_DATA* game_data;

Engine::AnimationProperty::AnimationProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_ANIMATION;
    this->anim_label = "@none";
    anim_prop_ptr = nullptr;
    Playing = false;
    start_sec = 0;
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

void Engine::AnimationProperty::onPreRender(RenderPipeline* pipeline) {
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
            NodeProperty* prop = node->getPropertyPtr<NodeProperty>();

            prop->translation = ch->getPostitionInterpolated(animTime);
            prop->scale = ch->getScaleInterpolated(animTime);
            prop->rotation = ch->getRotationInterpolated(animTime);
        }
    }
    ZSMATRIX4x4 identity_matrix = getIdentity();
    updateNodeTransform(obj, identity_matrix);
}

void Engine::AnimationProperty::updateNodeTransform(GameObject* obj, ZSMATRIX4x4 parent) {

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
            ZSMATRIX4x4 transl = transpose(getTranslationMat(prop->translation));
            ZSMATRIX4x4 _sca = transpose(getScaleMat(prop->scale));
            ZSMATRIX4x4 rot = (getRotationMat(prop->rotation));
            //Multiply all matrices
            prop->abs = transl * rot * _sca;
        }
    }
    //Apply parent transform
    prop->abs = parent * prop->abs;
    //Go deeper in tree
    for (unsigned int i = 0; i < obj->children.size(); i++) {
        updateNodeTransform(obj->children[i].updLinkPtr(), prop->abs);
    }
}

void Engine::AnimationProperty::copyTo(GameObjectProperty* dest) {
    if (dest->type != this->type) return; //if it isn't animation

    //Do base things
    GameObjectProperty::copyTo(dest);
    AnimationProperty* _dest = static_cast<AnimationProperty*>(dest);

    _dest->anim_label = this->anim_label;
    _dest->anim_prop_ptr = this->anim_prop_ptr;
}

void Engine::AnimationProperty::setAnimation(std::string anim) {
    this->anim_label = anim;
    updateAnimationPtr();
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