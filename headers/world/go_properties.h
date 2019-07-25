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

typedef uint8_t ZSLIGHTSOURCE_GL_ID;

class MeshProperty : public GameObjectProperty{
public:
    std::string resource_relpath; //Relative path to resource
    MeshResource* mesh_ptr; //Pointer to mesh

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

    void copyTo(GameObjectProperty* dest);
    void updTransformPtr();
    void onObjectDeleted();
    void onPreRender(RenderPipeline* pipeline);

    LightsourceProperty();
};

class AudioSourceProperty : public GameObjectProperty{
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

    AudioSourceProperty();
};

}

#endif
