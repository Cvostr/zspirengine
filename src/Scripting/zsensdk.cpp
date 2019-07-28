#include <iostream>
#include "../../headers/Scripting/zsensdk.h"
#include "../../headers/world/zs-camera.h"
#include "../../headers/world/go_properties.h"
#include "../../headers/world/tile_properties.h"

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

    luabridge::getGlobalNamespace(state).beginClass <ZSVIEWPORT>("CmViewport")
        .addData("startX", &ZSVIEWPORT::startX)
        .addData("startY", &ZSVIEWPORT::startY)
        .addData("endX", &ZSVIEWPORT::endX)
        .addData("endY", &ZSVIEWPORT::endY)
        .addConstructor <void(*) (unsigned int, unsigned int, unsigned int, unsigned int)>()
        .endClass();

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
        .endClass()

        .beginClass <GameObject>("GameObject")
        .addData("active", &GameObject::active, false)
        .addData("propsNum", &GameObject::props_num, false)
        .addFunction("getLabel", &GameObject::getLabel)
        .addFunction("setLabel", &GameObject::setLabel)
        .addFunction("setActive", &GameObject::setActive)
/*.addFunction("getProperty", &GameObject::getPropertyPtrByTypeI)
.addFunction("addProperty", &GameObject::addProperty)
.addFunction("removeProperty", &GameObject::removeProperty) */
        .addFunction("transform", &GameObject::getPropertyPtr<TransformProperty>)
        .addFunction("mesh", &GameObject::getPropertyPtr<MeshProperty>)
        .addFunction("audio", &GameObject::getPropertyPtr<AudioSourceProperty>)
        .addFunction("light", &GameObject::getPropertyPtr<LightsourceProperty>)
        .addFunction("tile", &GameObject::getPropertyPtr<TileProperty>)
        .addFunction("script", &GameObject::getPropertyPtr<ScriptGroupProperty>)
        .endClass()

        .beginClass <World>("World")
        .addFunction("findObject", &World::getGameObjectByLabel)
        //.addFunction("instantiate", &World::Instantiate)
        //.addFunction("addFromPrefab", &World::addObjectsFromPrefabStr)
        //.addFunction("removeObject", &World::removeObjPtr)
        .addData("camera", &World::cam, true)
        .endClass();


}
void ZSENSDK::bindKeyCodesSDK(lua_State* state){

}
