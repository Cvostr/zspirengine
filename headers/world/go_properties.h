#ifndef go_properties
#define go_properties

#include "World.h"
#include "../Scripting/LuaScript.h"
#include "../misc/oal_manager.h"
#include "../game.h"

namespace Engine {

enum LIGHTSOURCE_TYPE {
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

    //EDITOR STUFF
    void addPropertyInterfaceToInspector();
    void onValueChanged(); //Update mesh    pointer

    MeshProperty();
};

class ScriptGroupProperty : public GameObjectProperty {
public:
    int scr_num;
    std::vector<ObjectScript> scripts_attached;

    void onUpdate(float deltaTime); //calls update in scripts
    ObjectScript* getScriptByName(std::string name);

    ScriptGroupProperty();
};

class LightsourceProperty : public GameObjectProperty{
public:
    LIGHTSOURCE_TYPE light_type; //type of lightsource
    TransformProperty* transform; //pointer to object's transform

    ZSVECTOR3 direction; //direction for directional & spotlight in quats
    //To compare differences
    ZSVECTOR3 last_pos; //transform* last position
    ZSVECTOR3 last_rot; //transform* last rotation

    ZSRGBCOLOR color; //Color of light
    float intensity; //Light's intensity
    float range; //Light's range
    float spot_angle;

    void copyTo(GameObjectProperty* dest);
    void updTransformPtr();
    void onPreRender(RenderPipeline* pipeline);

    LightsourceProperty();
};

class AudioSourceProperty : public GameObjectProperty{
private:
    bool isPlaySheduled;
public:
    std::string resource_relpath; //Relative path to resource
    AudioResource* buffer_ptr;
    SoundSource source;

    ZSVECTOR3 last_pos;

    void onUpdate(float deltaTime);
    void onObjectDeleted();
    void copyTo(GameObjectProperty* dest);

    void updateAudioPtr();
    void audio_start();
    void audio_stop();
    void audio_pause();

    float getGain();
    float getPitch();
    void setGain(float gain);
    void setPitch(float pitch);

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

    NodeProperty();
};

class MaterialProperty : public GameObjectProperty{
public:
    //Pointer to picked material
    MaterialResource* material_ptr;
    //Path to material fil
    std::string material_path;

    //Draw shadows on object
    bool receiveShadows;

    void onValueChanged();
    void copyTo(GameObjectProperty* dest);
    void onRender(RenderPipeline* pipeline);

    MaterialProperty();
};

class PhysicalProperty : public Engine::GameObjectProperty{
protected:

    void init();
    void updateCollisionShape();
public:
    bool isCustomPhysicalSize;
    ZSVECTOR3 cust_size;

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
    Engine::TransformProperty* getTransformProperty();

    bool isTrigger;

    ColliderProperty();
};

class RigidbodyProperty : public Engine::PhysicalProperty{
public:
    ZSVECTOR3 gravity;
    ZSVECTOR3 linearVel;
    ZSVECTOR3 angularVel;

    void addPropertyInterfaceToInspector();
    void onUpdate(float deltaTime);
    void copyTo(Engine::GameObjectProperty* dest);
    void onValueChanged();

    void setLinearVelocity(ZSVECTOR3 lvel);

    RigidbodyProperty();
};

class AnimationProperty : public GameObjectProperty {
private:
public:
    bool Playing;
    double start_sec;

    Engine::AnimationResource* anim_prop_ptr;
    std::string anim_label;

    void onPreRender(RenderPipeline* pipeline);
    void onValueChanged();
    void copyTo(GameObjectProperty* dest);

    void play();
    void stop();
    void setAnimation(std::string anim);

    void updateAnimationPtr();
    void updateNodeTransform(GameObject* obj, ZSMATRIX4x4 parent);


    AnimationProperty();
};


}

#endif
