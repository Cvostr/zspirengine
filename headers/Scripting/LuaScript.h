#ifndef zs_luascript
#define zs_luascript

#include <string>
extern "C" {
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>
}
#include <LuaBridge/LuaBridge.h>

#include "../world/World.h"

namespace Engine {

class ObjectScript {

private:
    lua_State* L;
public:
    bool created;
    std::string name;
    std::string content;

    void _InitScript();
    void _DestroyScript();

    void _callStart(GameObject* obj, World* world);
    void _callDraw(float deltaTime);
    void callDrawUI();
    void onTrigger();

    unsigned int getArgCount(lua_State *_L);

    void func(lua_State *L);
    void _func(std::string func_name, luabridge::LuaRef arg_table);

    ObjectScript();

};

}
#endif
