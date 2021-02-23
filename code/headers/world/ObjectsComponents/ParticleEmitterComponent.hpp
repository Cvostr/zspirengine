#pragma once

#include "../World.hpp"
#include "../../math/RGBColor.hpp"
#include "../../math/Vec3.hpp"
#include "../../math/Vec2.hpp"

namespace Engine {

    enum ParticleEmitterShape {
        PE_SHAPE_NONE,
        PE_SHAPE_SPHERE,
        PE_SHAPE_BOX,
        PE_SHAPE_CONE
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
        RGBAColor Color; //Current color of particle
        float mTimePassed; //Time, elapsed from creation

        Particle() :
            mTimePassed(0)
        {}
    };

    class ParticleEmitterComponent : public Engine::IGameObjectComponent {
    public:

        std::vector<Particle> mParticles;

        ParticleEmitterShape mShape;
        float mDuration; //single cycle duration
        bool mLooping; //Does particle system loop
        bool mPrewarm;
        float mLifetime; //Lifetime of single particle
        int32_t mMaxParticles; //Limit particles amount

        DeltaValue<float> mVelocity;
        DeltaValue<float> mSize;
        float mStartRotation;

        DeltaValue<RGBAColor> Color;

        bool mSimulating;

        void StepSimulation();
        void StartSimulation();
        void StopSimulation();

        void addPropertyInterfaceToInspector();
        void onUpdate(float deltaTime);
        void copyTo(Engine::IGameObjectComponent* dest);
        void onStart();

        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);


        ParticleEmitterComponent();
    };
}