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
				ZSVECTOR3* vec = static_cast<ZSVECTOR3*>(ref);
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
	return ag_engine;
}

asIScriptContext* AGScriptMgr::getAgScriptContext() {
	return ag_context;
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
	ag_engine = asCreateScriptEngine();

	ag_engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	Engine::RegisterStdString(ag_engine);
	RegisterScriptHandle(ag_engine);
	RegisterScriptFile(ag_engine);
	//Add base script class
	RegisterInterface("ZPScript");

	bindGameObjectSDK(this);
	bindMathSDK(this);
	bindWorldSDK(this);
	bindResourcesSDK(this);
	bindResourceManagerSDK(this);
	bindInputSDK(this);
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
	ag_context = ag_engine->CreateContext();
}
void AGScriptMgr::recreate_Engine() {
	//close old engine
	ag_context->Release();
	ag_engine->ShutDownAndRelease();
	//Create new engine instance
	create_Engine();
}

AGScriptMgr::AGScriptMgr() {
	create_Engine();
}

AGScriptMgr::~AGScriptMgr() {
	ag_context->Release();
	ag_engine->ShutDownAndRelease();
}

int AGScriptMgr::RegisterInterface(const char* name) {
	return ag_engine->RegisterInterface(name);
}

int AGScriptMgr::RegisterObjectType(const char* name, int byteSize, asDWORD flags) {
	return ag_engine->RegisterObjectType(name, byteSize, flags);
}

int AGScriptMgr::RegisterObjectBehaviour(const char* obj, asEBehaviours behaviour, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary, int compositeOffset, bool isCompositeIndirect) {
	return ag_engine->RegisterObjectBehaviour(obj, behaviour, declaration, funcPointer, callConv, auxiliary, compositeOffset, isCompositeIndirect);
}

int AGScriptMgr::RegisterObjectMethod(const char* obj, std::string declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary, int compositeOffset, bool isCompositeIndirect) {
	return ag_engine->RegisterObjectMethod(obj, declaration.c_str(), funcPointer, callConv, auxiliary, compositeOffset, isCompositeIndirect);
}

int AGScriptMgr::RegisterGlobalProperty(std::string declaration, void* pointer) {
	return ag_engine->RegisterGlobalProperty(declaration.c_str(), pointer);
}

int AGScriptMgr::RegisterObjectProperty(const char* obj, const char* declaration, int byteOffset, int compositeOffset, bool isCompositeIndirect ) {
	return ag_engine->RegisterObjectProperty(obj, declaration, byteOffset, compositeOffset, isCompositeIndirect);
}

bool AGScriptMgr::RegisterGlobalFunction(std::string func_name,
	const asSFuncPtr& funcPointer,
	asDWORD 	callConv,
	void* auxiliary) {

	int result = ag_engine->RegisterGlobalFunction(func_name.c_str(), funcPointer, callConv, auxiliary);

	return (result >= 0);
}

int AGScriptMgr::RegisterEnum(const char* type) {
	return ag_engine->RegisterEnum(type);
}
int AGScriptMgr::RegisterEnumValue(const char* type, const char* name, int value) {
	return ag_engine->RegisterEnumValue(type, name, value);
}

asIScriptModule* AGScriptMgr::GetModule(std::string module, asEGMFlags 	flag) {
	return ag_engine->GetModule(module.c_str(), flag);
}