#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/world/go_properties.h"

//Hack to support resources
extern ZSGAME_DATA* game_data;

using namespace Engine;

void printToConsole(asIScriptGeneric* gen)
{	
	unsigned int args = gen->GetArgCount();
	std::string out = "";

	for (unsigned int arg_i = 0; arg_i < args; arg_i++) {
		void* ref = gen->GetArgAddress(arg_i);
		int typeId = gen->GetArgTypeId(arg_i);

		switch (typeId) {
			case asTYPEID_INT32: {
				int* v = static_cast<int*>(ref);
				out += std::to_string(*v);
				break;
			}
			case asTYPEID_FLOAT: {
				float* f = static_cast<float*>(ref);
				out += std::to_string(*f);
				break;
			}
			case asTYPEID_BOOL: {
				bool* f = static_cast<bool*>(ref);
				out += (*f == true) ? "true" : "false";
				break;
			}
			case AG_STRING: {
				std::string* str = static_cast<std::string*>(ref);
				out += *str;
				break;
			}
			case AG_VECTOR3: {
				Vec3* vec = static_cast<Vec3*>(ref);
				out += "(" + std::to_string(vec->X) + ", " + std::to_string(vec->Y) + ", " + std::to_string(vec->Z) + ")";
				break;
			}
		}
		//Add space
		out += " ";
	}
	game_data->out_manager->addConsoleLog(LogEntryType::LE_TYPE_SCRIPT_MESSAGE, out);
}

asIScriptEngine* AGScriptMgr::getAgScriptEngine() {
	return mEngine;
}

asIScriptContext* AGScriptMgr::getAgScriptContext() {
	return mContext;
}

asIScriptModule* AGScriptMgr::getAgScriptModule() {
	return mModule;
}

void MessageCallback(const asSMessageInfo* msg, void* param)
{
	LogEntryType le_type = LogEntryType::LE_TYPE_SCRIPT_ERROR;
	ScriptErrorType se_type = ScriptErrorType::SE_TYPE_NONE;

	switch (msg->type)
	{
		case asMSGTYPE_WARNING:
			se_type = ScriptErrorType::SE_TYPE_WARNING;
			break;
		case asMSGTYPE_ERROR:
			se_type = ScriptErrorType::SE_TYPE_ERROR;
			break;
	default:
		break;
	}
	//format string
	std::string _msg = "(" + std::string(msg->section) + ") " + "(" + std::to_string(msg->row)
		+ ":" + std::to_string(msg->col) + ") " + msg->message;
	//send message string
	if(game_data->out_manager != nullptr)
		game_data->out_manager->addConsoleLog(le_type, _msg, se_type);

	if (msg->type == asMSGTYPE_ERROR && game_data->out_manager != nullptr)
		game_data->out_manager->spawnRuntimeError(RuntimeErrorType::RE_TYPE_SCRIPT_ERROR);
}

void AGScriptMgr::create_Engine() {
	mEngine = asCreateScriptEngine();

	mEngine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	Engine::RegisterStdString(mEngine);
	RegisterScriptHandle(mEngine);
	RegisterScriptFile(mEngine);
	//Add base script class
	RegisterInterface("ZPScript");

	bindGameObjectSDK(this);
	bindMathSDK(this);
	bindWorldSDK(this);
	bindResourcesSDK(this);
	bindResourceManagerSDK(this);
	bindInputSDK(this);
	bindFilesSDK(this);
	bindUiSDK(this);
	bindGameObjectPropertySDK<TransformProperty>(this, TRANSFORM_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<LightsourceProperty>(this, LIGHTSOURCE_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<AudioSourceProperty>(this, AUDSOURCE_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<CharacterControllerProperty>(this, CHAR_CTRL_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<AnimationProperty>(this, ANIM_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<MeshProperty>(this, MESH_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<MaterialProperty>(this, MAT_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<RigidbodyProperty>(this, RIGIDBODY_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<ColliderProperty>(this, COLLIDER_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<ShadowCasterProperty>(this, SHADOWCAST_PROP_TYPE_NAME);

	bindGameObjectPropertiesSDK(this);

	std::string args = "?&in";
	for (unsigned int i = 0; i < 9; i++) {
		std::string func = "void print(" + args + ")";
		RegisterGlobalFunction(func, asFUNCTION(printToConsole), asCALL_GENERIC);
		args += ", ?&in";
	}

	//Create Context
	mContext = mEngine->CreateContext();
	CreateModule();
}

void AGScriptMgr::CreateModule() {
	if (ModuleCompiled) {
		mModule->Discard();
	}
	//Create module
	mBuilder.StartNewModule(getAgScriptEngine(), "ZSPscriptCore");
	mModule = getAgScriptEngine()->GetModule("ZSPscriptCore", asGM_ONLY_IF_EXISTS);
}

void AGScriptMgr::recreate_Engine() {
	//close old engine
	mContext->Release();
	mEngine->ShutDownAndRelease();
	//Create new engine instance
	create_Engine();
}

AGScriptMgr::AGScriptMgr() :
	CompileError(false),
	ModuleCompiled(false)
{
	create_Engine();
}

AGScriptMgr::~AGScriptMgr() {
	mContext->Release();
	mEngine->ShutDownAndRelease();
}

int AGScriptMgr::RegisterInterface(const char* name) {
	return mEngine->RegisterInterface(name);
}

int AGScriptMgr::RegisterObjectType(const char* name, int byteSize, asDWORD flags) {
	return mEngine->RegisterObjectType(name, byteSize, flags);
}

int AGScriptMgr::RegisterObjectBehaviour(const char* obj, asEBehaviours behaviour, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary, int compositeOffset, bool isCompositeIndirect) {
	return mEngine->RegisterObjectBehaviour(obj, behaviour, declaration, funcPointer, callConv, auxiliary, compositeOffset, isCompositeIndirect);
}

int AGScriptMgr::RegisterObjectMethod(const char* obj, std::string declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary, int compositeOffset, bool isCompositeIndirect) {
	return mEngine->RegisterObjectMethod(obj, declaration.c_str(), funcPointer, callConv, auxiliary, compositeOffset, isCompositeIndirect);
}

int AGScriptMgr::RegisterGlobalProperty(std::string declaration, void* pointer) {
	return mEngine->RegisterGlobalProperty(declaration.c_str(), pointer);
}

int AGScriptMgr::RegisterObjectProperty(const char* obj, const char* declaration, int byteOffset, int compositeOffset, bool isCompositeIndirect ) {
	return mEngine->RegisterObjectProperty(obj, declaration, byteOffset, compositeOffset, isCompositeIndirect);
}

bool AGScriptMgr::RegisterGlobalFunction(std::string func_name,
	const asSFuncPtr& funcPointer,
	asDWORD 	callConv,
	void* auxiliary) {

	int result = mEngine->RegisterGlobalFunction(func_name.c_str(), funcPointer, callConv, auxiliary);

	return (result >= 0);
}

int AGScriptMgr::RegisterEnum(const char* type) {
	return mEngine->RegisterEnum(type);
}
int AGScriptMgr::RegisterEnumValue(const char* type, const char* name, int value) {
	return mEngine->RegisterEnumValue(type, name, value);
}

bool AGScriptMgr::AddScriptFiles() {

	CreateModule();

	for (unsigned int i = 0; i < game_data->resources->getResourcesSize(); i++) {
		ZsResource* res_ptr = game_data->resources->getResourceByIndex(i);
		if (res_ptr->resource_type == RESOURCE_TYPE::RESOURCE_TYPE_SCRIPT) {
			Engine::ScriptResource* script = static_cast<Engine::ScriptResource*>(res_ptr);
			AddScriptFile(script);
			script->ClassName = script->resource_label;
			GetFileName(script->ClassName);
		}
	}

	int result = mBuilder.BuildModule();

	if (result < 0) {
		CompileError = true;
		game_data->out_manager->spawnRuntimeError(RuntimeErrorType::RE_TYPE_SCRIPT_ERROR);
	}
	else {
		CompileError = false;
		ModuleCompiled = true;
	}

	UpdateClassesNames();

	return (result >= 0);
}

bool AGScriptMgr::AddScriptFile(Engine::ScriptResource* res) {
	const char* script_str = res->script_content.c_str();
	unsigned int scr_file_offset = 0;
	int result = mBuilder.AddSectionFromMemory(res->resource_label.c_str(), script_str, res->size);
	std::string line;
	//read first script line
	readLine(line, script_str, scr_file_offset);
	bool Result = true;
	while (startsWith(line, "//use")) {
		const char* dep_str_c = line.c_str() + 6;
		std::string dep_str = std::string(dep_str_c);
		dep_str.pop_back();
		Engine::ScriptResource* DependencyRes = game_data->resources->getScriptByLabel(dep_str);
		//include dependency
		Result = AddScriptFile(DependencyRes);
		readLine(line, script_str, scr_file_offset);
	}
	return Result;
}

void AGScriptMgr::UpdateClassesNames() {
	ZPSClassInfos.clear();

	int tc = mModule->GetObjectTypeCount();
	for (int n = 0; n < tc; n++)
	{
		bool found = false;
		asITypeInfo* type = mModule->GetObjectTypeByIndex(n);
		int ic = type->GetInterfaceCount();
		for (int i = 0; i < ic; i++)
		{
			if (strcmp(type->GetInterface(i)->GetName(), "ZPScript") == 0)
			{
				ZPSClassDesc* desc = new ZPSClassDesc;
				desc->Name = type->GetName();
				desc->Info = type;
				//Get class fields amount
				desc->ClassFieldsNum = type->GetPropertyCount();
				for (unsigned int field_i = 0; field_i < desc->ClassFieldsNum; field_i++) {
					ClassFieldDesc cf_desc;
					const char* _Name;
					type->GetProperty(field_i, &_Name, &cf_desc.typeID, &cf_desc.isPrivate, &cf_desc.isProtected);
					cf_desc.name = std::string(_Name);
					cf_desc.index = field_i;
					desc->Fields.push_back(cf_desc);
				}

				ZPSClassInfos.push_back(desc);
			}
		}
	}
	for (unsigned int i = 0; i < ZPSClassInfos.size(); i ++) {
		asITypeInfo* __ClassInfo;
		CreateClass(ZPSClassInfos[i]->Name, new GameObject, &ZPSClassInfos[i]->__SampleObject, &__ClassInfo);
	}
}

bool AGScriptMgr::CreateClass(std::string ClassName, Engine::GameObject* obj, asIScriptObject** ClassObj, asITypeInfo** ClassInfo) {
	int result = 0;

	for (unsigned int i = 0; i < ZPSClassInfos.size(); i++) {
		asITypeInfo* info = ZPSClassInfos[i]->Info;
		if (strcmp(info->GetName(), ClassName.c_str()) == 0)
			*ClassInfo = info;
	}

	std::string construct_str = ClassName + " @" + ClassName + "(GameObject@)";
	asIScriptFunction* factory = (*ClassInfo)->GetFactoryByDecl(construct_str.c_str());
	//Allocate class by Constructor
	{
		mContext->Prepare(factory);
		if (obj != nullptr)
			mContext->SetArgObject(0, obj);
		result = mContext->Execute();
	}
	//Get returned created class
	*ClassObj = *(asIScriptObject**)mContext->GetAddressOfReturnValue();
	(*ClassObj)->AddRef();

	return true;
}

ZPSClassDesc* AGScriptMgr::GetClassDesc(std::string ClassName) {
	for (unsigned int i = 0; i < ZPSClassInfos.size(); i++) {
		asITypeInfo* info = ZPSClassInfos[i]->Info;
		if (strcmp(info->GetName(), ClassName.c_str()) == 0)
			return ZPSClassInfos[i];
	}
	return nullptr;
}

ClassFieldDesc* ZPSClassDesc::GetSuitableDesc(std::string Name, int TypeID, unsigned int index) {
	for (unsigned int i = 0; i < this->Fields.size(); i++) {
		ClassFieldDesc* desc = &Fields[i];

		if (desc->index == index && desc->typeID == TypeID && desc->name.compare(Name) == 0)
			return desc;
		if (desc->typeID == TypeID && desc->name.compare(Name) == 0)
			return desc;
	}
	return nullptr;
}