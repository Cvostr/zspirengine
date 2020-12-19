#include "../../headers/engine/Time.hpp"
#include <iostream>

using namespace Engine;

Time::Time() {
	mNow = SDL_GetTicks();
	mLast = 0;
	mFramesCount = 0;
}
void Time::Tick() {
	mFramesCount++;
	mLast = mNow;
	mNow = SDL_GetTicks();
	DeltaTime = (float)(mNow - mLast) / 1000.f;
}
float Time::GetDeltaTime() {
	return DeltaTime;
}
uint64_t Time::GetFramesCount() {
	return mFramesCount;
}

void Time::bindAngelScript(Engine::AGScriptMgr* mgr) {
	mgr->RegisterObjectType("Time", 0, asOBJ_REF | asOBJ_NOCOUNT);
	mgr->RegisterObjectMethod("Time", "float GetDeltaTime()", asMETHOD(Time, GetDeltaTime), asCALL_THISCALL);
	mgr->RegisterObjectMethod("Time", "uint64 GetFramesCount()", asMETHOD(Time, GetFramesCount), asCALL_THISCALL);
}