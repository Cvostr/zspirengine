#pragma once

#include "../World.hpp"
#include "../../math/RGBColor.hpp"
#include "../../math/Vec3.hpp"

namespace Engine {

    class Particle {
    public:
        Vec3 Position;
        Vec3 Velocity;
        float mTimePassed;

        Particle() :
            mTimePassed(0)
        {}
    };

    class ParticleEmitterComponent : public Engine::IGameObjectComponent {
    public:

        float mDuration; //single cycle duration
        bool mLooping; //Does 
        bool mPrewarm;
        float mLifetime; //Lifetime of single particle
        int32_t mMaxParticles; //Limit particles amount

        float mStartSpeed;
        float mStartSize;
        float mStartRotation;

        struct {
            bool mEnabled;
            RGBAColor mStartColor;
            RGBAColor mEndColor;
        }ColorOverLifetime;

        struct {
            Vec3 mDirection;
        }VelocityOverLifetime;

        void addPropertyInterfaceToInspector();
        void onUpdate(float deltaTime);
        void copyTo(Engine::IGameObjectComponent* dest);
        void onObjectDeleted(); //unregister in world
        void onStart();

        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);


        ParticleEmitterComponent();
    };
}