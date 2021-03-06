#include <Scripting/AngelScriptMgr.h>

#include <world/ObjectsComponents/AudioSourceComponent.hpp>
#include <world/ObjectsComponents/LightSourceComponent.hpp>
#include <world/ObjectsComponents/ShadowCasterComponent.hpp>
#include <world/ObjectsComponents/MaterialComponent.hpp>
#include <world/ObjectsComponents/MeshComponent.hpp>
#include <world/ObjectsComponents/AnimationComponent.hpp>
#include <world/ObjectsComponents/CharacterController.hpp>
#include <world/ObjectsComponents/RigidbodyComponent.hpp>
#include <world/ObjectsComponents/ColliderComponent.hpp>
#include <world/ObjectsComponents/CameraComponent.hpp>
#include <world/ObjectsComponents/ParticleEmitterComponent.hpp>

//Hack to support resources
extern ZSGAME_DATA* game_data;
//Hack to support meshes
extern ZSpireEngine* engine_ptr;

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

Time* GetTimeMgr() {
	return game_data->time;
}

UiManager* GetUi() {
	return game_data->ui_manager;
}

Window* GetWindow() {
	return game_data->window;
}

void LoadWorld(std::string World) {
	game_data->wlm->ScheduleWorldLoad(World);
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
	bindResourcesSDK(this);
	bindWorldSDK(this);
	bindResourceManagerSDK(this);
	bindInputSDK(this);
	bindFilesSDK(this);
	bindUiSDK(this);
	bindGameObjectPropertySDK<TransformProperty>(this, TRANSFORM_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<LightsourceComponent>(this, LIGHTSOURCE_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<AudioSourceProperty>(this, AUDSOURCE_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<CharacterControllerProperty>(this, CHAR_CTRL_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<AnimationProperty>(this, ANIM_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<MeshProperty>(this, MESH_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<MaterialProperty>(this, MAT_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<RigidbodyProperty>(this, RIGIDBODY_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<ColliderProperty>(this, COLLIDER_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<ShadowCasterProperty>(this, SHADOWCAST_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<CameraComponent>(this, CAMERA_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<ParticleEmitterComponent>(this, PARTICLE_EMITTER_PROP_TYPE_NAME);

	bindGameObjectPropertiesSDK(this);

	//Bind Time mgr
	Engine::Time::bindAngelScript(this);
	RegisterGlobalFunction("Time@ GetTime()", asFUNCTION(GetTimeMgr), asCALL_CDECL);

	Engine::UiManager::bindAngelScript(this);
	RegisterGlobalFunction("Ui@ GetUi()", asFUNCTION(GetUi), asCALL_CDECL);

	Engine::Window::bindAngelScript(this);
	RegisterGlobalFunction("Window@ GetWindow()", asFUNCTION(GetWindow), asCALL_CDECL);

	RegisterGlobalFunction("void LoadWorld(string)", asFUNCTION(LoadWorld), asCALL_CDECL);

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
	mBuilder.StartNewModule(getAgScriptEngine(), ZPS_DEFAULT_MODULE_NAME);
	mModule = getAgScriptEngine()->GetModule(ZPS_DEFAULT_MODULE_NAME, asGM_ONLY_IF_EXISTS);
	//Set builder #include callback
	mBuilder.SetIncludeCallback(onInclude, this);
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
	ModuleCompiled(false),
	mStreamWrite(new BytecodeStreamWriter)
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

int AGScriptMgr::RegisterObjectProperty(const char* obj, std::string declaration, int byteOffset, int compositeOffset, bool isCompositeIndirect ) {
	return mEngine->RegisterObjectProperty(obj, declaration.c_str(), byteOffset, compositeOffset, isCompositeIndirect);
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
	
	//Check error
	if (result < 0) {
		CompileError = true;
		game_data->out_manager->spawnRuntimeError(RuntimeErrorType::RE_TYPE_SCRIPT_ERROR);
	}
	else {
		SaveByteCode();
		CompileError = false;	
		ModuleCompiled = true;
	}

	UpdateClassesNames();

	return (result >= 0);
}

int Engine::onInclude (const char* include, const char* from, CScriptBuilder* builder, void* userParam) {
	Engine::ScriptResource* DependencyRes = game_data->resources->getScriptByLabel(include);
	//include dependency
	AGScriptMgr* mgr = static_cast<AGScriptMgr*>(userParam);
	int result = mgr->AddScriptFile(DependencyRes);
	return result;
}

int AGScriptMgr::SaveByteCode() {
	if (game_data->isEditor) {
		mStreamWrite->open_for_write();
		mModule->SaveByteCode(this->mStreamWrite, true);
		mStreamWrite->close();
	}
	return 1;
}

int AGScriptMgr::AddScriptFile(Engine::ScriptResource* res) {
	const char* script_str = res->script_content.c_str();
	unsigned int scr_file_offset = 0;
	int result = mBuilder.AddSectionFromMemory(res->resource_label.c_str(), script_str, res->size);
	
	return result;
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

void BytecodeStreamWriter::open_for_write() {
	if (!game_data->isEditor) 
		return;
	std::string path = engine_ptr->desc->game_dir + "/.cache/scriptcache";
	f = fopen(path.c_str(), "wb");
}

int BytecodeStreamWriter::Write(const void* ptr, asUINT size)
{
	if (size == 0 && !game_data->isEditor) return 0 ;
	fwrite(ptr, size, 1, f);
	//fclose(f);
	return 1;
}
int BytecodeStreamWriter::Read(void* ptr, asUINT size)
{
	if (size == 0) return 0;
	// fread(ptr, size, 1, f);
	return 1;
}