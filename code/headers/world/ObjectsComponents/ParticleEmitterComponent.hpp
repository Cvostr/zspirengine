#pragma once

#include "../World.hpp"
#include <math/RGBColor.hpp>
#include <math/Vec3.hpp>
#include <math/Vec2.hpp>

namespace Engine {

    enum ParticleEmitterShape {
        PE_SHAPE_NONE,
        PE_SHAPE_SPHERE,
        PE_SHAPE_BOX,
        PE_SHAPE_CONE
    };

    enum ParticleRotateMode {
        PRM_BILLBOARD,
        PRM_DIRECTION
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
    
    template<typename T>
    struct MinMaxValue {
        T Min;
        T Max;

        MinMaxValue() :
            Min(0),
            Max(10)
        {}

        MinMaxValue(T Value) :
            Min(Value),
            Max(Value)
        {
        }

        MinMaxValue(T BeginValue, T EndValue) :
            Min(BeginValue),
            Max(EndValue)
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

    class ParticleEmitterComponent;

    class Particle {
    public:
        bool mAlive;
        Vec3 Position; //Current position of particle
        Vec3 Velocity; //Current velocity of particle
        Vec2 Size; // Current size of particle
        float Rotation; //Rotation over Z of particle
        float RotationSpeed;
        RGBAColor Color; //Current color of particle
        float mTimePassed; //Time, elapsed from creation

        Particle() :
            mAlive(false),
            mTimePassed(0),
            Rotation(0),
            RotationSpeed(0)
        {}
    };

    class ParticleEmitterComponent : public Engine::IGameObjectComponent {
    private:
        //Array of particles
        std::vector<Particle*> mParticles;

        bool mSimulating;
        float mEmitterTime;
        float mSimulationTime;
    public:

        Engine::MeshResource* mParticleMesh;
        std::string mMeshResLabel;

        Engine::MaterialResource* mParticleMaterial;
        std::string mMaterialResLabel;

        
        //Shape of emitter
        ParticleEmitterShape mShape;
        float mDuration; //single cycle duration
        bool mLooping; //Does particle system loop
        bool mPrewarm; //Restart system on loop
        float mLifetime; //Lifetime of single particle
        int32_t mMaxParticles; //Limit particles amount
        MinMaxValue<int> mEmissionRate; //Particles per second emitted

        MinMaxValue<Vec3> mDirection;
        DeltaValue<MinMaxValue<Vec2>> mSize;
        MinMaxValue<float> mVelocity;
        Vec3 mConstantForce;
        float mDampingForce;
        MinMaxValue<float> mRotation;
        MinMaxValue<float> mRotationSpeed;

        DeltaValue<RGBAColor> Color;

        void GetParticlesTransforms(Mat4** Transforms, Camera& cam);

        void DestroyParticle(Particle* Particle);
        bool EmitNewParticle();
        void StepSimulation();
        void StartSimulation();
        void StopSimulation();
        void RestartSimulation();
        bool IsSimulating() {
            return mSimulating;
        }
        uint32_t GetFreeParticleIndex();
        uint32_t GetParticlesCount() {
            return static_cast<uint32_t>(mParticles.size());
        }
        uint32_t GetAliveParticlesCount();

        void GetNewParticleVelocityPos(Vec3& Velocity, Vec3& Pos);

        Vec3 GetRandomDirection();
        Vec2 GetRandomSize();
        float GetRandomVelocity();
        float GetRandomRotation();
        float GetRandomRotationSpeed();
        int GetRandomEmissionRate();

        float GetRandomFloat(float max);

        void updateMeshPtr(); //Updates pointer according to resource_relpath
        void addPropertyInterfaceToInspector();
        void onValueChanged(); //Update mesh    pointer
        void onUpdate(float deltaTime);
        void copyTo(Engine::IGameObjectComponent* dest);
        void onStart();

        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);
        void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);


        ParticleEmitterComponent();
    };
}