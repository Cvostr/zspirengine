#ifndef go_properties
#define go_properties

#include "World.h"
#include "../misc/oal_manager.h"
#include "../game.h"
#include "terrain.h"
#include "../Scripting/AngelScript.hpp"
#include "../misc/misc.h"

namespace Engine {
enum class LIGHTSOURCE_TYPE {
    LIGHTSOURCE_TYPE_NONE,
    LIGHTSOURCE_TYPE_DIRECTIONAL,
    LIGHTSOURCE_TYPE_POINT,
    LIGHTSOURCE_TYPE_SPOT
};

class MeshProperty : public GameObjectProperty{
public:
    std::string resource_relpath; //Relative path to resource
    MeshResource* mesh_ptr; //Pointer to mesh
    bool castShadows;

    std::string rootNodeStr;
    GameObject* skinning_root_node;

    void updateMeshPtr(); //Updates pointer according to resource_relpath
    void copyTo(GameObjectProperty* dest);
    void onRender(Engine::RenderPipeline* pipeline);
    void setMeshResource(MeshResource* resource);

    //EDITOR STUFF
    void addPropertyInterfaceToInspector();
    void onValueChanged(); //Update mesh    pointer
    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(std::ofstream* stream, GameObject* obj);
    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

    MeshProperty();
};

class ZPScriptProperty : public Engine::GameObjectProperty {
private:
    Engine::AGScript* script;
public:
    std::vector<GlobVarHandle*> vars;
    std::string script_path;
    ScriptResource* script_res;
    void onValueChanged();
    void addPropertyInterfaceToInspector();
    void onStart();
    void onStop();
    void onUpdate(float deltaTime); //calls update in scripts
    void copyTo(Engine::GameObjectProperty* dest);
    void makeGlobalVarsList();

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(std::ofstream* stream, GameObject* obj);

    void onTrigger(Engine::GameObject* obj);
    void onTriggerEnter(Engine::GameObject* obj);
    void onTriggerExit(Engine::GameObject* obj);

    ZPScriptProperty();
    ~ZPScriptProperty();
};


class LightsourceProperty : public Engine::GameObjectProperty{
public:
    LIGHTSOURCE_TYPE light_type; //type of lightsource

    ZSVECTOR3 direction; //direction for directional & spotlight in quats
    //To compare differences
    ZSVECTOR3 last_pos; //transform* last position
    ZSVECTOR3 last_rot; //transform* last rotation

    ZSRGBCOLOR color; //Color of light
    float intensity; //Light's intensity
    float range; //Light's range
    float spot_angle;

    void addPropertyInterfaceToInspector();
    void copyTo(Engine::GameObjectProperty* dest);
    void onPreRender(Engine::RenderPipeline* pipeline);
    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(std::ofstream* stream, GameObject* obj);
    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

    LightsourceProperty();
};

class AudioSourceProperty : public Engine::GameObjectProperty{
private:
    bool isPlaySheduled;
public:
    std::string resource_relpath; //Relative path to resource
    Engine::AudioResource* buffer_ptr;
    Engine::SoundSource source;

    ZSVECTOR3 last_pos;

    void addPropertyInterfaceToInspector();
    void onValueChanged(); //Update soud buffer pointer and send source props
    void onUpdate(float deltaTime);
    void onObjectDeleted();
    void copyTo(Engine::GameObjectProperty* dest);

    void setAudioFile(std::string relpath);
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
    void savePropertyToStream(std::ofstream* stream, GameObject* obj);
    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

    AudioSourceProperty();
};


class NodeProperty : public GameObjectProperty {
public:
    std::string node_label;

    ZSVECTOR3 translation;
    ZSVECTOR3 scale;
    ZSQUATERNION rotation;
    //Node transform from file
    ZSMATRIX4x4 transform_mat;
    //Caclulated node transform
    ZSMATRIX4x4 abs;

    void copyTo(GameObjectProperty* dest);
   // void addPropertyInterfaceToInspector();
    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(std::ofstream* stream, GameObject* obj);

    NodeProperty();
};

class SkyboxProperty : public Engine::GameObjectProperty{
public:
    void onPreRender(Engine::RenderPipeline* pipeline);
    void DrawSky(RenderPipeline* pipeline);
    SkyboxProperty();
};

class ShadowCasterProperty : public Engine::GameObjectProperty{
private:
    bool initialized;
    unsigned int shadowBuffer;
    unsigned int shadowDepthTexture;

    ZSMATRIX4x4 LightProjectionMat;
    ZSMATRIX4x4 LightViewMat[3];
public:
    int TextureWidth;
    int TextureHeight;
    float shadow_bias;
    float nearPlane;
    float farPlane;
    int projection_viewport;

    void addPropertyInterfaceToInspector();
    void onPreRender(Engine::RenderPipeline* pipeline);
    void copyTo(Engine::GameObjectProperty* dest);
    void onValueChanged();
    void init();
    void onObjectDeleted();
    void Draw(Engine::Camera* cam, RenderPipeline* pipeline);
    void setTexture();
    void setTextureSize();
    bool isRenderAvailable();
    //function to read shadowcaster property from memory
    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(std::ofstream* stream, GameObject* obj);

    ShadowCasterProperty();
};

class MaterialProperty : public Engine::GameObjectProperty{
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
    void copyTo(Engine::GameObjectProperty* dest);
    void onRender(Engine::RenderPipeline* pipeline);

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);
    void savePropertyToStream(std::ofstream* stream, GameObject* obj);

    MaterialProperty();
};


class PhysicalProperty : public Engine::GameObjectProperty{
protected:

    bool init();
    bool updateCollisionShape();
    btTransform getBtTransform();
public:
    bool isCustomPhysicalSize;
    ZSVECTOR3 cust_size;
    ZSVECTOR3 transform_offset;

    bool created;
    btRigidBody* rigidBody;
    btCollisionShape* shape;
    COLLIDER_TYPE coll_type;
    float mass;
    void copyTo(Engine::GameObjectProperty* dest);
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
    void copyTo(Engine::GameObjectProperty* dest);
    void onUpdate(float deltaTime);
    void onStart();
    Engine::TransformProperty* getTransformProperty();

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(std::ofstream* stream, GameObject* obj);

    ColliderProperty();
};

class RigidbodyProperty : public Engine::PhysicalProperty{
public:
    ZSVECTOR3 gravity;
    ZSVECTOR3 linearVel;
    ZSVECTOR3 angularVel;

    void addPropertyInterfaceToInspector();
    void onUpdate(float deltaTime);
    void onStart();
    void copyTo(Engine::GameObjectProperty* dest);
    void onValueChanged();
    void onObjectDeleted(); //unregister in world

    void setLinearVelocity(ZSVECTOR3 lvel);

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(std::ofstream* stream, GameObject* obj);

    RigidbodyProperty();
};

class CharacterControllerProperty : public Engine::PhysicalProperty{
private:
    bool _isOnGround;
public:
    float width;
    float height;

    ZSVECTOR3 gravity;
    ZSVECTOR3 linearVel;
    ZSVECTOR3 angularVel;

    void addPropertyInterfaceToInspector();
    void copyTo(Engine::GameObjectProperty* dest);
    void onUpdate(float deltaTime);
    void onStart();

    void setLinearVelocity(ZSVECTOR3 lvel);

    bool isOnGround();
    void jump(float height);
    void walk(float x, float z);

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(std::ofstream* stream, GameObject* obj);
    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

    CharacterControllerProperty();
};

class TriggerProperty : public Engine::PhysicalProperty {
private:
    
public:
    btGhostObject* m_ghost;

    void addPropertyInterfaceToInspector();
    void onUpdate(float deltaTime);
    void copyTo(Engine::GameObjectProperty* dest);
    void initGhost();
    void onObjectDeleted(); //unregister in world
    void onStart();

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(std::ofstream* stream, GameObject* obj);

    TriggerProperty();
};

class AnimationProperty : public Engine::GameObjectProperty {
private:
public:
    bool Playing;
    double start_sec;

    Engine::AnimationResource* anim_prop_ptr;
    std::string anim_label;

    void addPropertyInterfaceToInspector();
    void onPreRender(Engine::RenderPipeline* pipeline);
    void onValueChanged();
    void copyTo(Engine::GameObjectProperty* dest);

    void play();
    void stop();
    void setAnimation(std::string anim);
    void setAnimationResource(Engine::AudioResource* anim);

    void updateAnimationPtr();
    void updateNodeTransform(GameObject* obj, ZSMATRIX4x4 parent);

    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);
    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(std::ofstream* stream, GameObject* obj);

    AnimationProperty();
};

class TerrainProperty : public Engine::GameObjectProperty{

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
    void onRender(Engine::RenderPipeline* pipeline);

    void DrawMesh(RenderPipeline* pipeline);
    void DrawGrass(RenderPipeline* pipeline);
    void onValueChanged();
    void onAddToObject();
    void copyTo(Engine::GameObjectProperty* dest);
    void onUpdate(float deltaTime);
    void onStart();
    
    void onMouseMotion(int posX, int posY, int screenY, bool isLeftButtonHold, bool isCtrlHold);
    void getPickedVertexId(int posX, int posY, int screenY, unsigned char* data);

    void modifyTerrainVertex(unsigned char* gl_data, bool isCtrlHold);

    TerrainData* getTerrainData();

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(std::ofstream* stream, GameObject* obj);

    TerrainProperty();
    ~TerrainProperty();
};

}

#endif
