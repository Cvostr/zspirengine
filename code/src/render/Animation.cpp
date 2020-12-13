#include "../../headers/render/Animation.hpp"
#include <cassert>

Engine::Animation::Animation() :
    NumChannels(0),
    TPS(0),
    channels(nullptr),
    duration(0) {}

Engine::Animation::~Animation(){
    delete[] this->channels;
}

Engine::AnimationChannel::AnimationChannel() :
    posKeysNum(0),
    rotationKeysNum(0),
    scaleKeysNum(0),
    //Set all pointers to nullptr
    pos(nullptr),
    rot(nullptr),
    scale(nullptr),

    rotTimes(nullptr),
    scaleTimes(nullptr),
    posTimes(nullptr)
{
    anim_ptr = nullptr;
}

Engine::AnimationChannel::~AnimationChannel(){
    delete[] this->pos;
    delete[] this->rot;
    delete[] this->scale;

    delete[] this->rotTimes;
    delete[] this->posTimes;
    delete[] this->scaleTimes;
}

Engine::AnimationChannel* Engine::Animation::getChannelByNodeName(std::string node_name){
    for(unsigned int i = 0; i < this->NumChannels; i ++){
        AnimationChannel* cha = &this->channels[i];
        if(cha->bone_name.compare(node_name) == false) return cha;
    }
    return nullptr;
}

unsigned int Engine::AnimationChannel::getPositionIndex(double Time){
    for(unsigned int i = 0; i < this->posKeysNum - 1; i ++){
        double _time = posTimes[i + 1];
        if(_time > Time){
            return i;
        }
    }
    //assert(0);
    return 0;
}
unsigned int Engine::AnimationChannel::getScaleIndex(double Time){
    for(unsigned int i = 0; i < this->scaleKeysNum - 1; i ++){
        double _time = scaleTimes[i + 1];
        if(_time > Time)
            return i;
    }
    //assert(0);
    return 0;
}
unsigned int Engine::AnimationChannel::getRotationIndex(double Time){
    for(unsigned int i = 0; i < this->rotationKeysNum - 1; i ++){
        double _time = rotTimes[i + 1];
        if(_time > Time){
            return i;
        }
    }
    //assert(0);
    return 0;
}

Vec3 Engine::AnimationChannel::getPostitionInterpolated(double Time){
    unsigned int index1 = getPositionIndex(Time);
    unsigned int index2 = index1 + 1;

    double delta = Time - posTimes[index1];
    delta /= anim_ptr->TPS;

    Vec3 v1 = pos[index1];
    Vec3 v2 = pos[index2];

    return lerp(v1, v2, static_cast<float>(delta));
}

Vec3 Engine::AnimationChannel::getScaleInterpolated(double Time){
    unsigned int index1 = getScaleIndex(Time);
    unsigned int index2 = index1 + 1;

    double delta = Time - scaleTimes[index1];
    delta /= anim_ptr->TPS;

    Vec3 v1 = scale[index1];
    Vec3 v2 = scale[index2];

    return lerp(v1, v2, static_cast<float>(delta));
}

ZSQUATERNION Engine::AnimationChannel::getRotationInterpolated(double Time){
    unsigned int index1 = getRotationIndex(Time);
    unsigned int index2 = index1 + 1;

    double delta = Time - posTimes[index1];
    delta /= anim_ptr->TPS;

    ZSQUATERNION q1 = rot[index1];
    ZSQUATERNION q2 = rot[index2];

    return slerp(q1, q2, static_cast<float>(delta));
}
