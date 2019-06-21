#ifndef zs_luascript
#define zs_luascript

#include <string>
extern "C" {
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>

//#include "zsensdk.h"
#include "../world/World.h"

namespace Engine {

class ObjectScript {

private:
    lua_State* L;
    //ZSENSDK::ZSENGmObject getGameObjectSDK();
public:
    GameObjectLink link;
    std::string fpath; //path to file

    void _InitScript();
    void _DestroyScript();

    void _callStart();
    void _callDraw(float deltaTime);
    void callDrawUI();

};

}
#endif
