#include "../../headers/Scripting/AngelScript.hpp"
#include "../../headers/game.h"

using namespace Engine;

//Hack to support resources
extern ZSGAME_DATA* game_data;

ZPSClassDesc* AGScript::getClassDesc() {
	return game_data->script_manager->GetClassDesc(ClassName);
}

void AGScript::onStart() {

	AGScriptMgr* engine = game_data->script_manager;
	
	//Obtain Start() function
	asIScriptFunction* func = getFuncOnMainClass("void Start()");
	if (func != nullptr) {
		//Execute function
		engine->getAgScriptContext()->Prepare(func);
		engine->getAgScriptContext()->SetObject(mainClass_obj);
		engine->getAgScriptContext()->Execute();
	}
}

void AGScript::onStop() {
	AGScriptMgr* engine = game_data->script_manager;

	//Obtain Stop() function
	asIScriptFunction* func = getFuncOnMainClass("void Stop()");
	if (func != nullptr) {
		//Execute function
		engine->getAgScriptContext()->Prepare(func);
		engine->getAgScriptContext()->SetObject(mainClass_obj);
		engine->getAgScriptContext()->Execute();
	}

	engine->getAgScriptEngine()->ReleaseScriptObject(this->mainClass_obj, this->main_class);
}

std::string AGScript::getClassName() {
	return ClassName;
}
asIScriptObject* AGScript::getMainClassPtr() {
	return mainClass_obj;
}

asITypeInfo* AGScript::getMainClassType() {
	return main_class;
}

asIScriptFunction* AGScript::getFuncOnMainClass(const char* decl) {
	return main_class->GetMethodByDecl(decl);
}

void AGScript::onUpdate() {
	AGScriptMgr* engine = game_data->script_manager;
	//Obtain Update() function
	asIScriptFunction* func = getFuncOnMainClass("void Update()");
	if (func != nullptr) {
		//Execute function
		engine->getAgScriptContext()->Prepare(func);
		engine->getAgScriptContext()->SetObject(mainClass_obj);
		engine->getAgScriptContext()->Execute();
	}
}

void AGScript::onTrigger(Engine::GameObject* obj) {
	AGScriptMgr* engine = game_data->script_manager;
	//Obtain onTrigger() function
	asIScriptFunction* func = getFuncOnMainClass("void onTrigger(GameObject@)");
	if (func != nullptr) {
		//Execute function
		engine->getAgScriptContext()->Prepare(func);
		engine->getAgScriptContext()->SetObject(mainClass_obj);
		engine->getAgScriptContext()->SetArgObject(0, obj);
		engine->getAgScriptContext()->Execute();
	}
}

void AGScript::onTriggerEnter(Engine::GameObject* obj) {
	AGScriptMgr* engine = game_data->script_manager;
	//Obtain onTrigger() function
	asIScriptFunction* func = getFuncOnMainClass("void onTriggerEnter(GameObject@)");
	if (func != nullptr) {
		//Execute function
		engine->getAgScriptContext()->Prepare(func);
		engine->getAgScriptContext()->SetObject(mainClass_obj);
		engine->getAgScriptContext()->SetArgObject(0, obj);
		engine->getAgScriptContext()->Execute();
	}
}
void AGScript::onTriggerExit(Engine::GameObject* obj) {
	AGScriptMgr* engine = game_data->script_manager;

	//Obtain onTrigger() function
	asIScriptFunction* func = getFuncOnMainClass("void onTriggerExit(GameObject@)");
	if (func != nullptr) {
		//Execute function
		engine->getAgScriptContext()->Prepare(func);
		engine->getAgScriptContext()->SetObject(mainClass_obj);
		engine->getAgScriptContext()->SetArgObject(0, obj);
		engine->getAgScriptContext()->Execute();
	}
}

void* AGScript::getGlobalVariableAddr(unsigned int index) {
	return this->mainClass_obj->GetAddressOfProperty(index);
}

void AGScript::obtainScriptMainClass() {
	AGScriptMgr* engine = game_data->script_manager;

	engine->CreateClass(ClassName, obj, &mainClass_obj, &main_class);
}

AGScript::AGScript(Engine::GameObject* obj, std::string ClassName) :
	ClassName(ClassName),
	obj(obj),
	mainClass_obj(nullptr),
	main_class(nullptr)
{
	
}
AGScript::~AGScript() {
	AGScriptMgr* engine = game_data->script_manager;
}