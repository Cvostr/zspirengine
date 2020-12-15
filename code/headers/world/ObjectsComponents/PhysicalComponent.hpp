#pragma once

#include "../World.hpp"

namespace Engine {

    class PhysicalProperty : public Engine::IGameObjectComponent {
    protected:

        bool init();
        bool updateCollisionShape();
        btTransform getBtTransform();
    public:
        bool isCustomPhysicalSize;
        Vec3 cust_size;
        Vec3 transform_offset;

        bool created;
        btRigidBody* rigidBody;
        btCollisionShape* shape;
        COLLIDER_TYPE coll_type;
        float mass;
        void copyTo(Engine::IGameObjectComponent* dest);
        void addColliderRadio();
        void addMassField();
        void addCustomSizeField();
        void onUpdate(float deltaTime);
        PhysicalProperty();
    };

}