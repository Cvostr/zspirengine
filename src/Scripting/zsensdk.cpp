#include <iostream>
#include "../../headers/Scripting/zsensdk.h"
#include "../../headers/world/zs-camera.h"

void ZSENSDK::Debug::Log(std::string text){
    std::cout << "SCRIPT: " << text << std::endl;
}

ZSVECTOR3 ZSENSDK::Math::vnormalize(ZSVECTOR3 vec){
    ZSVECTOR3 result = vec;
    vNormalize(&result);
    return result;
}

ZSVECTOR3 ZSENSDK::Math::vadd(ZSVECTOR3 v1, ZSVECTOR3 v2){
    return v1 + v2;
}

void ZSENSDK::bindSDK(lua_State* state){
    using namespace Engine;
    luabridge::getGlobalNamespace(state).beginClass <ZSVECTOR3>("Vec3")
        .addData("x", &ZSVECTOR3::X)
        .addData("y", &ZSVECTOR3::Y)
        .addData("z", &ZSVECTOR3::Z)
        .addConstructor <void(*) (float, float, float)>()
        .endClass();

    luabridge::getGlobalNamespace(state).beginClass <ZSRGBCOLOR>("RGBColor")
        .addData("r", &ZSRGBCOLOR::r)
        .addData("g", &ZSRGBCOLOR::g)
        .addData("b", &ZSRGBCOLOR::b)
        .addConstructor <void(*) (float, float, float, float)>()
        .endClass();

    luabridge::getGlobalNamespace(state)
        .addFunction("length", &length)
        .addFunction("distance", &getDistance)
        .addFunction("normalize", &ZSENSDK::Math::vnormalize)
        .addFunction("v_add", &ZSENSDK::Math::vadd)
        .addFunction("v_cross", &vCross);

    luabridge::getGlobalNamespace(state)
        .beginClass <Camera>("Camera")
        .addFunction("setPosition", &Camera::setPosition)
        .addFunction("setFront", &Camera::setFront)
        .addData("pos", &Camera::camera_pos, false)
        .addData("front", &Camera::camera_front, false)
        .addFunction("setProjection", &Camera::setProjectionType)
        .addFunction("setZplanes", &Camera::setZplanes)
        .addFunction("setViewport", &Camera::setViewport)
        .addData("viewport", &Camera::viewport, false)
        .addData("Fov", &Camera::FOV, false)
        .addData("nearZ", &Camera::nearZ, false)
        .addData("farZ", &Camera::farZ, false)
        .endClass();

}
void ZSENSDK::bindKeyCodesSDK(lua_State* state){

}
