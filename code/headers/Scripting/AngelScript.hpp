#pragma once

#include <angelscript/angelscript.h>
#include <angelscript/scriptbuilder.h>

#include "../world/World.hpp"
#include "AngelScriptMgr.h"
#include "../engine/Resources.hpp"
#include <string>
#include <cstdint>
#include <iostream>

#define AG_STRING 67108876
#define AG_VECTOR3 67108881
#define AG_QUAT 67108882
#define AG_RGB_COLOR 67108883

namespace Engine{

    class ClassFieldValue {
    public:
        void* inscriptValue;
        void* TempValue;

        ClassFieldDesc* Desc;
        uint32_t FieldSize;

        ClassFieldValue() {
            delete TempValue;
        }

        template <typename T>
        T& Value() {
            return *(static_cast<T*>(TempValue));
        }
        
        template <typename T>
        void InitValue(T value) {
            TempValue = new T;
            FieldSize = sizeof(T);
            copyValue(&value, TempValue);
        }
        //Put temp value into script value
        void applyValue() {
            copyValue(TempValue, inscriptValue);
        }
        void updateValue() {
            copyValue(inscriptValue, TempValue);
        }

        void copyValue(void* src, void* dest) {
            switch (Desc->typeID) {
            case AG_STRING: {
                std::string* src_Str = static_cast<std::string*>(src);
                std::string* dest_Str = static_cast<std::string*>(dest);
                *dest_Str = *src_Str;

                break;
            }
            default:
                memcpy(dest, src, FieldSize);
                break;
            }
        }

    };

    typedef std::vector<ClassFieldValue*> tCFVList;

    class AGScript{
    private:
       
        Engine::GameObject* obj;
        std::string ClassName;
        
        asIScriptObject* mainClass_obj; //Object of main script class
        asITypeInfo* main_class; //Type of main script class
    public:
        void obtainScriptMainClass();
        asIScriptFunction* getFuncOnMainClass(const char* decl);
        asITypeInfo* getMainClassType();
        asIScriptObject* getMainClassPtr();
        std::string getClassName();

        ZPSClassDesc* getClassDesc();
        void* getGlobalVariableAddr(unsigned int index);
      
        void onStart();
        void onStop();
        void onUpdate();
        void onTrigger(Engine::GameObject* obj);
        void onTriggerEnter(Engine::GameObject* obj);
        void onTriggerExit(Engine::GameObject* obj);

        AGScript(Engine::GameObject* obj, std::string ClassName);
        ~AGScript();
    };

    
}