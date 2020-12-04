#pragma once

#include <string>
#include "Math.hpp"

namespace Engine {

class AnimationChannel;

class Animation{
public:
    double duration;
    double TPS;
    std::string name;

    //Amount of animation channels
    unsigned int NumChannels;
    //Array of animation channels
    AnimationChannel* channels;
    //Get pointer to channel by specified node name
    AnimationChannel* getChannelByNodeName(std::string node_name);

    Animation();
    ~Animation();

};

class AnimationChannel{
public:
    std::string bone_name;
    Animation* anim_ptr;
    //Number of position keys in animation channel
    unsigned int posKeysNum;
    //Number of scale keys in animation channel
    unsigned int scaleKeysNum;
    //Number of rotation keys in animation channel
    unsigned int rotationKeysNum;

    Vec3* pos;
    Vec3* scale;
    ZSQUATERNION* rot;

    double* posTimes;
    double* scaleTimes;
    double* rotTimes;

    unsigned int getPositionIndex(double Time);
    unsigned int getScaleIndex(double Time);
    unsigned int getRotationIndex(double Time);

    Vec3 getPostitionInterpolated(double Time);
    Vec3 getScaleInterpolated(double Time);
    ZSQUATERNION getRotationInterpolated(double Time);

    AnimationChannel();
    ~AnimationChannel();
};

}