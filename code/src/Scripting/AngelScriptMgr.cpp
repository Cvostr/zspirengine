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
			int v = 0;
			memcpy(&v, ref, sizeof(int));
			out += std::to_string(v) + " ";
			break;
		}
		case asTYPEID_FLOAT: {
			float f = 0;
			memcpy(&f, ref, sizeof(float));
			out += std::to_string(f) + " ";
			break;
		}
		}
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

AGScriptMgr::AGScriptMgr() {
	ag_engine = asCreateScriptEngine();
	int r = ag_engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	assert(r >= 0);
	RegisterStdString(ag_engine);
	RegisterScriptHandle(ag_engine);
	//Add base script class
	RegisterInterface("ZPScript");

	bindGameObjectSDK(this);
	bindMathSDK(this);
	bindGameObjectPropertySDK<TransformProperty>(this, TRANSFORM_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<LightsourceProperty>(this, LIGHTSOURCE_PROP_TYPE_NAME);
	bindGameObjectPropertySDK<AudioSourceProperty>(this, AUDSOURCE_PROP_TYPE_NAME);
	bindGameObjectPropertiesSDK(this);

	r = RegisterGlobalFunction("void print(?&in, ?&in)", asFUNCTION(printToConsole), asCALL_GENERIC);

	assert(r >= 0);

	//Create Context
	ag_context = ag_engine->CreateContext();
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

int AGScriptMgr::RegisterObjectMethod(const char* obj, const char* declaration, const asSFuncPtr& funcPointer, asDWORD callConv, void* auxiliary, int compositeOffset, bool isCompositeIndirect) {
	return ag_engine->RegisterObjectMethod(obj, declaration, funcPointer, callConv, auxiliary, compositeOffset, isCompositeIndirect);
}

int AGScriptMgr::RegisterGlobalProperty(const char* declaration, void* pointer) {
	return ag_engine->RegisterGlobalProperty(declaration, pointer);
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

asIScriptModule* AGScriptMgr::GetModule(std::string module, asEGMFlags 	flag) {
	return ag_engine->GetModule(module.c_str(), flag);
}