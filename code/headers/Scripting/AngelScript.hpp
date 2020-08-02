#ifndef ANGEL_SCRIPT_HPP
#define ANGEL_SCRIPT_HPP

#include <angelscript/angelscript.h>
#include <angelscript/scriptbuilder.h>

#include "../world/World.h"
#include "AngelScriptMgr.h"
#include "../engine/resources.h"

namespace Engine{
    class AGScript{
    private:
        AGScriptMgr* engine;
        Engine::GameObject* obj;
        std::string ClassName;
        std::string ModuleName;
        CScriptBuilder builder;

        asITypeInfo* getClassWithInterface(std::string class_name);
        void obtainScriptMainClass();

        asIScriptObject* mainClass_obj; //Object of main script class
        asITypeInfo* main_class; //Type of main script class
        bool hasErrors;
    public:

        void compileFromResource(Engine::ScriptResource* res);

        void onStart();
        void onUpdate();

        AGScript(AGScriptMgr* engine, Engine::GameObject* obj, std::string ClassName);
        ~AGScript();
    };
}

#endif