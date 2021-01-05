#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/input/Input.hpp"
#include <SDL2/SDL.h>

void bindKeyCodes(Engine::AGScriptMgr* mgr) {
	mgr->RegisterEnum(KEY_TYPE_NAME);
	for (char i = 0; i < 26; i++) {
		char letter = 'A' + i;
		std::string name = "KEY_";
		name.push_back(letter);
		mgr->RegisterEnumValue(KEY_TYPE_NAME, name.c_str(), SDLK_a + i);
	}
	mgr->RegisterEnumValue(KEY_TYPE_NAME, "KEY_ESCAPE", SDLK_ESCAPE);
	mgr->RegisterEnumValue(KEY_TYPE_NAME, "KEY_TAB", SDLK_TAB);
	mgr->RegisterEnumValue(KEY_TYPE_NAME, "KEY_LCTRL", SDLK_LCTRL);
	mgr->RegisterEnumValue(KEY_TYPE_NAME, "KEY_RCTRL", SDLK_RCTRL);
	mgr->RegisterEnumValue(KEY_TYPE_NAME, "KEY_LSHIFT", SDLK_LSHIFT);
	mgr->RegisterEnumValue(KEY_TYPE_NAME, "KEY_LALT", SDLK_LALT);
	mgr->RegisterEnumValue(KEY_TYPE_NAME, "KEY_RALT", SDLK_RALT);
	mgr->RegisterEnumValue(KEY_TYPE_NAME, "KEY_ENTER", SDLK_RETURN);
	mgr->RegisterEnumValue(KEY_TYPE_NAME, "KEY_SPACE", SDLK_SPACE);
}

void bindMouseStateClassSDK(Engine::AGScriptMgr* mgr) {
	mgr->RegisterObjectType(MOUSE_STATE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectProperty(MOUSE_STATE_NAME, "int mouseX", offsetof(Input::MouseState, mouseX));
	mgr->RegisterObjectProperty(MOUSE_STATE_NAME, "int mouseY", offsetof(Input::MouseState, mouseY));
	mgr->RegisterObjectProperty(MOUSE_STATE_NAME, "int relX", offsetof(Input::MouseState, mouseRelX));
	mgr->RegisterObjectProperty(MOUSE_STATE_NAME, "int relY", offsetof(Input::MouseState, mouseRelY));

	mgr->RegisterObjectProperty(MOUSE_STATE_NAME, "int mouseWheelX", offsetof(Input::MouseState, mouseWheelX));
	mgr->RegisterObjectProperty(MOUSE_STATE_NAME, "int mouseWheelY", offsetof(Input::MouseState, mouseWheelY));

	mgr->RegisterObjectProperty(MOUSE_STATE_NAME, "bool isLButtonDown", offsetof(Input::MouseState, isLButtonDown));
	mgr->RegisterObjectProperty(MOUSE_STATE_NAME, "bool isRButtonDown", offsetof(Input::MouseState, isRButtonDown));
	mgr->RegisterObjectProperty(MOUSE_STATE_NAME, "bool isMidButtonDown", offsetof(Input::MouseState, isMidBtnDown));
}

void Engine::bindInputSDK(AGScriptMgr* mgr) {
	bindKeyCodes(mgr);
	bindMouseStateClassSDK(mgr);

	mgr->RegisterGlobalFunction("bool isKeyPressed(Key)", asFUNCTION(Input::isKeyPressed), asCALL_CDECL);
	mgr->RegisterGlobalFunction("bool isKeyHold(Key)", asFUNCTION(Input::isKeyHold), asCALL_CDECL);
	mgr->RegisterGlobalFunction("void SetMouseRelativeMode(bool)", asFUNCTION(Input::SetMouseRelativeMode), asCALL_CDECL);
	mgr->RegisterGlobalFunction("MouseState@ getMouseState()", asFUNCTION(Input::getMouseStatePtr), asCALL_CDECL);
}