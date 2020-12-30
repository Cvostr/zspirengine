#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/engine/Resources.hpp"
#include "../../headers/game.h"

//Hack to support resources
extern ZSGAME_DATA* game_data;

Engine::ResourceManager* getResourcesMgr() {
	return game_data->resources;
}

void Engine::bindResourceManagerSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(RESOURCEMAN_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

	mgr->RegisterObjectMethod(RESOURCEMAN_TYPE_NAME, std::string(TEXRESOURCE_TYPE_NAME) + "@ getTexture(string)", asMETHOD(Engine::ResourceManager, getTextureByLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(RESOURCEMAN_TYPE_NAME, std::string(MESHRESOURCE_TYPE_NAME) + "@ getMesh(string)", asMETHOD(Engine::ResourceManager, getMeshByLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(RESOURCEMAN_TYPE_NAME, std::string(AUDRESOURCE_TYPE_NAME) + "@ getAudio(string)", asMETHOD(Engine::ResourceManager, getAudioByLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(RESOURCEMAN_TYPE_NAME, std::string(ANIMRESOURCE_TYPE_NAME) + "@ getAnimation(string)", asMETHOD(Engine::ResourceManager, getAnimationByLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(RESOURCEMAN_TYPE_NAME, std::string(MATRESOURCE_TYPE_NAME) + "@ getMaterial(string)", asMETHOD(Engine::ResourceManager, getMaterialByLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(RESOURCEMAN_TYPE_NAME, std::string(FONTRESOURCE_TYPE_NAME) + "@ getFont(string)", asMETHOD(Engine::ResourceManager, getFontByLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(RESOURCEMAN_TYPE_NAME, std::string(PREFABRESOURCE_TYPE_NAME) + "@ getPrefab(string)", asMETHOD(Engine::ResourceManager, getPrefabByLabel), asCALL_THISCALL);
	//register global var to acess all resources
	mgr->RegisterGlobalFunction(std::string(RESOURCEMAN_TYPE_NAME) + "@ getResources()", asFUNCTION(getResourcesMgr), asCALL_CDECL);
}

void Engine::bindResourceSDK(const char* className, AGScriptMgr* mgr) {

	mgr->RegisterObjectType(className, 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectProperty(className, "string label", offsetof(ZsResource, rel_path));
	mgr->RegisterObjectProperty(className, "string path", offsetof(ZsResource, resource_label));
}

void Engine::bindResourcesSDK(AGScriptMgr* mgr) {
	bindResourceSDK(TEXRESOURCE_TYPE_NAME, mgr);
	bindResourceSDK(MESHRESOURCE_TYPE_NAME, mgr);
	bindResourceSDK(AUDRESOURCE_TYPE_NAME, mgr);
	bindResourceSDK(ANIMRESOURCE_TYPE_NAME, mgr);
	bindResourceSDK(MATRESOURCE_TYPE_NAME, mgr);
	bindResourceSDK(FONTRESOURCE_TYPE_NAME, mgr);
	bindResourceSDK(PREFABRESOURCE_TYPE_NAME, mgr);
}