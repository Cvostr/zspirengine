#ifndef ZSANIMATION_H
#define ZSANIMATION_H

#include <string>
#include "zs-math.h"

namespace ZSPIRE {

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

    ZSVECTOR3* pos;
    ZSVECTOR3* scale;
    ZSQUATERNION* rot;

    double* posTimes;
    double* scaleTimes;
    double* rotTimes;

    unsigned int getPositionIndex(double Time);
    unsigned int getScaleIndex(double Time);
    unsigned int getRotationIndex(double Time);

    ZSVECTOR3 getPostitionInterpolated(double Time);
    ZSVECTOR3 getScaleInterpolated(double Time);
    ZSQUATERNION getRotationInterpolated(double Time);

    ZSVECTOR3 getPostitionInterpolated(unsigned int frame);
    ZSVECTOR3 getScaleInterpolated(unsigned int frame);
    ZSQUATERNION getRotationInterpolated(unsigned int frame);

    AnimationChannel();
    ~AnimationChannel();
};

}

#endif // ZSANIMATION_H
