#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/ui/UiManager.hpp"
#include "../../headers/ui/Layouts/LinearLayout.hpp"

#define LINEARLAYOUT_TYPE_NAME "LinearLayout"

void Engine::bindUiSDK(AGScriptMgr* mgr) {
	mgr->RegisterObjectType(LINEARLAYOUT_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectBehaviour(LINEARLAYOUT_TYPE_NAME, asBEHAVE_FACTORY, "LinearLayout@ f()", asFUNCTION(new_as_ref_T<Engine::LinearLayout>), asCALL_CDECL);
}