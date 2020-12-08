#include "../../../headers/world/go_properties.h"

extern ZSGAME_DATA* game_data;

Engine::ZPScriptProperty::ZPScriptProperty() : 
	script(nullptr),
	script_res(nullptr)
{
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT;
}

Engine::ZPScriptProperty::~ZPScriptProperty() {
	delete script;
}

Engine::AGScript* Engine::ZPScriptProperty::getScript() {
	return script;
}

void Engine::ZPScriptProperty::SetupScript() {
	script_res = game_data->resources->getScriptByLabel(script_path);
	//if resource isn't found, then exit function
	if (script_res == nullptr)
		return;
	if (this->script != nullptr)
		delete this->script;
	//Allocate script with that resource
	script = new AGScript(go_link.updLinkPtr(), script_res->ClassName);
	script->obtainScriptMainClass();
	if (script_res != nullptr) {
		//Write global variables
		if (mVars.size() > 0) {
			//Send all global data to script
			for (unsigned int v_i = 0; v_i < mVars.size(); v_i++) {
				mVars[v_i]->inscriptValue = script->getGlobalVariableAddr(mVars[v_i]->Desc->index);
				mVars[v_i]->applyValue();
			}
		}
	}
}

void Engine::ZPScriptProperty::onStart() {
    if (script_res != nullptr) {
        script->onStart(); 
    }
}

void Engine::ZPScriptProperty::onUpdate(float deltaTime) {
    if (script_res != nullptr) {
        script->onUpdate();
    }
}

bool Engine::ZPScriptProperty::makeFieldsList() {
	
	ZPSClassDesc* Desc = this->script->getClassDesc();
	//Clear old fields array
	mVars.clear();
	for (unsigned int field_i = 0; field_i < Desc->ClassFieldsNum; field_i++) {
		ClassFieldDesc* CFDesc = &Desc->Fields[field_i];
		ClassFieldValue* new_CFV = new ClassFieldValue;
		new_CFV->Desc = CFDesc;
		void* v = Desc->__SampleObject->GetAddressOfProperty(field_i);
		bool TypeCreated = false;
		switch (CFDesc->typeID) {
		case asTYPEID_INT32:
			new_CFV->InitValue(*((int*)v));
			TypeCreated = true;
			break;
		case asTYPEID_FLOAT:
			new_CFV->InitValue(*((float*)v));
			TypeCreated = true;
			break;
		case asTYPEID_BOOL:
			new_CFV->InitValue(*((bool*)v));
			TypeCreated = true;
			break;
		case AG_VECTOR3:
			new_CFV->InitValue(*((Vec3*)v));
			TypeCreated = true;
			break;
		case AG_RGB_COLOR:
			new_CFV->InitValue(*((ZSRGBCOLOR*)v));
			TypeCreated = true;
			break;
		case AG_STRING:
			new_CFV->InitValue(*((std::string*)v));
			TypeCreated = true;
			break;
		}
		if (TypeCreated && !CFDesc->isPrivate) {
			mVars.push_back(new_CFV);
		}
	}

	return true;
}

void Engine::ZPScriptProperty::onStop() {
	script->onStop();
	delete script;
}

void Engine::ZPScriptProperty::copyTo(Engine::IGameObjectComponent* dest) {
	if (dest->type != this->type) return; //if it isn't transform

	ZPScriptProperty* _dest = static_cast<ZPScriptProperty*>(dest);
	//write path to script
	_dest->script_path = this->script_path;
	_dest->SetScript(game_data->resources->getScriptByLabel(script_path));
	//copy global variable handlers
	for (unsigned int v_i = 0; v_i < mVars.size(); v_i++) {
		ClassFieldValue* old_handle = mVars[v_i];
		ClassFieldValue* new_handle = _dest->mVars[v_i];

		new_handle->Desc = old_handle->Desc;
		memcpy(new_handle->TempValue, old_handle->TempValue, old_handle->FieldSize);
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

void Engine::ZPScriptProperty::SetScript(ScriptResource* script) {
	script_res = script;
	if (this->script != nullptr)
		delete this->script;
	this->script = new AGScript(go_link.updLinkPtr(), script_res->ClassName);
	makeFieldsList();
}

void Engine::ZPScriptProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
	unsigned int offset = 1;
	unsigned int vars = 0;

	readString(script_path, data, offset);
	//get ScriptResource pointer
	SetScript(game_data->resources->getScriptByLabel(script_path));
	//get count of variables
	readBinaryValue(&vars, data + offset, offset);
	//read all variables data
	for (unsigned int v_i = 0; v_i < vars; v_i++) {
		int typeID = 0;
		unsigned int index = v_i;
		//read index
		readBinaryValue(&index, data + offset, offset);
		//read ID of type
		readBinaryValue(&typeID, data + offset, offset);
		//read field name
		std::string FieldName;
		readString(FieldName, data, offset);

		Engine::ClassFieldValue* var = this->mVars[v_i];
		Engine::ClassFieldDesc* SuitableDesc = script->getClassDesc()->GetSuitableDesc(FieldName, typeID, index);
		
		if (SuitableDesc == nullptr)
			continue;

		var->Desc = SuitableDesc;
		
		if (typeID != AG_STRING) {
			//if value is non-std string
			memcpy(var->TempValue, data + offset, var->FieldSize);
			offset += var->FieldSize;
		}
		else {
			//value is string
			//then load its symbols
			std::string* str = static_cast<std::string*>(var->TempValue);
			//clear old string
			str->clear();
			//Obtain size of string
			readString(*str, data, offset);
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
	unsigned int varsNum = static_cast<unsigned int>(mVars.size());
	stream->writeBinaryValue(&varsNum);

	for (unsigned int v_i = 0; v_i < mVars.size(); v_i++) {
		Engine::ClassFieldValue* var = mVars[v_i];

		stream->writeBinaryValue(&var->Desc->index);
		stream->writeBinaryValue(&var->Desc->typeID);
		stream->writeString(var->Desc->name);
		if (var->Desc->typeID != AG_STRING)
			stream->write(reinterpret_cast<char*>(var->TempValue), var->FieldSize);
		else {
			std::string* str = static_cast<std::string*>(var->TempValue);
			const char* ch = str->c_str();
			unsigned int str_size = static_cast<unsigned int>(str->size());
			stream->writeString(*str);
		}
	}
}