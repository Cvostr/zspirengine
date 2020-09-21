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

Engine::AGScript* Engine::ZPScriptProperty::getScript() {
	return script;
}

void Engine::ZPScriptProperty::onStart() {
    if (script_res != nullptr) {
        script = new AGScript(game_data->script_manager, go_link.updLinkPtr(), script_res->rel_path);
        script->compileFromResource(this->script_res);
		script->obtainScriptModule();
		//if script contains errors, then stop further activity 
		if (script->hasCompilerErrors())
			return;

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
    }
}

void Engine::ZPScriptProperty::onUpdate(float deltaTime) {
    if (script_res != nullptr) {
        script->onUpdate();
    }
}

bool Engine::ZPScriptProperty::makeGlobalVarsList() {
	if (script == nullptr) 
		return false;
	//Clear vars list
	vars.clear();
	if (!script->compileFromResource(this->script_res)) {
		game_data->out_manager->spawnRuntimeError(RuntimeErrorType::RE_TYPE_SCRIPT_ERROR);
		return false;
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
	return true;
}

void Engine::ZPScriptProperty::onStop() {
	script->onStop();
	delete script;
}

void Engine::ZPScriptProperty::copyTo(Engine::GameObjectProperty* dest) {
	if (dest->type != this->type) return; //if it isn't transform

	ZPScriptProperty* _dest = static_cast<ZPScriptProperty*>(dest);
	//write path to script
	_dest->script_path = this->script_path;
	_dest->script_res = game_data->resources->getScriptByLabel(script_path);
	//_dest->lastCheckHasErrors = this->lastCheckHasErrors;
	//copy global variable handlers
	for (unsigned int v_i = 0; v_i < vars.size(); v_i++) {
		GlobVarHandle* old_handle = vars[v_i];
		GlobVarHandle* new_handle = new GlobVarHandle(old_handle->typeID);

		new_handle->index = old_handle->index;
		new_handle->name = old_handle->name;
		new_handle->_namespace = old_handle->_namespace;
		memcpy(new_handle->value_ptr, old_handle->value_ptr, old_handle->size);
		_dest->vars.push_back(new_handle);
	}
}

void Engine::ZPScriptProperty::onTrigger(Engine::GameObject* obj) {
	script->onTrigger(obj);
}
void Engine::ZPScriptProperty::onTriggerEnter(Engine::GameObject* obj) {
	script->onTriggerEnter(obj);
}
void Engine::ZPScriptProperty::onTriggerExit(Engine::GameObject* obj) {
	script->onTriggerExit(obj);
}

void Engine::ZPScriptProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
	unsigned int offset = 1;
	unsigned int vars = 0;

	readString(script_path, data, offset);
	bool _hasErrors;
	//get ScriptResource pointer
	{
		script_res = game_data->resources->getScriptByLabel(script_path);
		script = new AGScript(game_data->script_manager, nullptr, script_res->rel_path);
		_hasErrors = !makeGlobalVarsList();
	}

	if (_hasErrors)
		return;

	//get count of variables
	readBinaryValue(&vars, data + offset, offset);
	//read all variables data
	for (unsigned int v_i = 0; v_i < vars; v_i++) {
		int typeID = 0;
		unsigned int index = 0;
		//read index
		readBinaryValue(&index, data + offset, offset);
		//read ID of type
		readBinaryValue(&typeID, data + offset, offset);

		Engine::GlobVarHandle* var = this->vars[index];
		var->index = index;
		if (typeID != AG_STRING) {
			//if value is non-std string
			memcpy(var->value_ptr, data + offset, var->size);
			offset += var->size;
		}
		else {
			//value is string
			//then load its symbols
			std::string* str = static_cast<std::string*>(var->value_ptr);
			//clear old string
			str->clear();
			//Obtain size of string
			unsigned int str_size = 0;
			{
				readBinaryValue(&str_size, data + offset, offset);
			}
			//Read and append string symbols
			for (unsigned int i = 0; i < str_size; i++) {
				str->push_back(*(data + offset));	
				offset += 1;
			}
		}
	}
}

void Engine::ZPScriptProperty::savePropertyToStream(ZsStream* stream, GameObject* obj){
	//Write header
	*stream << "\nG_SCRIPT ";
	stream->writeBinaryValue(&active);
	*stream << " ";
	//Write script
	stream->writeString(script_path);
	unsigned int varsNum = static_cast<unsigned int>(vars.size());
	stream->writeBinaryValue(&varsNum);

	for (unsigned int v_i = 0; v_i < vars.size(); v_i++) {
		Engine::GlobVarHandle* var = vars[v_i];

		stream->writeBinaryValue(&var->index);
		stream->writeBinaryValue(&var->typeID);
		if (var->typeID != AG_STRING)
			stream->write(reinterpret_cast<char*>(var->value_ptr), var->size);
		else {
			std::string* str = static_cast<std::string*>(var->value_ptr);
			const char* ch = str->c_str();
			unsigned int str_size = static_cast<unsigned int>(str->size());
			stream->writeBinaryValue(&str_size);
			stream->write(ch, str_size);
		}
	}
}