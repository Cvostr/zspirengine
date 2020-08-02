#include "../../headers/Scripting/AngelScript.hpp"
#include "../../headers/game.h"

using namespace Engine;

//Hack to support resources
extern ZSGAME_DATA* game_data;

void AGScript::compileFromResource(Engine::ScriptResource* res) {
	int result = 0;
	
	result = builder.AddSectionFromMemory(res->rel_path.c_str(), res->script_content.c_str(), res->size);

	result = builder.BuildModule();
	if (result < 0)
		hasErrors = true;
}

void AGScript::onStart() {

	if (hasErrors == true)
		return;

	obtainScriptMainClass();

	//Obtain Start() function
	asIScriptFunction* func = main_class->GetMethodByDecl("void Start()");
	//Execute function
	engine->getAgScriptContext()->Prepare(func);
	engine->getAgScriptContext()->SetObject(mainClass_obj);
	int r = engine->getAgScriptContext()->Execute();
	assert(r >= 0);
}

void AGScript::onUpdate() {
	if (hasErrors == true)
		return;

	//Obtain Update() function
	asIScriptFunction* func = main_class->GetMethodByDecl("void Update()");
	//Execute function
	engine->getAgScriptContext()->Prepare(func);
	engine->getAgScriptContext()->SetObject(mainClass_obj);
	int r = engine->getAgScriptContext()->Execute();
	assert(r >= 0);
}

void AGScript::obtainScriptMainClass() {
	asIScriptModule* mod = engine->GetModule(ModuleName, asGM_ONLY_IF_EXISTS);
	//Obtain class with interface ZPScript
	main_class = getClassWithInterface("ZPScript");
	ClassName = main_class->GetName();
	//Get pointer to class contructor
	std::string construct_str = this->ClassName + " @" + this->ClassName + "(GameObject@)";
	asIScriptFunction* factory = main_class->GetFactoryByDecl(construct_str.c_str());
	//Allocate class by Constructor
	{
		engine->getAgScriptContext()->Prepare(factory);
		engine->getAgScriptContext()->SetArgObject(0, obj);
		engine->getAgScriptContext()->Execute();
	}
	//Get returned created class
	mainClass_obj = *(asIScriptObject**)engine->getAgScriptContext()->GetAddressOfReturnValue();
	mainClass_obj->AddRef();
}

asITypeInfo* AGScript::getClassWithInterface(std::string class_name) {
	asITypeInfo* type = 0;
	asIScriptModule* mod = engine->GetModule(ModuleName, asGM_ONLY_IF_EXISTS);
	int tc = mod->GetObjectTypeCount();
	for (int n = 0; n < tc; n++)
	{
		bool found = false;
		type = mod->GetObjectTypeByIndex(n);
		int ic = type->GetInterfaceCount();
		for (int i = 0; i < ic; i++)
		{
			if (strcmp(type->GetInterface(i)->GetName(), class_name.c_str()) == 0)
			{
				return type;
			}
		}
	}
	return type;
}

AGScript::AGScript(AGScriptMgr* engine, Engine::GameObject* obj, std::string ClassName) {
	this->ClassName = ClassName;
	this->engine = engine;
	this->obj = obj;

	ModuleName = ClassName + "_" + obj->str_id;

	int r = builder.StartNewModule(engine->getAgScriptEngine(), ModuleName.c_str());
	hasErrors = false;
}
AGScript::~AGScript() {

}