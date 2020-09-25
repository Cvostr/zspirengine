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
#define AG_VECTOR3 67108881
#define AG_RGB_COLOR 67108882

namespace Engine{

    class GlobVarHandle {
    protected:
    public:
        unsigned int index;
        std::string name;
        std::string _namespace;
        void* address;
        void* value_ptr;
        int typeID;
        unsigned int size;

        void applyValue();
        void updValue();
        void copyValue(void* src, void* dest);

        template <typename T>
        T* getValue() {
            return static_cast<T*>(value_ptr);
        }

        GlobVarHandle(int typeID);
        ~GlobVarHandle();
    };

    class AGScript{
    private:
        
        AGScriptMgr* engine;
        Engine::GameObject* obj;
        std::string ClassName;
        std::string ModuleName;
        CScriptBuilder builder;


        asITypeInfo* getClassWithInterface(std::string class_name);
        
        asIScriptModule* module; //Module of the script
        asIScriptObject* mainClass_obj; //Object of main script class
        asITypeInfo* main_class; //Type of main script class
        bool hasErrors;
    public:
        ZPScriptClass* scr_class;
        void obtainScriptModule();
        void obtainScriptMainClass();
        asIScriptFunction* getFuncOnMainClass(const char* decl);
        asITypeInfo* getMainClassType();
        asIScriptObject* getMainClassPtr();
        std::string getClassName();
        unsigned int getGlobalVarsCount();
        void* getGlobalVariableAddr(unsigned int index);
        void getGlobalVariable(unsigned int index, const char** name, const char** _namespace, int* typeID);
        
        bool compileFromResource(Engine::ScriptResource* res);
        bool hasCompilerErrors();

        void onStart();
        void onStop();
        void onUpdate();
        void onTrigger(Engine::GameObject* obj);
        void onTriggerEnter(Engine::GameObject* obj);
        void onTriggerExit(Engine::GameObject* obj);

        AGScript(AGScriptMgr* engine, Engine::GameObject* obj, std::string ClassName);
        ~AGScript();
    };

    
}

#endif