#include "../../../headers/world/go_properties.h"

extern ZSGAME_DATA* game_data;

Engine::ZPScriptProperty::ZPScriptProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT;
    script = nullptr;
    script_res = nullptr;
}

Engine::ZPScriptProperty::~ZPScriptProperty() {

}
void Engine::ZPScriptProperty::onStart() {
    if (script_res != nullptr) {
        script = new AGScript(game_data->script_manager, go_link.updLinkPtr(), "angel");
        script->compileFromResource(this->script_res);
		//call onStart() inside script
        script->onStart(); 
		if(vars.size() == 0)
			//update vars list
			makeGlobalVarsList();
		else {
			//Send all global data to script
			for (unsigned int v_i = 0; v_i < vars.size(); v_i++) {
				vars[v_i]->address = script->getGlobalVariableAddr(v_i);
				vars[v_i]->applyValue();
			}
		}
    }
}

void Engine::ZPScriptProperty::onUpdate(float deltaTime) {
    if (script_res != nullptr) {
        script->onUpdate();
    }
}

void Engine::ZPScriptProperty::makeGlobalVarsList() {
	if (script == nullptr) return;
	//Clear vars list
	vars.clear();
	if (!script->compileFromResource(this->script_res)) {
		game_data->out_manager->spawnRuntimeError(RuntimeErrorType::RE_TYPE_SCRIPT_ERROR);
		return;
	}
	script->obtainScriptModule();
	unsigned int gVarsNum = script->getGlobalVarsCount();
	//iterate over all variables
	for (unsigned int gV_i = 0; gV_i < gVarsNum; gV_i++) {
		//define property variables
		const char* gVar_name;
		const char* gVar_namespace;
		int typeID = 0;
		//get variable properties
		script->getGlobalVariable(gV_i, &gVar_name, &gVar_namespace, &typeID);

		GlobVarHandle* handle = nullptr;
		if (typeID == asTYPEID_INT32)
			handle = new GVH_INT32;
		if (typeID == asTYPEID_FLOAT)
			handle = new GVH_FLOAT;
		if (typeID == asTYPEID_BOOL)
			handle = new GVH_BOOL;
		if (typeID == AG_VECTOR3)
			handle = new GVH_VEC3;
		if (typeID == AG_STRING)
			handle = new GVH_STR;
		//assign variable props
		handle->index = gV_i;
		handle->typeID = typeID;
		handle->name = std::string(gVar_name);
		handle->_namespace = std::string(gVar_namespace);
		handle->address = script->getGlobalVariableAddr(gV_i);
		//Write base value
		handle->getValue();
		//push property handle
		this->vars.push_back(handle);
	}
	delete script;
}


void Engine::ZPScriptProperty::copyTo(Engine::GameObjectProperty* dest) {
	if (dest->type != this->type) return; //if it isn't transform

	ZPScriptProperty* _dest = static_cast<ZPScriptProperty*>(dest);
	//write path to script
	_dest->script_path = this->script_path;
	//copy global variable handlers
	for (unsigned int v_i = 0; v_i < vars.size(); v_i++) {

	}
}

void Engine::ZPScriptProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {

}
