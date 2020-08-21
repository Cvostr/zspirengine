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
        
        bool isRef;
    public:
        unsigned int index;
        std::string name;
        std::string _namespace;
        void* address;
        void* value_ptr;
        int typeID;
        unsigned int size;

        void applyValue() {
            if (!isRef) {
                memcpy(address, value_ptr, size);
            }
            else {
                void* moved = address;
                uint64_t* vc = (uint64_t*)(*((uint64_t*)address));
                memcpy(vc, value_ptr, sizeof(ZSVECTOR3));
            }
        }
        void updValue() {
            if (!isRef) {
                memcpy(value_ptr, address, size);
            }
            else {
                void* moved = address;
                uint64_t* vc = (uint64_t*)(*((uint64_t*)address));
                memcpy(value_ptr, vc, sizeof(ZSVECTOR3));
            }
        }

        template <typename T>
        T* getValue() {
            return static_cast<T*>(value_ptr);
        }

        GlobVarHandle(int typeID) {
            index = 0;
            name = "";
            address = nullptr;
            this->typeID = typeID;
            isRef = false;

            switch (typeID) {
            case asTYPEID_INT32:
                size = sizeof(int);
                value_ptr = malloc(size);
                break;
            case asTYPEID_FLOAT:
                size = sizeof(float);
                value_ptr = malloc(size);
                break;
            case asTYPEID_BOOL:
                size = sizeof(bool);
                value_ptr = malloc(size);
                break;
            case AG_VECTOR3:
                size = sizeof(ZSVECTOR3);
                value_ptr = new ZSVECTOR3;
                isRef = true;
                break;
            case AG_STRING:
                size = sizeof(std::string);
                value_ptr = new std::string;
                break;
            }
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
        bool hasCompilerErrors();

        void onStart();
        void onUpdate();

        AGScript(AGScriptMgr* engine, Engine::GameObject* obj, std::string ClassName);
        ~AGScript();
    };
}

#endif