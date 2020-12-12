#pragma once

#include "../../Scripting/AngelScript.hpp"
#include "../../engine/Resources.hpp"

namespace Engine {
    class ZPScriptProperty : public Engine::IGameObjectComponent {
    private:
        Engine::AGScript* script;
    public:
        tCFVList mVars;
        std::string script_path;
        ScriptResource* script_res;
        Engine::AGScript* getScript();

        void SetScript(ScriptResource* script);

        void onValueChanged();
        void addPropertyInterfaceToInspector();
        void SetupScript();
        //function calls on script start
        void onStart();
        //function calls on script stop
        void onStop();
        //function calls on script update
        void onUpdate(float deltaTime); //calls update in scripts
        void copyTo(Engine::IGameObjectComponent* dest);
        bool makeFieldsList();
        //function calls on script text change
        void OnScriptChanges();
        ClassFieldValue* GetSuitableField(std::string Name, int TypeID, unsigned int index);

        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);

        void onTrigger(Engine::GameObject* obj);
        void onTriggerEnter(Engine::GameObject* obj);
        void onTriggerExit(Engine::GameObject* obj);

        ZPScriptProperty();
        ~ZPScriptProperty();
    };

}