#include "../../headers/Scripting/AngelScript.hpp"
#include "../../headers/game.h"

using namespace Engine;

//Hack to support resources
extern ZSGAME_DATA* game_data;

GlobVarHandle::GlobVarHandle(int typeID) {
	index = 0;
	name = "";
	address = nullptr;
	value_ptr = nullptr;
	this->typeID = typeID;

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
		break;
	case AG_RGB_COLOR:
		size = sizeof(ZSRGBCOLOR);
		value_ptr = new ZSRGBCOLOR;
		break;
	case AG_STRING:
		size = sizeof(std::string);
		value_ptr = new std::string;
		break;
	}
}

GlobVarHandle::~GlobVarHandle() {
	auto cl = value_ptr;
	delete cl;
}

void GlobVarHandle::applyValue() {
	copyValue(value_ptr, address);
}
void GlobVarHandle::updValue() {
	copyValue(address, value_ptr);
}

void GlobVarHandle::copyValue(void* src, void* dest) {
	switch (typeID) {
	case AG_STRING: {
#ifdef _DEBUG
		std::string* src_Str = static_cast<std::string*>(src);
		std::string* dest_Str = static_cast<std::string*>(dest);
		*dest_Str = *src_Str;
#else
		memcpy(dest, src, size);
#endif
		break;
	}
	default:
		memcpy(dest, src, size);
		break;
	}
}

bool AGScript::compileFromResource(Engine::ScriptResource* res) {
	int result = 0;
	unsigned int scr_file_offset = 0;
	const char* script_str = res->script_content.c_str();
	std::string line;
	
	//Add main script
	result = builder.AddSectionFromMemory(res->rel_path.c_str(), script_str, res->size);
	//read first script line
	readLine(line, script_str, scr_file_offset);
	while (startsWith(line, "//use")) {
		const char* dep_str_c = line.c_str() + 6;
		std::string dep_str = std::string(dep_str_c);
		dep_str.pop_back();
		Engine::ScriptResource* _res = game_data->resources->getScriptByLabel(dep_str);
		readLine(line, script_str, scr_file_offset);
		//include dependency
		result = builder.AddSectionFromMemory(dep_str_c, _res->script_content.c_str(), res->size);
	}

	result = builder.BuildModule();
	if (result < 0) {
		hasErrors = true;
		res->hasError = true;
		return false;
	}
	res->hasError = false;
	return true;
}

void AGScript::onStart() {

	if (hasErrors == true)
		return;
	//fill pointer with main class
	obtainScriptMainClass();
	
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
	if (hasErrors == true)
		return;

	//Obtain Stop() function
	asIScriptFunction* func = getFuncOnMainClass("void Stop()");
	if (func != nullptr) {
		//Execute function
		engine->getAgScriptContext()->Prepare(func);
		engine->getAgScriptContext()->SetObject(mainClass_obj);
		engine->getAgScriptContext()->Execute();
	}
}

bool AGScript::hasCompilerErrors() {
	return hasErrors;
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
	if (hasErrors == true)
		return;

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
	if (hasErrors == true)
		return;

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
	if (hasErrors == true)
		return;

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
	if (hasErrors == true)
		return;

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

unsigned int AGScript::getGlobalVarsCount() {
	return module->GetGlobalVarCount();
}

void* AGScript::getGlobalVariableAddr(unsigned int index) {
	return module->GetAddressOfGlobalVar(index);
}

void AGScript::getGlobalVariable(unsigned int index, const char** name, const char** _namespace, int* typeID) {
	unsigned int max = module->GetGlobalVarCount();
	if (index > max - 1) 
		return;
	module->GetGlobalVar(index, name, _namespace, typeID);
}

void AGScript::obtainScriptModule() {
	asIScriptModule* mod = engine->GetModule(ModuleName, asGM_ONLY_IF_EXISTS);
	this->module = mod;
}

void AGScript::obtainScriptMainClass() {
	int result = 0;
	obtainScriptModule();
	//Obtain class with interface ZPScript
	main_class = getClassWithInterface("ZPScript");	
	ClassName = main_class->GetName();
	//Get pointer to class contructor
	std::string construct_str = this->ClassName + " @" + this->ClassName + "(GameObject@)";
	asIScriptFunction* factory = main_class->GetFactoryByDecl(construct_str.c_str());
	//Allocate class by Constructor
	{
		engine->getAgScriptContext()->Prepare(factory);
		if(obj != nullptr)
			engine->getAgScriptContext()->SetArgObject(0, obj);
		result = engine->getAgScriptContext()->Execute();
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
	if (obj != nullptr)
		ModuleName = ClassName + "_" + obj->str_id;
	else
		ModuleName = "startupModule";

	builder.StartNewModule(engine->getAgScriptEngine(), ModuleName.c_str());
	hasErrors = false;
	//nullptr to pointers
	module = nullptr;
	mainClass_obj = nullptr;
	main_class = nullptr;
}
AGScript::~AGScript() {
	if(module != nullptr)
		module->Discard();
}