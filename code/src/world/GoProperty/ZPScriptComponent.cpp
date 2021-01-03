#include "../../../headers/world/ObjectsComponents/ZPScriptComponent.hpp"
#include "../../../headers/misc/misc.h"
#include "../../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::ZPScriptComponent::ZPScriptComponent() :
	script(nullptr),
	script_res(nullptr)
{
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT;
}

Engine::ZPScriptComponent::~ZPScriptComponent() {
	delete script;
}

Engine::AGScript* Engine::ZPScriptComponent::getScript() {
	return script;
}

void Engine::ZPScriptComponent::SetupScript() {
	if (this->script != nullptr)
		delete this->script;
	script_res = game_data->resources->getScriptByLabel(script_path);
	//if resource isn't found, then exit function
	if (script_res == nullptr)
		return;
	//Allocate script with that resource
	script = new AGScript(go_link.updLinkPtr(), script_res->ClassName);
	script->obtainScriptMainClass();
	if (script_res != nullptr) {
		//Write global variables
		if (mVars.size() > 0) {
			//Send all global data to script
			for (unsigned int v_i = 0; v_i < mVars.size(); v_i++) {
				mVars[v_i]->inscriptValue = script->getGlobalVariableAddr(mVars[v_i]->Desc.index);
				mVars[v_i]->applyValue();
			}
		}
	}
}

void Engine::ZPScriptComponent::onStart() {
    if (script_res != nullptr) {
        script->onStart(); 
    }
}

void Engine::ZPScriptComponent::onUpdate(float deltaTime) {
    if (script_res != nullptr) {
        script->onUpdate();
		if (game_data->isEditor) {
			//if we are in editor
			//then update value in inspector
			for (unsigned int v_i = 0; v_i < mVars.size(); v_i++) {
				ClassFieldValue* field = mVars[v_i];
				field->updateValue();
			}
		}
    }
}

bool Engine::ZPScriptComponent::makeFieldsList() {
	
	ZPSClassDesc* Desc = this->script->getClassDesc();
	//Clear old fields array
	mVars.clear();
	for (unsigned int field_i = 0; field_i < Desc->ClassFieldsNum; field_i++) {
		ClassFieldDesc* CFDesc = &Desc->Fields[field_i];
		ClassFieldValue* new_CFV = new ClassFieldValue;
		new_CFV->Desc = *CFDesc;
		void* v = Desc->__SampleObject->GetAddressOfProperty(field_i);
		bool TypeCreated = true;
		switch (CFDesc->typeID) {
		case asTYPEID_INT32:
			new_CFV->InitValue(*((int*)v));
			break;
		case asTYPEID_FLOAT:
			new_CFV->InitValue(*((float*)v));
			break;
		case asTYPEID_BOOL:
			new_CFV->InitValue(*((bool*)v));
			break;
		case AG_VECTOR3:
			new_CFV->InitValue(*((Vec3*)v));
			break;
		case AG_RGB_COLOR:
			new_CFV->InitValue(*((RGBAColor*)v));
			break;
		case AG_STRING:
			new_CFV->InitValue(*((std::string*)v));
			break;
		default:
			TypeCreated = false;
			break;
		}
		if (TypeCreated && !CFDesc->isPrivate) {
			mVars.push_back(new_CFV);
		}
	}

	return true;
}

void Engine::ZPScriptComponent::onStop() {
	script->onStop();
	delete script;
}

void Engine::ZPScriptComponent::copyTo(Engine::IGameObjectComponent* dest) {
	if (dest->type != this->type) return; //if it isn't transform

	ZPScriptComponent* _dest = static_cast<ZPScriptComponent*>(dest);
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

void Engine::ZPScriptComponent::onTrigger(Engine::GameObject* obj) {
	script->onTrigger(obj);
}
void Engine::ZPScriptComponent::onTriggerEnter(Engine::GameObject* obj) {
	script->onTriggerEnter(obj);
}
void Engine::ZPScriptComponent::onTriggerExit(Engine::GameObject* obj) {
	script->onTriggerExit(obj);
}

Engine::ClassFieldValue* Engine::ZPScriptComponent::GetSuitableField(std::string Name, int TypeID, unsigned int index) {
	for (unsigned int i = 0; i < this->mVars.size(); i++) {
		ClassFieldValue* field = mVars[i];

		if (field->Desc.index == index && field->Desc.typeID == TypeID && field->Desc.name.compare(Name) == 0)
			return field;
		if (field->Desc.typeID == TypeID && field->Desc.name.compare(Name) == 0)
			return field;
	}
	return nullptr;
}

void Engine::ZPScriptComponent::OnScriptChanges() {
	if (game_data->script_manager->HasCompilerErrors())
		return;
	//Store old fields
	tCFVList tempVars = mVars;
	//Get new fields arrays
	SetScript(script_res);

	Engine::ZPSClassDesc* class_desc = script->getClassDesc();
	for (unsigned int field_i = 0; field_i < class_desc->ClassFieldsNum; field_i++) {
		ClassFieldDesc* Desc = &class_desc->Fields[field_i];

		Engine::ClassFieldValue* oldVar = nullptr;
		Engine::ClassFieldValue* newVar = nullptr;
		//Try to find old and new field pointer
		for (unsigned int tmpv_i = 0; tmpv_i < tempVars.size(); tmpv_i++) {
			Engine::ClassFieldValue* field = tempVars[tmpv_i];
			if (field->Desc.typeID == Desc->typeID && field->Desc.name.compare(Desc->name) == 0)
				oldVar = field;
		}
		for (unsigned int newv_i = 0; newv_i < this->mVars.size(); newv_i++) {
			Engine::ClassFieldValue* field = mVars[newv_i];
			if (field->Desc.typeID == Desc->typeID && field->Desc.name.compare(Desc->name) == 0)
				newVar = field;
		}
		if(newVar != nullptr && oldVar != nullptr)
			oldVar->copyValue(oldVar->TempValue, newVar->TempValue);
	}
}

void Engine::ZPScriptComponent::SetScript(ScriptResource* script) {
	if (script == nullptr)
		return;
	script_res = script;
	if (this->script != nullptr)
		delete this->script;
	if (game_data->script_manager->HasCompilerErrors())
		return;
	this->script = new AGScript(go_link.updLinkPtr(), script_res->ClassName);
	makeFieldsList();
}

void Engine::ZPScriptComponent::loadPropertyFromMemory(const char* data, GameObject* obj) {
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
		unsigned int index = 0;
		std::string FieldName;
		//read index
		readBinaryValue(&index, data + offset, offset);
		//read ID of type
		readBinaryValue(&typeID, data + offset, offset);
		//read field name
		readString(FieldName, data, offset);

		//Engine::ClassFieldDesc* SuitableDesc = script->getClassDesc()->GetSuitableDesc(FieldName, typeID, index);
		Engine::ClassFieldValue* var = GetSuitableField(FieldName, typeID, index);

		if (var == nullptr )
			continue;
		//else
			//var->Desc = SuitableDesc; //Bind existing desc
		
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

void Engine::ZPScriptComponent::savePropertyToStream(ZsStream* stream, GameObject* obj){
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

		stream->writeBinaryValue(&var->Desc.index);
		stream->writeBinaryValue(&var->Desc.typeID);
		stream->writeString(var->Desc.name);
		if (var->Desc.typeID != AG_STRING)
			stream->write(reinterpret_cast<char*>(var->TempValue), var->FieldSize);
		else {
			std::string* str = static_cast<std::string*>(var->TempValue);
			const char* ch = str->c_str();
			unsigned int str_size = static_cast<unsigned int>(str->size());
			stream->writeString(*str);
		}
	}
}