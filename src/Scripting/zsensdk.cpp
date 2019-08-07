#include <iostream>
#include "../../headers/Scripting/zsensdk.h"
#include "../../headers/world/zs-camera.h"
#include "../../headers/world/go_properties.h"
#include "../../headers/world/tile_properties.h"

extern ZSpireEngine* engine_ptr;

unsigned int _mode_fullscreen = SDL_WINDOW_FULLSCREEN;
unsigned int _mode_borderless = SDL_WINDOW_FULLSCREEN_DESKTOP;
unsigned int _mode_windowed = 0;

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

void EZSENSDK::Window::setWindowSize(int W, int H){
    engine_ptr->updateResolution(W, H);
}
void EZSENSDK::Window::setWindowMode(unsigned int mode){
    engine_ptr->setWindowMode(mode);
}

void EZSENSDK::bindSDK(lua_State* state){
    using namespace Engine;

    luabridge::getGlobalNamespace(state)
        .beginNamespace("window")
        .addVariable("MODE_WINDOWED", &_mode_windowed, false)
        .addVariable("MODE_FULLSCREEN", &_mode_fullscreen, false)
        .addVariable("MODE_BORDERLESS", &_mode_borderless, false)
        .addFunction("setWindowSize", &EZSENSDK::Window::setWindowSize)
        .addFunction("setWindowMode", &EZSENSDK::Window::setWindowMode)
        .endNamespace();

    luabridge::getGlobalNamespace(state)
        .beginNamespace("debug")
        .addFunction("Log", &EZSENSDK::Debug::Log)
        .endNamespace();

    luabridge::getGlobalNamespace(state)
        .beginNamespace("input")
        .addFunction("isKeyPressed", &EZSENSDK::Input::isKeyPressed)
        .addFunction("isKeyHold", &EZSENSDK::Input::isKeyHold)
        .addFunction("getMouseState", &EZSENSDK::Input::getMouseState)
        //Add mouse state class
        .beginClass <Input::MouseState>("MouseState")
        .addData("cursorX", &Input::MouseState::mouseX)
        .addData("cursorY", &Input::MouseState::mouseY)
        .addData("relX", &Input::MouseState::mouseRelX)
        .addData("relY", &Input::MouseState::mouseRelY)
        .addData("isLButtonDown", &Input::MouseState::isLButtonDown)
        .addData("isRButtonDown", &Input::MouseState::isRButtonDown)
        .addData("isMidButtonDown", &Input::MouseState::isMidBtnDown)
        .endClass()

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
        .endClass()

        //Lightsource class
        .deriveClass <Engine::LightsourceProperty, GameObjectProperty>("LightSource")
        .addData("intensity", &LightsourceProperty::intensity)
        .addData("range", &LightsourceProperty::range)
        .addData("color", &LightsourceProperty::color)
        .endClass()
        //Transform class
        .deriveClass <Engine::TransformProperty, GameObjectProperty>("Transform")
        .addData("translation", &Engine::TransformProperty::translation, false)
        .addData("scale", &Engine::TransformProperty::scale, false)
        .addData("rotation", &Engine::TransformProperty::rotation, false)
        .addFunction("setPosition", &Engine::TransformProperty::setTranslation)
        .addFunction("setScale", &Engine::TransformProperty::setScale)
        .addFunction("setRotation", &Engine::TransformProperty::setRotation)
        .endClass()
        //Audio source
        .deriveClass <Engine::AudioSourceProperty, Engine::GameObjectProperty>("AudioSource")
        //.addFunction("setAudioFile", &AudioSourceProperty::setAudioFile)
        .addFunction("Play", &Engine::AudioSourceProperty::audio_start)
        .addFunction("Stop", &Engine::AudioSourceProperty::audio_stop)
        .addFunction("Pause", &Engine::AudioSourceProperty::audio_pause)
        .addFunction("getGain", &Engine::AudioSourceProperty::getGain)
        .addFunction("getPitch", &Engine::AudioSourceProperty::getPitch)
        .addFunction("setGain", &Engine::AudioSourceProperty::setGain)
        .addFunction("setPitch", &Engine::AudioSourceProperty::setPitch)
        .endClass()

        //Tile class
        .deriveClass <Engine::TileProperty, GameObjectProperty>("Tile2D")
        .addFunction("playAnim", &Engine::TileProperty::playAnim)
        .addFunction("setDiffuseTexture", &Engine::TileProperty::setDiffuseTexture)
        .addFunction("stopAnim", &Engine::TileProperty::stopAnim)
        .endClass()

        .deriveClass <Engine::ScriptGroupProperty, GameObjectProperty>("ScriptGroup")
        .addFunction("getScript", &ScriptGroupProperty::getScriptByName)
        .endClass()


        .endNamespace();



}
