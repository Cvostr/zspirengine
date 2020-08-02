#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/world/go_properties.h"

//Hack to support resources
extern ZSGAME_DATA* game_data;

using namespace Engine;

void print(std::string& msg)
{
	printf("%s", msg.c_str());
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
		case asMSGTYPE_ERROR:
			se_type = ScriptErrorType::SE_TYPE_ERROR;
	default:
		break;
	}

	std::string _msg = "(" + std::to_string(*msg->section) + ") " + "(" + std::to_string(msg->row)
		+ ":" + std::to_string(msg->col) + ") " + msg->message;

	game_data->out_manager->addConsoleLog(le_type, _msg, se_type);
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
	bindGameObjectPropertySDK<TransformProperty>(this, "TransformProperty");

	bindGameObjectPropertiesSDK(this);

	r = ag_engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);

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

bool AGScriptMgr::registerGlobalFunc(std::string func_name,
	const asSFuncPtr& funcPointer,
	asDWORD 	callConv,
	void* auxiliary) {

	int result = ag_engine->RegisterGlobalFunction(func_name.c_str(), funcPointer, asCALL_CDECL);

	return (result >= 0);
}

asIScriptModule* AGScriptMgr::GetModule(std::string module, asEGMFlags 	flag) {
	return ag_engine->GetModule(module.c_str(), flag);
}