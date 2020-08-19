#pragma once

#include <angelscript/angelscript.h>
#include <angelscript/scriptstdstring.h>
#include <angelscript/scripthandle.h>
#include <string>

#define GAME_OBJECT_TYPE_NAME "GameObject"
#define GAME_OBJECT_PROP_TYPE_NAME "GameObjectProperty"
#define VEC3_TYPE_NAME "Vec3"
#define RGBCOLOR_TYPE_NAME "rgbColor"

#define TRANSFORM_PROP_TYPE_NAME "Transform"
#define LIGHTSOURCE_PROP_TYPE_NAME "LightSource"
#define AUDSOURCE_PROP_TYPE_NAME "AudioSource"

namespace Engine {
    class AGScriptMgr {
    private:
        asIScriptEngine* ag_engine;
        asIScriptContext* ag_context;
    public:

        asIScriptEngine* getAgScriptEngine();
        asIScriptContext* getAgScriptContext();

        int RegisterInterface(const char* name);
        int RegisterObjectType(const char* name, int byteSize, asDWORD flags);
        int RegisterObjectMethod(const char* obj, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary = 0, int compositeOffset = 0, bool isCompositeIndirect = false);
        int RegisterGlobalProperty(const char* declaration, void* pointer);
        int RegisterObjectProperty(const char* obj, const char* declaration, int byteOffset, int compositeOffset = 0, bool isCompositeIndirect = false);

        int RegisterObjectBehaviour(const char* obj, asEBehaviours behaviour, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary = 0, int compositeOffset = 0, bool isCompositeIndirect = false);
        bool RegisterGlobalFunction(std::string func_name,
                                const asSFuncPtr& funcPointer,
                                asDWORD 	callConv,
                                void* auxiliary = 0);

        asIScriptModule* GetModule(std::string module,
            asEGMFlags 	flag = asGM_ONLY_IF_EXISTS
        );

        AGScriptMgr();
        ~AGScriptMgr();
    };
    void bindGameObjectSDK(AGScriptMgr* mgr);
    void bindGameObjectPropertiesSDK(AGScriptMgr* mgr);
    void bindMathSDK(AGScriptMgr* mgr);
    template<class T>
    void bindGameObjectPropertySDK(AGScriptMgr* mgr, const char* obj_type) {
        int result = 0;
        
        T* prop = new T;
        prop->bindObjectPropertyToAngel(mgr);

        result = mgr->RegisterObjectMethod(obj_type, "void setActive(bool)", asMETHOD(T, setActive), asCALL_THISCALL);
        
    }


}