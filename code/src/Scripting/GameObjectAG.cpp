#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/world/World.h"
#include "../../headers/world/go_properties.h"

void Engine::bindGameObjectSDK(AGScriptMgr* mgr) {
	//Regiter base GameObject
	mgr->RegisterObjectType(GAME_OBJECT_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectBehaviour(GAME_OBJECT_TYPE_NAME, asBEHAVE_FACTORY, "GameObject@ new_obj()", asFUNCTION(new_as_ref_T<Engine::GameObject>), asCALL_CDECL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "void setActive(bool)", asMETHOD(Engine::GameObject, setActive), asCALL_THISCALL);
	mgr->RegisterObjectProperty(GAME_OBJECT_TYPE_NAME, "int propsNum", offsetof(Engine::GameObject, props_num));
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "string getLabel()", asMETHOD(Engine::GameObject, getLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "void setLabel(string)", asMETHOD(Engine::GameObject, setLabel), asCALL_THISCALL);

	//Register GameObjectProperty class
	mgr->RegisterObjectType(GAME_OBJECT_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);	
}

void Engine::bindWorldSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(WORLD_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectMethod(WORLD_TYPE_NAME, std::string(GAME_OBJECT_TYPE_NAME) +  "@ findObject(string)", asMETHOD(Engine::World, getGameObjectByLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(WORLD_TYPE_NAME, "void removeObject(GameObject@)", asMETHOD(Engine::World, removeObject), asCALL_THISCALL);
	mgr->RegisterObjectMethod(WORLD_TYPE_NAME, std::string(CAM_TYPE_NAME) + "@ getCamera()", asMETHOD(Engine::World, getCameraPtr), asCALL_THISCALL);

	mgr->RegisterObjectProperty(GAME_OBJECT_TYPE_NAME, "World@ world", offsetof(Engine::GameObject, world_ptr));
}

void Engine::bindGameObjectPropertiesSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(TRANSFORM_PROP_TYPE_NAME) + "@ transform()", asMETHOD(Engine::GameObject, getPropertyPtr<TransformProperty>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(LIGHTSOURCE_PROP_TYPE_NAME) + "@ light()", asMETHOD(Engine::GameObject, getPropertyPtr<LightsourceProperty>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(AUDSOURCE_PROP_TYPE_NAME) + "@ audio()", asMETHOD(Engine::GameObject, getPropertyPtr<AudioSourceProperty>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(CHAR_CTRL_PROP_TYPE_NAME) + "@ char_ctrl()", asMETHOD(Engine::GameObject, getPropertyPtr<CharacterControllerProperty>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(ANIM_PROP_TYPE_NAME) + "@ anim()", asMETHOD(Engine::GameObject, getPropertyPtr<AnimationProperty>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(MESH_PROP_TYPE_NAME) + "@ mesh()", asMETHOD(Engine::GameObject, getPropertyPtr<MeshProperty>), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(MAT_PROP_TYPE_NAME) + "@ material()", asMETHOD(Engine::GameObject, getPropertyPtr<MaterialProperty>), asCALL_THISCALL);
}
