#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/engine/resources.h"
#include "../../headers/game.h"

//Hack to support resources
extern ZSGAME_DATA* game_data;

void Engine::bindResourceManagerSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(RESOURCEMAN_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

	mgr->RegisterObjectMethod(RESOURCEMAN_TYPE_NAME, "TextureResource@ getTexture(string)", asMETHOD(Engine::ResourceManager, getTextureByLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(RESOURCEMAN_TYPE_NAME, "MeshResource@ getMesh(string)", asMETHOD(Engine::ResourceManager, getMeshByLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(RESOURCEMAN_TYPE_NAME, "AudioResource@ getAudio(string)", asMETHOD(Engine::ResourceManager, getAudioByLabel), asCALL_THISCALL);
	//register global var to acess all resources
	mgr->RegisterGlobalProperty(std::string(RESOURCEMAN_TYPE_NAME) + "@ resources", game_data->resources);
}

void Engine::bindResourceSDK(const char* className, AGScriptMgr* mgr) {

	mgr->RegisterObjectType(className, 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectProperty(className, "string label", offsetof(ZsResource, rel_path));
	mgr->RegisterObjectProperty(className, "string path", offsetof(ZsResource, resource_label));
}
