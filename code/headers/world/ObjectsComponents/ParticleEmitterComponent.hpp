#pragma once

#include "../World.hpp"
#include "../../math/RGBColor.hpp"
#include "../../math/Vec3.hpp"
#include "../../math/Vec2.hpp"

namespace Engine {

    enum OverLifeCurve {
        OLC_DIRECT,
        OLC_INDIRECT
    };

    template <typename T>
    struct BeginEndValue {
        T Begin;
        T End;

        BeginEndValue() :
            Begin(0),
            End(10)
        {}

        BeginEndValue(T Value) :
            Begin(Value),
            End(Value)
        {
        }

        BeginEndValue(T BeginValue, T EndValue) :
            Begin(BeginValue),
            End(EndValue)
        {}
    };
    
    template <typename T>
    struct DeltaValue {
        T OriginalValue;

        float Add;
        float Mul;

        DeltaValue() :
            Add(0),
            Mul(1)
        {}

        DeltaValue(T Original) :
            OriginalValue(Original),
            Add(0),
            Mul(1) 
        {}

    };

    class Particle {
    public:
        Vec3 Position; //Current position of particle
        Vec3 Velocity; //Current velocity of particle
        Vec2 Size; // Current size of particle
        float mTimePassed; //Time, elapsed from creation

        Particle() :
            mTimePassed(0)
        {}
    };

    class ParticleEmitterComponent : public Engine::IGameObjectComponent {
    public:

        std::vector<Particle> mParticles;

        float mDuration; //single cycle duration
        bool mLooping; //Does particle system loop
        bool mPrewarm;
        float mLifetime; //Lifetime of single particle
        int32_t mMaxParticles; //Limit particles amount

        float mSpeed;
        DeltaValue<float> mSize;
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