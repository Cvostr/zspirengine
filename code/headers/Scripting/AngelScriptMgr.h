#pragma once

#include <angelscript/angelscript.h>
#include "Types/StdString.hpp"
#include <angelscript/scripthandle.h>
#include <string>

#define GAME_OBJECT_TYPE_NAME "GameObject"
#define WORLD_TYPE_NAME "World"
#define GAME_OBJECT_PROP_TYPE_NAME "GameObjectProperty"

#define RESOURCE_TYPE_NAME "Resource"
#define RESOURCEMAN_TYPE_NAME "ResourceManager"
#define TEXRESOURCE_TYPE_NAME "TextureResource"
#define MESHRESOURCE_TYPE_NAME "MeshResource"
#define AUDRESOURCE_TYPE_NAME "AudioResource"
#define ANIMRESOURCE_TYPE_NAME "AnimationResource"
#define MATRESOURCE_TYPE_NAME "MaterialResource"

#define VEC3_TYPE_NAME "Vec3"
#define QUAT_TYPE_NAME "Quat"
#define RGBCOLOR_TYPE_NAME "rgbColor"
#define CAM_TYPE_NAME "Camera"
#define KEY_TYPE_NAME "Key"
#define MOUSE_STATE_NAME "MouseState"

#define FILE_STREAM_OBJ "File"
#define KV_FILE_OBJ "KeyValFile"

#define TRANSFORM_PROP_TYPE_NAME "Transform"
#define LIGHTSOURCE_PROP_TYPE_NAME "LightSource"
#define AUDSOURCE_PROP_TYPE_NAME "AudioSource"
#define CHAR_CTRL_PROP_TYPE_NAME "CharacterController"
#define ANIM_PROP_TYPE_NAME "Animation"
#define MESH_PROP_TYPE_NAME "Mesh"
#define MAT_PROP_TYPE_NAME "Material"
#define RIGIDBODY_PROP_TYPE_NAME "Rigidbody"

namespace Engine {
    class AGScriptMgr {
    private:
        asIScriptEngine* ag_engine;
        asIScriptContext* ag_context;
    public:

        void create_Engine();
        void recreate_Engine();

        asIScriptEngine* getAgScriptEngine();
        asIScriptContext* getAgScriptContext();

        int RegisterInterface(const char* name);
        int RegisterObjectType(const char* name, int byteSize, asDWORD flags);
        int RegisterObjectMethod(const char* obj, std::string declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary = 0, int compositeOffset = 0, bool isCompositeIndirect = false);
        int RegisterGlobalProperty(std::string declaration, void* pointer);
        int RegisterObjectProperty(const char* obj, const char* declaration, int byteOffset, int compositeOffset = 0, bool isCompositeIndirect = false);

        int RegisterObjectBehaviour(const char* obj, asEBehaviours behaviour, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary = 0, int compositeOffset = 0, bool isCompositeIndirect = false);
        bool RegisterGlobalFunction(std::string func_name,
                                const asSFuncPtr& funcPointer,
                                asDWORD 	callConv,
                                void* auxiliary = 0);

        int RegisterEnum(const char* type);
        int RegisterEnumValue(const char* type, const char* name, int value);

        asIScriptModule* GetModule(std::string module,
            asEGMFlags 	flag = asGM_ONLY_IF_EXISTS
        );

        AGScriptMgr();
        ~AGScriptMgr();
    };
    void bindGameObjectSDK(AGScriptMgr* mgr);
    void bindWorldSDK(AGScriptMgr* mgr);
    void bindGameObjectPropertiesSDK(AGScriptMgr* mgr);
    void bindMathSDK(AGScriptMgr* mgr);
    void bindResourceSDK(const char* className, AGScriptMgr* mgr);
    void bindResourcesSDK(AGScriptMgr* mgr);
    void bindInputSDK(AGScriptMgr* mgr);
    void bindFilesSDK(AGScriptMgr* mgr);
    void bindResourceManagerSDK(AGScriptMgr* mgr);

    template<class T>
    void bindGameObjectPropertySDK(AGScriptMgr* mgr, const char* obj_type) {
        int result = 0;
        
        T* prop = new T;
        prop->bindObjectPropertyToAngel(mgr);

        result = mgr->RegisterObjectMethod(obj_type, "void setActive(bool)", asMETHOD(T, setActive), asCALL_THISCALL);
    }
    template <typename T>
    static void new_as_obj_T(T* ptr) {
        new (ptr) T();
    }
    template <typename T>
    static void del_as_obj_T(T* ptr) {
        ((T*)ptr)->~T();
    }
    template <typename T>
    static T* new_as_ref_T(T* ptr) {
        return new T;
    }
}