#pragma once

#include "World.hpp"
#include "../audio/SoundSource.hpp"
#include "../game.h"
#include "Terrain.hpp"
#include "../Scripting/AngelScript.hpp"
#include "../misc/misc.h"

namespace Engine {
enum class LIGHTSOURCE_TYPE {
    LIGHTSOURCE_TYPE_NONE,
    LIGHTSOURCE_TYPE_DIRECTIONAL,
    LIGHTSOURCE_TYPE_POINT,
    LIGHTSOURCE_TYPE_SPOT
};

class MeshProperty : public IGameObjectComponent {
public:
    std::string resource_relpath; //Relative path to resource
    MeshResource* mesh_ptr; //Pointer to mesh
    bool castShadows;

    std::string rootNodeStr;
    GameObject* skinning_root_node;

    void updateMeshPtr(); //Updates pointer according to resource_relpath
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

class ZPScriptProperty : public Engine::IGameObjectComponent {
private:
    Engine::AGScript* script;
public:
    std::vector<ClassFieldValue*> mVars;
    std::string script_path;
    ScriptResource* script_res;
    Engine::AGScript* getScript();
    
    void SetScript(ScriptResource* script);

    void onValueChanged();
    void addPropertyInterfaceToInspector();
    void SetupScript();
    void onStart();
    void onStop();
    void onUpdate(float deltaTime); //calls update in scripts
    void copyTo(Engine::IGameObjectComponent* dest);
    bool makeFieldsList();

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);

    void onTrigger(Engine::GameObject* obj);
    void onTriggerEnter(Engine::GameObject* obj);
    void onTriggerExit(Engine::GameObject* obj);

    ZPScriptProperty();
    ~ZPScriptProperty();
};


class LightsourceProperty : public Engine::IGameObjectComponent {
public:
    LIGHTSOURCE_TYPE light_type; //type of lightsource

    Vec3 direction; //direction for directional & spotlight in quats
    //To compare differences
    Vec3 last_pos; //transform* last position
    Vec3 last_rot; //transform* last rotation

    ZSRGBCOLOR color; //Color of light
    float intensity; //Light's intensity
    float range; //Light's range
    float spot_angle;

    void addPropertyInterfaceToInspector();
    void copyTo(Engine::IGameObjectComponent* dest);
    void onPreRender(Engine::Renderer* pipeline);
    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);
    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

    LightsourceProperty();
};

class AudioSourceProperty : public Engine::IGameObjectComponent {
private:
    bool isPlaySheduled;
public:
    std::string resource_relpath; //Relative path to resource
    Engine::AudioResource* buffer_ptr;
    Engine::SoundSource source;

    Vec3 last_pos;

    void addPropertyInterfaceToInspector();
    void onValueChanged(); //Update soud buffer pointer and send source props
    void onUpdate(float deltaTime);
    void onObjectDeleted();
    void copyTo(Engine::IGameObjectComponent* dest);

    void setAudioResource(Engine::AudioResource* res);
    void updateAudioPtr();
    void audio_start();
    void audio_pause();
    void audio_stop();

    float getGain();
    float getPitch();
    void setGain(float gain);
    void setPitch(float pitch);

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);
    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

    AudioSourceProperty();
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

class ShadowCasterProperty : public Engine::IGameObjectComponent {
private:
    bool initialized;

    unsigned int mShadowBuffer;
    unsigned int mShadowDepthTexture;

    Mat4 LightProjectionMat;
    Mat4 LightViewMat[3];

    void reinitialize();
public:
    int TextureSize;
    float mShadowBias;
    float nearPlane;
    float farPlane;
    int projection_viewport;
    int mCascadesNum;

    void addPropertyInterfaceToInspector();
    void onPreRender(Engine::Renderer* pipeline);
    void copyTo(Engine::IGameObjectComponent* dest);
    void onValueChanged();
    void init();
    void onObjectDeleted();
    void Draw(Engine::Camera* cam, Renderer* pipeline);
    void setTexture();
    void setTextureSize(int TextureSize);
    void SetCascadesAmount(int CascadesNum);
    bool isRenderAvailable();

    //function to read shadowcaster property from memory
    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);
    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

    ShadowCasterProperty();
};

class MaterialProperty : public Engine::IGameObjectComponent {
private:
    std::string group_label;
public:
    //Pointer to picked material
    Material* material_ptr;
    //Path to material fil
    std::string material_path;
    //Draw shadows on object
    bool receiveShadows;

    void setMaterial(Material* mat);
    void _setMaterial(MaterialResource* mat);
    void setMaterial(std::string path);
    void addPropertyInterfaceToInspector();
    void onValueChanged();
    void copyTo(Engine::IGameObjectComponent* dest);
    void onRender(Engine::Renderer* pipeline);

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);

    MaterialProperty();
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

class TerrainProperty : public Engine::IGameObjectComponent {

private:
    TerrainData data;
    char edit_mode;

    btRigidBody* rigidBody;
public:
    Engine::UniformBuffer* terrainUniformBuffer;
    Engine::UniformBuffer* transformBuffer;

    std::vector<HeightmapTexturePair> textures;
    
    std::string file_label;
    int Width; //Width of terrain mesh
    int Length; //Height of terrain mesh
    int MaxHeight;
    
    bool castShadows;
    int textures_size;
    int grassType_size;

    int range; //Range of edit
    float editHeight; //Modifying height
    int textureid;
    int vegetableid;

    void addPropertyInterfaceToInspector();
    void onRender(Engine::Renderer* pipeline);

    void DrawMesh(Renderer* pipeline);
    void DrawGrass(Renderer* pipeline);
    void onValueChanged();
    void onAddToObject();
    void copyTo(Engine::IGameObjectComponent* dest);
    void onUpdate(float deltaTime);
    void onStart();
    
    void onMouseMotion(int posX, int posY, int screenY, bool isLeftButtonHold, bool isCtrlHold);
    void getPickedVertexId(int posX, int posY, int screenY, unsigned char* data);

    void modifyTerrainVertex(unsigned char* gl_data, bool isCtrlHold);

    TerrainData* getTerrainData();

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);

    TerrainProperty();
    ~TerrainProperty();
};

}
