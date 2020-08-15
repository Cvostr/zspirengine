#ifndef ANGEL_SCRIPT_HPP
#define ANGEL_SCRIPT_HPP

#include <angelscript/angelscript.h>
#include <angelscript/scriptbuilder.h>

#include "../world/World.h"
#include "AngelScriptMgr.h"
#include "../engine/resources.h"
#include <string>
#include <cstdint>
#include <iostream>

#define AG_STRING 67108876
#define AG_VECTOR3 1140850705

namespace Engine{

    class GlobVarHandle {
    protected:
        unsigned int size;
        bool isRef;
    public:
        unsigned int index;
        std::string name;
        std::string _namespace;
        void* address;
        int typeID;

        virtual void applyValue() {
            //memcpy(address, &value, size);
        }
        virtual void getValue() {
            //memcpy(&value, address, size);
        }

        GlobVarHandle() {
            size = 0;
            isRef = false;
            index = 0;
            name = "";
            address = nullptr;
            typeID = 0;
        }
    };

    class GVH_INT32 : public GlobVarHandle {
    public:
        int32_t value;

        void applyValue() {
            memcpy(address, &value, size);
        }
        void getValue() {
            memcpy(&value, address, size);
        }

        GVH_INT32() {
            size = sizeof(int);
            isRef = false;
            value = 0;
        }
    };

    class GVH_FLOAT : public GlobVarHandle {
    public:
        float value;

        void applyValue() {
            memcpy(address, &value, size);
        }
        void getValue() {
            memcpy(&value, address, size);
        }

        GVH_FLOAT() {
            size = sizeof(float);
            isRef = false;
            value = 0.f;
        }
    };

    class GVH_BOOL : public GlobVarHandle {
    public:
        bool value;

        void applyValue() {
            memcpy(address, &value, size);
        }
        void getValue() {
            memcpy(&value, address, size);
        }

        GVH_BOOL() {
            size = sizeof(bool);
            isRef = false;
            value = false;
        }
    };

    class GVH_VEC3 : public GlobVarHandle {
    public:
        ZSVECTOR3 value;

        void applyValue() {
            
        }
        void getValue();

        GVH_VEC3() {
            size = sizeof(ZSVECTOR3);
            isRef = true;
            value = ZSVECTOR3(0, 0, 0);
        }
    };
    class GVH_STR : public GlobVarHandle {
    public:
        std::string value;

        void applyValue() {
            memcpy(address, &value, size);
        }
        void getValue() {
            memcpy(&value, address, size);
        }

        GVH_STR() {
            size = sizeof(std::string);
            isRef = false;
            value = "";
        }
    };

    class AGScript{
    private:
        
        AGScriptMgr* engine;
        Engine::GameObject* obj;
        std::string ClassName;
        std::string ModuleName;
        CScriptBuilder builder;

        asITypeInfo* getClassWithInterface(std::string class_name);
        
        
        void obtainScriptMainClass();

        asIScriptModule* module; //Module of the script
        asIScriptObject* mainClass_obj; //Object of main script class
        asITypeInfo* main_class; //Type of main script class
        bool hasErrors;
    public:
        void obtainScriptModule();

        unsigned int getGlobalVarsCount();
        void* getGlobalVariableAddr(unsigned int index);
        void getGlobalVariable(unsigned int index, const char** name, const char** _namespace, int* typeID);
        
        bool compileFromResource(Engine::ScriptResource* res);

        void onStart();
        void onUpdate();

        AGScript(AGScriptMgr* engine, Engine::GameObject* obj, std::string ClassName);
        ~AGScript();
    };
}

#endif