#include "../../../headers/world/go_properties.h"

extern ZSGAME_DATA* game_data;

Engine::ZPScriptProperty::ZPScriptProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT;
    script = nullptr;
    script_res = nullptr;
}

Engine::ZPScriptProperty::~ZPScriptProperty() {
	delete script;
}
void Engine::ZPScriptProperty::onStart() {
    if (script_res != nullptr) {
        script = new AGScript(game_data->script_manager, go_link.updLinkPtr(), "angel");
        script->compileFromResource(this->script_res);
		script->obtainScriptModule();
		//Write global variables
		if(vars.size() > 0) {
			//Send all global data to script
			for (unsigned int v_i = 0; v_i < vars.size(); v_i++) {
				vars[v_i]->address = script->getGlobalVariableAddr(v_i);
				vars[v_i]->applyValue();
			}
		}
		//call onStart() inside script
        script->onStart(); 
		if(vars.size() == 0)
			//update vars list
			makeGlobalVarsList();
		
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

		GlobVarHandle* handle = new GlobVarHandle(typeID);
		//assign variable props
		handle->index = gV_i;
		handle->name = std::string(gVar_name);
		handle->_namespace = std::string(gVar_namespace);
		handle->address = script->getGlobalVariableAddr(gV_i);
		//Write base value
		handle->updValue();
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
		GlobVarHandle* old_handle = vars[v_i];
		GlobVarHandle* new_handle = new GlobVarHandle(old_handle->typeID);

		new_handle->index = old_handle->index;
		new_handle->name = old_handle->name;
		new_handle->_namespace = old_handle->_namespace;
		memcpy(new_handle->value_ptr, old_handle->value_ptr, old_handle->size);
	}
}

void Engine::ZPScriptProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
	unsigned int offset = 1;

	while (data[offset] != ' ' && data[offset] != '\n') {
		script_path += data[offset];
		offset++;
	}
}
