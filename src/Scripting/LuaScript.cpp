#include "../../headers/Scripting/LuaScript.h"
#include <iostream>

#define SCRIPT_LOG std::cout << "SCRIPT "

void Engine::ObjectScript::_InitScript() {
    L = luaL_newstate();
    luaL_openlibs(L);

    int start_result = luaL_dostring(L, this->content.c_str());

    if(start_result == 1){ //if error in script
        SCRIPT_LOG << name << " error loading occured!" << std::endl;
        std::cout << "ERROR: " << lua_tostring(L, -1) << std::endl;
    }

    lua_pcall(L, 0, 0, 0);

    //Bind DSDK to script
    ZSENSDK::bindSDK(L);
    ZSENSDK::bindKeyCodesSDK(L);
}

void Engine::ObjectScript::_DestroyScript(){
    lua_close(L);
}

void Engine::ObjectScript::_callStart() {
    luabridge::LuaRef start = luabridge::getGlobal(L, "onStart");
    int result = 0;
    if (start.isFunction() == true) { //If function found
        try {
            result = start(this->link.updLinkPtr(), this->link.world_ptr); //Call script onStart()
        }
        catch (luabridge::LuaException e) {
           SCRIPT_LOG << "SCRIPT" << "Error occured in script (onStart) " << name << e.what() << std::endl;
        }
    }
    //Some error returned by script
    if(result == 1) SCRIPT_LOG << "Script (onStart) function exited with 1" << name << std::endl;

}


void Engine::ObjectScript::_callDraw(float deltaTime) {

   luabridge::LuaRef frame = luabridge::getGlobal(L, "onFrame");
    if (frame.isFunction() == true) { //If function found
        try {
            frame(deltaTime / 1000.0f);
        }
        catch (luabridge::LuaException e) {
            SCRIPT_LOG << "Error occured in script (onFrame) " << name << e.what() << std::endl;
        }
    }
}
void Engine::ObjectScript::callDrawUI() {

    luabridge::LuaRef ui = luabridge::getGlobal(L, "onDrawUI");
    if (ui.isFunction() == true) { //If function found
        try {
            ui();
        }
        catch (luabridge::LuaException e) {
            SCRIPT_LOG << "Error occured in script (onDrawUI) " << name << e.what() << std::endl;
        }
    }
}

unsigned int Engine::ObjectScript::getArgCount(lua_State *_L){
    return static_cast<unsigned int>(lua_gettop(_L));
}

void Engine::ObjectScript::func(lua_State *L){
    unsigned int argsNum = getArgCount(L);

    std::string func_name = lua_tostring(L, 2);

    int function = lua_getglobal(this->L, func_name.c_str());
    for(unsigned int i = 0; i <= argsNum; i ++){
        int arg_index = static_cast<int>(3 + i);
        if(lua_isinteger(L, arg_index)){
            int in = lua_tointeger(L, arg_index);
            lua_pushinteger(this->L, in);
            continue;
        }
        if(lua_isnumber(L, arg_index)){
            double in = lua_tonumber(L, arg_index);
            lua_pushnumber(this->L, in);
            continue;
        }
        if(lua_isstring(L, arg_index)){
            char* s = (char*)lua_tostring(L, arg_index);
            lua_pushstring(this->L, s);
        }
        if(lua_isboolean(L, arg_index)){
            bool in = lua_toboolean(L, arg_index);
            lua_pushboolean(this->L, in);
        }
        if(lua_islightuserdata(L, arg_index)){
            void* in = (void*)lua_topointer(L, arg_index);
            lua_pushlightuserdata(this->L, in);
        }
    }

      /* function to be called */

    if(lua_pcall(this->L, argsNum - 2, 0, 0) != 0){
        SCRIPT_LOG << "Error occured in script " << name << " function " << func_name << " : " << lua_tostring(this->L, -1) << std::endl;
    }

}
