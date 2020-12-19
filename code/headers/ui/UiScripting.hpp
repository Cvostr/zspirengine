#pragma once

#include "View.hpp"
#include "../../headers/Scripting/AngelScriptMgr.h"

namespace Engine {

    template<class T>
    void bindViewSDK(AGScriptMgr* mgr, const char* obj_type) {
        int result = 0;

        result = mgr->RegisterObjectType(obj_type, 0, asOBJ_REF | asOBJ_NOCOUNT);
        mgr->RegisterObjectMethod(obj_type, "View@ opImplCast()", asFUNCTION((ag_cast<T, IView>)), asCALL_CDECL_OBJLAST);
        result = mgr->RegisterObjectMethod(obj_type, "void resize(uint, uint)", asMETHODPR(T, resize, (unsigned int, unsigned int), void), asCALL_THISCALL);
        result = mgr->RegisterObjectMethod(obj_type, "void move(uint, uint)", asMETHODPR(T, move, (unsigned int, unsigned int), void), asCALL_THISCALL);
        
        result = mgr->RegisterObjectMethod(obj_type, "ViewSize& GetSize()", asMETHOD(T, GetSize), asCALL_THISCALL);
        result = mgr->RegisterObjectMethod(obj_type, "ViewPosition& GetPos()", asMETHOD(T, GetPos), asCALL_THISCALL);
    }

    template<class T>
    void bindLayoutSDK(AGScriptMgr* mgr, const char* obj_type) {
        int result = 0;
        
        bindViewSDK<T>(mgr, obj_type);

        result = mgr->RegisterObjectMethod(obj_type, "void AddView(View@)", asMETHOD(T, AddView), asCALL_THISCALL);
        result = mgr->RegisterObjectMethod(obj_type, "void RemoveView(View@)", asMETHOD(T, RemoveView), asCALL_THISCALL);
    }
}