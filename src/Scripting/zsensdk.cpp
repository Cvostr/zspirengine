#include <iostream>
#include "../../headers/Scripting/zsensdk.h"
#include "../../headers/world/zs-camera.h"
#include "../../headers/world/go_properties.h"
#include "../../headers/world/tile_properties.h"

void EZSENSDK::Debug::Log(std::string text){
    std::cout << "SCRIPT: " << text << std::endl;
}

ZSVECTOR3 EZSENSDK::Math::vnormalize(ZSVECTOR3 vec){
    ZSVECTOR3 result = vec;
    vNormalize(&result);
    return result;
}

ZSVECTOR3 EZSENSDK::Math::vadd(ZSVECTOR3 v1, ZSVECTOR3 v2){
    return v1 + v2;
}

void EZSENSDK::bindSDK(lua_State* state){
    using namespace Engine;

    luabridge::getGlobalNamespace(state)
        .beginNamespace("debug")
        .addFunction("Log", &EZSENSDK::Debug::Log)
        .endNamespace();

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
        .addFunction("normalize", &EZSENSDK::Math::vnormalize)
        .addFunction("v_add", &EZSENSDK::Math::vadd)
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

        .beginNamespace("engine")

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
        .addFunction("instantiate", &World::Instantiate)
        .addFunction("addFromPrefab", &World::addObjectsFromPrefab)
        .addFunction("removeObject", &World::removeObject)
        .addData("camera", &World::cam, true)
        .endClass()

        .beginClass <GameObjectProperty>("ObjectProperty")
        .addFunction("setActive", &GameObjectProperty::setActive)
        .addData("active", &GameObjectProperty::active, false)
        .addData("type", &GameObjectProperty::type, false)
        .endClass()

        //Usual script
        .beginClass <ObjectScript>("Script")
        .addFunction("onStart", &ObjectScript::_callStart)
        .addFunction("onFrame", &ObjectScript::_callDraw)
        .addFunction("func", &ObjectScript::func)
           //.addFunction("funcA", &ObjectScript::_func)
        .endClass()


        .deriveClass <LightsourceProperty, GameObjectProperty>("LightSource")
        .addData("intensity", &LightsourceProperty::intensity)
        .addData("range", &LightsourceProperty::range)
        .addData("color", &LightsourceProperty::color)
        .endClass()


        .endNamespace();



}
void EZSENSDK::bindKeyCodesSDK(lua_State* state){

}
