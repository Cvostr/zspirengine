#pragma once

#include "World.hpp"
#include "../game.h"

#include "../misc/misc.h"

namespace Engine {

class MeshProperty : public IGameObjectComponent {
public:
    std::string resource_relpath; //Relative path to resource
    MeshResource* mesh_ptr; //Pointer to mesh
    bool castShadows;

    std::string rootNodeStr;
    GameObject* skinning_root_node;

    void updateMeshPtr(); //Updates pointer according to resource_relpath
    void updateRootNodePtr();
    void copyTo(IGameObjectComponent* dest);
    void onRender(Engine::Renderer* pipeline);
    void setMeshResource(MeshResource* resource);

    //EDITOR STUFF
    void addPropertyInterfaceToInspector();
    void onValueChanged(); //Update mesh    pointer
    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);
    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

    MeshProperty();
};


class NodeProperty : public IGameObjectComponent {
public:
    std::string node_label;

    Vec3 translation;
    Vec3 scale;
    ZSQUATERNION rotation;
    //Node transform from file
    Mat4 transform_mat;
    //Caclulated node transform
    Mat4 abs;

    void copyTo(IGameObjectComponent* dest);
   // void addPropertyInterfaceToInspector();
    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);

    NodeProperty();
};

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

class AnimationProperty : public Engine::IGameObjectComponent {
private:
public:
    bool Playing;
    double start_sec;

    Engine::AnimationResource* anim_prop_ptr;
    std::string anim_label;

    void addPropertyInterfaceToInspector();
    void onPreRender(Engine::Renderer* pipeline);
    void onValueChanged();
    void copyTo(Engine::IGameObjectComponent* dest);

    void play();
    void stop();

    void setAnimationResource(Engine::AudioResource* anim);

    void updateAnimationPtr();
    void updateNodeTransform(GameObject* obj, Mat4 parent);

    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);
    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);

    AnimationProperty();
};

}
