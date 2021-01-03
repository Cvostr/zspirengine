#pragma once

#include "View.hpp"
#include "../../headers/Scripting/AngelScriptMgr.h"

namespace Engine {

    template<class T>
    void bindViewSDK(AGScriptMgr* mgr, const char* obj_type) {
        mgr->RegisterObjectType(obj_type, 0, asOBJ_REF | asOBJ_NOCOUNT);
        mgr->RegisterObjectMethod(obj_type, "View@ opImplCast()", asFUNCTION((ag_cast<T, IView>)), asCALL_CDECL_OBJLAST);
        mgr->RegisterObjectMethod(obj_type, "void resize(int, int)", asMETHODPR(T, resize, (int, int), void), asCALL_THISCALL);
        mgr->RegisterObjectMethod(obj_type, "void move(int, int)", asMETHODPR(T, move, (int, int), void), asCALL_THISCALL);
        
        mgr->RegisterObjectMethod(obj_type, "ViewSize& GetSize()", asMETHOD(T, GetSize), asCALL_THISCALL);
        mgr->RegisterObjectMethod(obj_type, "ViewPosition& GetPos()", asMETHOD(T, GetPos), asCALL_THISCALL);
    }

    template<class T>
    void bindWidgetSDK(AGScriptMgr* mgr, const char* obj_type) {
        bindViewSDK<T>(mgr, obj_type);

        mgr->RegisterObjectMethod(obj_type, "void setText(string)", asMETHOD(T, setText), asCALL_THISCALL);
        mgr->RegisterObjectMethod(obj_type, "void setTextColor(RGBAColor)", asMETHOD(T, setTextColor), asCALL_THISCALL);
    }

    template<class T>
    void bindLayoutSDK(AGScriptMgr* mgr, const char* obj_type) {
        bindViewSDK<T>(mgr, obj_type);

        mgr->RegisterObjectMethod(obj_type, "void AddView(View@)", asMETHOD(T, AddView), asCALL_THISCALL);
        mgr->RegisterObjectMethod(obj_type, "void RemoveView(View@)", asMETHOD(T, RemoveView), asCALL_THISCALL);
    }
}