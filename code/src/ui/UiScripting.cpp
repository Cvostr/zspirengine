#include <ui/UiManager.hpp>
#include <ui/Layouts/ILayout.hpp>
#include <ui/UiScripting.hpp>

#include <ui/Widgets/Button.hpp>
#include <ui/Layouts/LinearLayout.hpp>

#define VIEWSIZE_TYPE_NAME "ViewSize"
#define VIEWMARGIN_TYPE_NAME "ViewMargin"
#define VIEWPOSITION_TYPE_NAME "ViewPosition"

#define VIEW_TYPE_NAME "View"
#define LAYOUT_TYPE_NAME "Layout"
#define LINEARLAYOUT_TYPE_NAME "LinearLayout"
#define LAYOUT_ORIENTATION_NAME "LayoutOrientation"
#define FREELAYOUT_TYPE_NAME "FreeLayout"

#define WIDGET_TYPE_NAME "Widget"
#define BUTTON_TYPE_NAME "Button"

static void CreateViewSize(int Width, int Height, Engine::ViewSize* ptr) {
	new (ptr) Engine::ViewSize(Width, Height);
}

static void CreateViewPosition(int posX, int posY, Engine::ViewPosition* ptr) {
	new (ptr) Engine::ViewPosition(posX, posY);
}

static void CreateMargin(int Top, int Bottom, int Left, int Right, Engine::ViewMargin* ptr) {
	new (ptr) Engine::ViewMargin(Top, Bottom, Left, Right);
}

void bindButtonSDK(Engine::AGScriptMgr* mgr);
void bindLinearLayoutSDK(Engine::AGScriptMgr* mgr);

void Engine::bindUiSDK(AGScriptMgr* mgr) {

	mgr->RegisterObjectType(VIEWSIZE_TYPE_NAME, sizeof(ViewSize), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<ViewSize>());
	mgr->RegisterObjectBehaviour(VIEWSIZE_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<ViewSize>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(VIEWSIZE_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(int, int)", asFUNCTION(CreateViewSize), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(VIEWSIZE_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<ViewSize>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectProperty(VIEWSIZE_TYPE_NAME, "int Width", offsetof(ViewSize, WIDTH));
	mgr->RegisterObjectProperty(VIEWSIZE_TYPE_NAME, "int Height", offsetof(ViewSize, HEIGHT));

	mgr->RegisterObjectType(VIEWPOSITION_TYPE_NAME, sizeof(ViewPosition), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<ViewPosition>());
	mgr->RegisterObjectBehaviour(VIEWPOSITION_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<ViewPosition>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(VIEWPOSITION_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(int, int)", asFUNCTION(CreateViewPosition), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(VIEWPOSITION_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<ViewPosition>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectProperty(VIEWPOSITION_TYPE_NAME, "int posX", offsetof(ViewPosition, posX));
	mgr->RegisterObjectProperty(VIEWPOSITION_TYPE_NAME, "int posY", offsetof(ViewPosition, posY));

	mgr->RegisterObjectType(VIEWMARGIN_TYPE_NAME, sizeof(ViewMargin), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<ViewMargin>());
	mgr->RegisterObjectBehaviour(VIEWMARGIN_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(new_as_obj_T<ViewMargin>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(VIEWMARGIN_TYPE_NAME, asBEHAVE_CONSTRUCT, "void f(int, int, int, int)", asFUNCTION(CreateMargin), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectBehaviour(VIEWMARGIN_TYPE_NAME, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(del_as_obj_T<ViewMargin>), asCALL_CDECL_OBJLAST);
	mgr->RegisterObjectProperty(VIEWMARGIN_TYPE_NAME, "int marginTop", offsetof(ViewMargin, marginTop));
	mgr->RegisterObjectProperty(VIEWMARGIN_TYPE_NAME, "int marginBottom", offsetof(ViewMargin, marginBottom));
	mgr->RegisterObjectProperty(VIEWMARGIN_TYPE_NAME, "int marginLeft", offsetof(ViewMargin, marginLeft));
	mgr->RegisterObjectProperty(VIEWMARGIN_TYPE_NAME, "int marginRight", offsetof(ViewMargin, marginRight));

	//Bind base view class
	bindViewSDK<Engine::IView>(mgr, VIEW_TYPE_NAME);
	bindLayoutSDK<Engine::ILayout>(mgr, LAYOUT_TYPE_NAME);


	bindLinearLayoutSDK(mgr);
	bindButtonSDK(mgr);
	
}

void bindButtonSDK(Engine::AGScriptMgr* mgr) {
	Engine::bindWidgetSDK<Engine::Button>(mgr, BUTTON_TYPE_NAME);
	mgr->RegisterObjectBehaviour(BUTTON_TYPE_NAME, asBEHAVE_FACTORY, "Button@ f()", asFUNCTION(Engine::new_as_ref_T<Engine::Button>), asCALL_CDECL);
	mgr->RegisterObjectMethod(BUTTON_TYPE_NAME, "bool isClicked()", asMETHOD(Engine::Button, isClicked), asCALL_THISCALL);
	mgr->RegisterObjectMethod(BUTTON_TYPE_NAME, "void SetDefaultSprite(TextureResource@)", asMETHOD(Engine::Button, SetDefaultSprite), asCALL_THISCALL);
	mgr->RegisterObjectMethod(BUTTON_TYPE_NAME, "bool SetHoveredSprite(TextureResource@)", asMETHOD(Engine::Button, SetHoveredSprite), asCALL_THISCALL);
}

void bindLinearLayoutSDK(Engine::AGScriptMgr* mgr) {
	mgr->RegisterEnum(LAYOUT_ORIENTATION_NAME);
	
	mgr->RegisterEnumValue(LAYOUT_ORIENTATION_NAME, "ORIENTATION_HORIZONTAL", Engine::ORIENTATION_HORIZONTAL);
	mgr->RegisterEnumValue(LAYOUT_ORIENTATION_NAME, "ORIENTATION_VERTICAL", Engine::ORIENTATION_VERTICAL);


	Engine::bindLayoutSDK<Engine::LinearLayout>(mgr, LINEARLAYOUT_TYPE_NAME);
	mgr->RegisterObjectBehaviour(LINEARLAYOUT_TYPE_NAME, asBEHAVE_FACTORY, "LinearLayout@ f()", asFUNCTION(Engine::new_as_ref_T<Engine::LinearLayout>), asCALL_CDECL);
	mgr->RegisterObjectProperty(LINEARLAYOUT_TYPE_NAME, "LayoutOrientation Orientation", offsetof(Engine::LinearLayout, Orientation));
}