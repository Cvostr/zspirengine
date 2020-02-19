#ifndef ZSENSDK_H
#define ZSENSDK_H


#include <string>
#include "../world/World.h"

extern "C" {
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>

#include "LuaScript.h"
#include "../input/zs-input.h"

namespace EZSENSDK {

namespace _Engine {
    void loadWorldFromFile(std::string file);
}

namespace Debug{
    void Log(std::string text);
}

namespace Math{
    ZSVECTOR3 vnormalize(ZSVECTOR3 vec);
    ZSVECTOR3 vadd(ZSVECTOR3 v1, ZSVECTOR3 v2);
    ZSVECTOR3 vmul(ZSVECTOR3 v1, float m);
}

namespace Window {
    void setWindowSize(int W, int H);
    void setWindowMode(unsigned int mode);
}

void bindSDKBaseMath(lua_State* state);
void bindSDKProperties(lua_State* state);
void bindSDKClasses(lua_State* state);
void bindKeyCodesSDK(lua_State* state);

}

#endif // ZSENSDK_H
