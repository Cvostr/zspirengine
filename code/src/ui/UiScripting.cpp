#include "../../headers/Scripting/AngelScriptMgr.h"
#include "../../headers/ui/UiManager.hpp"
#include "../../headers/ui/Layouts/LinearLayout.hpp"


#define VIEWSIZE_TYPE_NAME "ViewSize"
#define VIEWMARGIN_TYPE_NAME "ViewMargin"
#define VIEWPOSITION_TYPE_NAME "ViewPosition"

#define VIEW_TYPE_NAME "View"

#define LAYOUT_TYPE_NAME "Layout"
#define LINEARLAYOUT_TYPE_NAME "LinearLayout"

#define WIDGET_TYPE_NAME "Widget"
#define BUTTON_TYPE_NAME "Button"

void Engine::bindUiSDK(AGScriptMgr* mgr) {

	mgr->RegisterObjectType(VIEWSIZE_TYPE_NAME, sizeof(ViewSize), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<ViewSize>());
	mgr->RegisterObjectBehaviour(VIEWSIZE_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<ViewSize>), asCALL_CDECL_OBJLAST);
	//mgr->RegisterObjectBehaviour(VEC3_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(CreateVec3), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(VIEWSIZE_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<ViewSize>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectProperty(VIEWSIZE_TYPE_NAME, "float Width", offsetof(ViewSize, WIDTH));
	mgr->RegisterObjectProperty(VIEWSIZE_TYPE_NAME, "float Height", offsetof(ViewSize, HEIGHT));

	//mgr->RegisterObjectType(LINEARLAYOUT_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
	//mgr->RegisterObjectBehaviour(LINEARLAYOUT_TYPE_NAME, asBEHAVE_FACTORY, "LinearLayout@ f()", asFUNCTION(new_as_ref_T<Engine::LinearLayout>), asCALL_CDECL);
}