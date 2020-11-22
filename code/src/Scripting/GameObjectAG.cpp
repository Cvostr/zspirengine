#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/world/World.hpp"
#include "../../headers/world/go_properties.h"

void Engine::bindGameObjectSDK(AGScriptMgr* mgr) {
	//Regiter base GameObject
	mgr->RegisterObjectType(GAME_OBJECT_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectBehaviour(GAME_OBJECT_TYPE_NAME, asBEHAVE_FACTORY, "GameObject@ new_obj()", asFUNCTION(new_as_ref_T<Engine::GameObject>), asCALL_CDECL);

	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "void setActive(bool)", asMETHOD(Engine::GameObject, setActive), asCALL_THISCALL);
	mgr->RegisterObjectProperty(GAME_OBJECT_TYPE_NAME, "int propsNum", offsetof(Engine::GameObject, props_num));
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "const string& getLabel()", asMETHOD(Engine::GameObject, getLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "void setLabel(const string&in)", asMETHOD(Engine::GameObject, setLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "ZPScript@ getScriptClass(const string&in)", asMETHOD(Engine::GameObject, getScriptObjectWithName), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "uint64 getChildrenNum()", asMETHOD(Engine::GameObject, getChildrenNum), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "GameObject@ getChild(uint)", asMETHOD(Engine::GameObject, getChild), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "void addChild(GameObject@)", asMETHODPR(Engine::GameObject, addChildObject, (GameObject*, bool), void), asCALL_THISCALL);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, "void removeChild(GameObject@)", asMETHODPR(Engine::GameObject, removeChildObject, (GameObject*), void), asCALL_THISCALL);
	//Register GameObjectProperty class
	mgr->RegisterObjectType(GAME_OBJECT_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);	
}

void Engine::bindWorldSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(WORLD_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectMethod(WORLD_TYPE_NAME, std::string(GAME_OBJECT_TYPE_NAME) +  "@ findObject(const string&in)", asMETHOD(Engine::World, getGameObjectByLabel), asCALL_THISCALL);
	mgr->RegisterObjectMethod(WORLD_TYPE_NAME, std::string(GAME_OBJECT_TYPE_NAME) + "@ getObjectByInd(int)", asMETHOD(Engine::World, getGameObjectByArrayIndex), asCALL_THISCALL);
	mgr->RegisterObjectMethod(WORLD_TYPE_NAME, "void removeObject(GameObject@)", asMETHOD(Engine::World, removeObject), asCALL_THISCALL);
	mgr->RegisterObjectMethod(WORLD_TYPE_NAME, std::string(CAM_TYPE_NAME) + "@ getCamera()", asMETHOD(Engine::World, getCameraPtr), asCALL_THISCALL);

	mgr->RegisterObjectProperty(GAME_OBJECT_TYPE_NAME, "World@ world", offsetof(Engine::GameObject, mWorld));
}

void Engine::bindGameObjectPropertiesSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(TRANSFORM_PROP_TYPE_NAME) + "@ transform()", asFUNCTION(ASGetPropertyPtr<TransformProperty>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(LIGHTSOURCE_PROP_TYPE_NAME) + "@ light()", asFUNCTION(ASGetPropertyPtr<LightsourceProperty>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(AUDSOURCE_PROP_TYPE_NAME) + "@ audio()", asFUNCTION(ASGetPropertyPtr<AudioSourceProperty>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(CHAR_CTRL_PROP_TYPE_NAME) + "@ character()", asFUNCTION(ASGetPropertyPtr<CharacterControllerProperty>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(ANIM_PROP_TYPE_NAME) + "@ anim()", asFUNCTION(ASGetPropertyPtr<AnimationProperty>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(MESH_PROP_TYPE_NAME) + "@ mesh()", asFUNCTION(ASGetPropertyPtr<MeshProperty>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(MAT_PROP_TYPE_NAME) + "@ material()", asFUNCTION(ASGetPropertyPtr<MaterialProperty>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(RIGIDBODY_PROP_TYPE_NAME) + "@ rigidbody()", asFUNCTION(ASGetPropertyPtr<RigidbodyProperty>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(COLLIDER_PROP_TYPE_NAME) + "@ collider()", asFUNCTION(ASGetPropertyPtr<ColliderProperty>), asCALL_CDECL_OBJFIRST);
	mgr->RegisterObjectMethod(GAME_OBJECT_TYPE_NAME, std::string(SHADOWCAST_PROP_TYPE_NAME) + "@ shadow()", asFUNCTION(ASGetPropertyPtr<ShadowCasterProperty>), asCALL_CDECL_OBJFIRST);
}
