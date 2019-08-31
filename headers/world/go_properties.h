#ifndef go_properties
#define go_properties

#include "World.h"
#include "../Scripting/LuaScript.h"
#include "../misc/oal_manager.h"

namespace Engine {

enum LIGHTSOURCE_TYPE {
    LIGHTSOURCE_TYPE_NONE,
    LIGHTSOURCE_TYPE_DIRECTIONAL,
    LIGHTSOURCE_TYPE_POINT
};

class MeshProperty : public GameObjectProperty{
public:
    std::string resource_relpath; //Relative path to resource
    MeshResource* mesh_ptr; //Pointer to mesh
    bool castShadows;

    void updateMeshPtr(); //Updates pointer according to resource_relpath
    void copyTo(GameObjectProperty* dest);
    MeshProperty();
};

class ScriptGroupProperty : public GameObjectProperty {
public:
    int scr_num;
    std::vector<ObjectScript> scripts_attached;

    void wakeUp(); //on scene startup
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

    void onValueChanged(); //Update soud buffer pointer and send source props
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

class MaterialProperty : public GameObjectProperty{
public:
    //Pointer to picked material
    //Material* material_ptr;
    //Path to material fil
    std::string material_path;

    void onValueChanged();
    void copyTo(GameObjectProperty* dest);
    void onRender(RenderPipeline* pipeline);

    MaterialProperty();
};

class RigidbodyProperty : public GameObjectProperty{
public:

    ZSVECTOR3 speed;

    float mass;
    bool hasGravity;

    void onUpdate(float deltaTime);
    void copyTo(GameObjectProperty* dest);

    RigidbodyProperty();
};

}

#endif
