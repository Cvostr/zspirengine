#pragma once

#include "World.hpp"
#include "../game.h"

#include "../misc/misc.h"

namespace Engine {


class SkyboxProperty : public Engine::IGameObjectComponent {
public:
    void onPreRender(Engine::Renderer* pipeline);
    void DrawSky(Renderer* pipeline);
    SkyboxProperty();
};


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

class ColliderProperty : public Engine::PhysicalProperty{
public:
    void onObjectDeleted(); //unregister in world
    void addPropertyInterfaceToInspector();
    void copyTo(Engine::IGameObjectComponent* dest);
    void onUpdate(float deltaTime);
    void onStart();
    Engine::TransformProperty* getTransformProperty();

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);
    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

    ColliderProperty();
};

class RigidbodyProperty : public Engine::PhysicalProperty{
public:
    Vec3 gravity;
    Vec3 linearVel;
    Vec3 angularVel;

    void addPropertyInterfaceToInspector();
    void onUpdate(float deltaTime);
    void onStart();
    void copyTo(Engine::IGameObjectComponent* dest);
    void onValueChanged();
    void onObjectDeleted(); //unregister in world

    void setLinearVelocity(Vec3& lvel);
    void applyCentralImpulse(Vec3& imp);
    void applyCentralForce(Vec3& v);

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);
    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

    RigidbodyProperty();
};

class CharacterControllerProperty : public Engine::PhysicalProperty{
private:
    bool _isOnGround;
public:
    float width;
    float height;

    Vec3 gravity;
    Vec3 linearVel;
    Vec3 angularVel;

    void addPropertyInterfaceToInspector();
    void copyTo(Engine::IGameObjectComponent* dest);
    void onUpdate(float deltaTime);
    void onStart();

    void setLinearVelocity(Vec3 lvel);

    bool isOnGround();
    void jump(float height);
    void walk(float x, float z);

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);
    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

    CharacterControllerProperty();
};

class TriggerProperty : public Engine::PhysicalProperty {
private:
    SmArray<int>* _array;

public:
    btGhostObject* m_ghost;

    void addPropertyInterfaceToInspector();
    void onUpdate(float deltaTime);
    void copyTo(Engine::IGameObjectComponent* dest);
    void initGhost();
    void onObjectDeleted(); //unregister in world
    void onStart();

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);

    TriggerProperty();
};

}
