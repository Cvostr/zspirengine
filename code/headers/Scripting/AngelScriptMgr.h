#pragma once

#include <angelscript/angelscript.h>
#include <angelscript/scriptbuilder.h>
#include "Types/StdString.hpp"
#include "Types/ASFile.hpp"
#include <angelscript/scripthandle.h>
#include <string>
#include "../engine/Resources.hpp"

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
#define FONTRESOURCE_TYPE_NAME "FontResource"
#define PREFABRESOURCE_TYPE_NAME "PrefabResource"

#define VEC3_TYPE_NAME "Vec3"
#define QUAT_TYPE_NAME "Quat"
#define RGBACOLOR_TYPE_NAME "RGBAColor"
#define PLANE_TYPE_NAME "Plane"
#define CAM_TYPE_NAME "Camera"
#define KEY_TYPE_NAME "Key"
#define MOUSE_STATE_NAME "MouseState"

#define FILE_STREAM_OBJ "File"
#define KV_FILE_OBJ "KeyValueFile"

#define TRANSFORM_PROP_TYPE_NAME "Transform"
#define LIGHTSOURCE_PROP_TYPE_NAME "LightSource"
#define AUDSOURCE_PROP_TYPE_NAME "AudioSource"
#define CHAR_CTRL_PROP_TYPE_NAME "CharacterController"
#define ANIM_PROP_TYPE_NAME "Animation"
#define MESH_PROP_TYPE_NAME "Mesh"
#define MAT_PROP_TYPE_NAME "Material"
#define RIGIDBODY_PROP_TYPE_NAME "Rigidbody"
#define COLLIDER_PROP_TYPE_NAME "Collider"
#define SHADOWCAST_PROP_TYPE_NAME "ShadowCaster"
#define CAMERA_PROP_TYPE_NAME "Camera"

#define AG_STRING 67108876
#define AG_VECTOR3 67108881
#define AG_QUAT 67108882
#define AG_RGB_COLOR 67108883

namespace Engine {
    class GameObject;

    struct ClassFieldDesc {
        unsigned int index;
        std::string name;
        int typeID;
        bool isPrivate;
        bool isProtected;

        ClassFieldDesc():
            index(0)
        {
        }
    };

    class ZPSClassDesc {
    public:
        std::string Name;
        asITypeInfo* Info;
        unsigned int ClassFieldsNum;
        std::vector<ClassFieldDesc> Fields;
        asIScriptObject* __SampleObject;

        ClassFieldDesc* GetSuitableDesc(std::string Name, int TypeID, unsigned int index);

        ZPSClassDesc():
            Name(""),
            Info(nullptr),
            ClassFieldsNum(0) {}
        ~ZPSClassDesc() {
            Fields.clear();
            __SampleObject->Release();
        }

    };

    int onInclude(const char* include, const char* from, CScriptBuilder* builder, void* userParam);

    class BytecodeStreamWriter : public asIBinaryStream {
    private:
        FILE* f;
    public:
        
        void open_for_write();
        void close() { fclose(f); }

        int Write(const void* ptr, asUINT size);
        int Read(void* ptr, asUINT size);
    };

    class AGScriptMgr {
    private:
        asIScriptEngine* mEngine;
        asIScriptContext* mContext;
        asIScriptModule* mModule; //Module of the script
        CScriptBuilder mBuilder;
        BytecodeStreamWriter* mStreamWrite;

        std::vector<ZPSClassDesc*> ZPSClassInfos;

        void UpdateClassesNames();
        void CreateModule();

        bool CompileError;
        bool ModuleCompiled;
    public:
        //Create angel script engine
        void create_Engine();
        void recreate_Engine();

        int SaveByteCode();
        bool HasCompilerErrors() { return CompileError; }

        asIScriptEngine* getAgScriptEngine();
        asIScriptContext* getAgScriptContext();
        asIScriptModule* getAgScriptModule();

        int RegisterInterface(const char* name);
        int RegisterObjectType(const char* name, int byteSize, asDWORD flags);
        int RegisterObjectMethod(const char* obj, std::string declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary = 0, int compositeOffset = 0, bool isCompositeIndirect = false);
        int RegisterGlobalProperty(std::string declaration, void* pointer);
        int RegisterObjectProperty(const char* obj, std::string declaration, int byteOffset, int compositeOffset = 0, bool isCompositeIndirect = false);

        int RegisterObjectBehaviour(const char* obj, asEBehaviours behaviour, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary = 0, int compositeOffset = 0, bool isCompositeIndirect = false);
        bool RegisterGlobalFunction(std::string func_name,
                                const asSFuncPtr& funcPointer,
                                asDWORD 	callConv,
                                void* auxiliary = 0);

        int RegisterEnum(const char* type);
        int RegisterEnumValue(const char* type, const char* name, int value);

        
        bool AddScriptFiles();
        int AddScriptFile(Engine::ScriptResource* res);

        bool CreateClass(std::string ClassName, Engine::GameObject* obj, asIScriptObject** ClassObj, asITypeInfo** ClassInfo);
        ZPSClassDesc* GetClassDesc(std::string ClassName);

        AGScriptMgr();
        ~AGScriptMgr();
    };

    void bindUiSDK(AGScriptMgr* mgr);
    void bindGameObjectSDK(AGScriptMgr* mgr);
    void bindWorldSDK(AGScriptMgr* mgr);
    void bindGameObjectPropertiesSDK(AGScriptMgr* mgr);
    void bindMathSDK(AGScriptMgr* mgr);
    void bindResourceSDK(const char* className, AGScriptMgr* mgr);
    void bindResourcesSDK(AGScriptMgr* mgr);
    void bindInputSDK(AGScriptMgr* mgr);
    void bindFilesSDK(AGScriptMgr* mgr);
    void bindResourceManagerSDK(AGScriptMgr* mgr);

    
    template <typename T>
    static void new_as_obj_T(T* ptr) {
        new (ptr) T();
    }
    template <typename T>
    static void del_as_obj_T(T* ptr) {
        ((T*)ptr)->~T();
    }
    template <typename T>
    static T* new_as_ref_T() {
        return new T;
    }
    template <typename T, typename U>
    static U* ag_cast(T* ptr) {
        return static_cast<U*>(ptr);
    }
 
}