#include <iostream>
#include "../../headers/Scripting/zsensdk.h"
#include "../../headers/world/zs-camera.h"
#include "../../headers/world/go_properties.h"
#include "../../headers/world/tile_properties.h"

extern ZSpireEngine* engine_ptr;
extern ZSGAME_DATA* game_data;

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

ZSVECTOR3 EZSENSDK::Math::vmul(ZSVECTOR3 v1, float m){
    return v1 * m;
}

void EZSENSDK::Window::setWindowSize(int W, int H){
    engine_ptr->updateResolution(W, H);
}
void EZSENSDK::Window::setWindowMode(unsigned int mode){
    engine_ptr->setWindowMode(mode);
}

void EZSENSDK::bindSDKBaseMath(lua_State* state){
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

    luabridge::getGlobalNamespace(state).beginClass <ZSQUATERNION>("Quat")
        .addData("x", &ZSQUATERNION::X)
        .addData("y", &ZSQUATERNION::Y)
        .addData("z", &ZSQUATERNION::Z)
        .addData("w", &ZSQUATERNION::W)
        .addConstructor <void(*) (float, float, float, float)>()
        .endClass();

    luabridge::getGlobalNamespace(state).beginClass <ZSRGBCOLOR>("RGBColor")
        .addData("r", &ZSRGBCOLOR::r)
        .addData("g", &ZSRGBCOLOR::g)
        .addData("b", &ZSRGBCOLOR::b)
        .addConstructor <void(*) (float, float, float, float)>()
        .endClass();

    luabridge::getGlobalNamespace(state).beginClass <Engine::ZSVIEWPORT>("CmViewport")
       .addData("startX", &Engine::ZSVIEWPORT::startX)
       .addData("startY", &Engine::ZSVIEWPORT::startY)
       .addData("endX", &Engine::ZSVIEWPORT::endX)
       .addData("endY", &Engine::ZSVIEWPORT::endY)
       .addConstructor <void(*) (unsigned int, unsigned int, unsigned int, unsigned int)>()
       .endClass();

    luabridge::getGlobalNamespace(state)
        .addFunction("length", &getLength)
        .addFunction("distance", &getDistance)
        .addFunction("toeuler", &quatToEuler)
        .addFunction("normalize", &EZSENSDK::Math::vnormalize)
        .addFunction("v_add", &EZSENSDK::Math::vadd)
        .addFunction("sum", &EZSENSDK::Math::vadd)
        .addFunction("mul", &EZSENSDK::Math::vmul)
        .addFunction("v_cross", &vCross);

    luabridge::getGlobalNamespace(state).beginClass <Engine::Camera>("Camera")
            .addFunction("setPosition", &Engine::Camera::setPosition)
            .addFunction("setFront", &Engine::Camera::setFront)
            .addData("pos", &Engine::Camera::camera_pos, false)
            .addData("front", &Engine::Camera::camera_front, false)
            .addData("up", &Engine::Camera::camera_up, false)
            .addFunction("setProjection", &Engine::Camera::setProjectionType)
            .addFunction("setZplanes", &Engine::Camera::setZplanes)
            .addFunction("setViewport", &Engine::Camera::setViewport)
            .addData("viewport", &Engine::Camera::viewport, false)
            .addData("Fov", &Engine::Camera::FOV, false)
            .addData("nearZ", &Engine::Camera::nearZ, false)
            .addData("farZ", &Engine::Camera::farZ, false)
        .endClass();
    luabridge::getGlobalNamespace(state)
        .beginNamespace("input")
        .addFunction("isKeyPressed", &Input::isKeyPressed)
        .addFunction("isKeyHold", &Input::isKeyHold)
        .addFunction("getMouseState", &Input::getMouseState)
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

}

void EZSENSDK::bindSDKProperties(lua_State* state){
    luabridge::getGlobalNamespace(state)
        .beginClass <Engine::GameObjectProperty>("ObjectProperty")
            .addFunction("setActive", &Engine::GameObjectProperty::setActive)
            .addData("active", &Engine::GameObjectProperty::active, false)
            .addData("type", &Engine::GameObjectProperty::type, false)
        .endClass()

        .deriveClass <Engine::TransformProperty, Engine::GameObjectProperty>("Transform")
            .addData("translation", &Engine::TransformProperty::translation, false)
            .addData("scale", &Engine::TransformProperty::scale, false)
            .addData("rotation", &Engine::TransformProperty::rotation, false)
            .addFunction("setPosition", &Engine::TransformProperty::setTranslation)
            .addFunction("setScale", &Engine::TransformProperty::setScale)
            .addFunction("setRotation", &Engine::TransformProperty::setRotation)
        .endClass()

        .deriveClass <Engine::AudioSourceProperty, Engine::GameObjectProperty>("AudioSource")
            .addFunction("setAudioFile", &Engine::AudioSourceProperty::setAudioFile)
            .addFunction("setAudioResource", &Engine::AudioSourceProperty::setAudioResource)
            .addFunction("Play", &Engine::AudioSourceProperty::audio_start)
            .addFunction("Stop", &Engine::AudioSourceProperty::audio_stop)
            .addFunction("Pause", &Engine::AudioSourceProperty::audio_pause)
            .addFunction("getGain", &Engine::AudioSourceProperty::getGain)
            .addFunction("getPitch", &Engine::AudioSourceProperty::getPitch)
            .addFunction("setGain", &Engine::AudioSourceProperty::setGain)
            .addFunction("setPitch", &Engine::AudioSourceProperty::setPitch)
        .endClass()

        .deriveClass <Engine::LightsourceProperty, Engine::GameObjectProperty>("LightSource")
            .addData("intensity", &Engine::LightsourceProperty::intensity)
            .addData("range", &Engine::LightsourceProperty::range)
            .addData("color", &Engine::LightsourceProperty::color)
            .addData("spot_angle", &Engine::LightsourceProperty::spot_angle)
        .endClass()

        .deriveClass <Engine::PhysicalProperty, Engine::GameObjectProperty>("Physical")
            .addData("mass", &Engine::PhysicalProperty::mass, false)
        .endClass()

        .deriveClass <Engine::RigidbodyProperty, Engine::PhysicalProperty>("Rigidbody")
            .addData("gravity", &Engine::RigidbodyProperty::gravity, false)
            .addData("linearVelocity", &Engine::RigidbodyProperty::linearVel, false)
            .addFunction("setLinearVelocity", &Engine::RigidbodyProperty::setLinearVelocity)
        .endClass()

        .deriveClass <Engine::CharacterControllerProperty, Engine::PhysicalProperty>("CharacterController")
            .addData("gravity", &Engine::CharacterControllerProperty::gravity, false)
            .addData("linearVelocity", &Engine::CharacterControllerProperty::linearVel, false)
            .addFunction("Jump", &Engine::CharacterControllerProperty::jump)
            .addFunction("Walk", &Engine::CharacterControllerProperty::walk)
        .endClass()

        .deriveClass <Engine::TileProperty, Engine::GameObjectProperty>("Tile2D")
            .addFunction("playAnim", &Engine::TileProperty::playAnim)
            .addFunction("setDiffuseTexture", &Engine::TileProperty::setDiffuseTexture)
            .addFunction("stopAnim", &Engine::TileProperty::stopAnim)
        .endClass()

        .deriveClass <Engine::MaterialProperty, Engine::GameObjectProperty>("Material")
            .addFunction("playAnim", &Engine::MaterialProperty::_setMaterial)
        .endClass()

        .deriveClass <Engine::MeshProperty, Engine::GameObjectProperty>("Mesh")
            .addFunction("setMesh", &Engine::MeshProperty::setMeshResource)
        .endClass()

        .deriveClass <Engine::AnimationProperty, Engine::GameObjectProperty>("Animation")
            .addFunction("play", &Engine::AnimationProperty::play)
            .addFunction("stop", &Engine::AnimationProperty::stop)
            .addFunction("setAnimation", &Engine::AnimationProperty::setAnimationResource)
        .endClass();

    luabridge::getGlobalNamespace(state)
        //Common script
       .beginClass <Engine::ObjectScript>("Script")
       .addFunction("onStart", &Engine::ObjectScript::_callStart)
       .addFunction("onFrame", &Engine::ObjectScript::_callDraw)
       .addFunction("func", &Engine::ObjectScript::func)
       .addFunction("funcA", &Engine::ObjectScript::_func)
       .endClass();

}

void EZSENSDK::bindSDKResources(lua_State* state) {
    luabridge::getGlobalNamespace(state)
        .beginNamespace("engine")
            .beginNamespace("res")
                .beginClass <Engine::ZsResource>("Resource")
                .addFunction("release", &Engine::ZsResource::Release)
                .addFunction("load", &Engine::ZsResource::load)
                .endClass()

                .deriveClass <Engine::AudioResource, Engine::ZsResource>("AudioResource")
                .endClass()

                .deriveClass <Engine::TextureResource, Engine::ZsResource>("TextureResource")
                .endClass()

                .deriveClass <Engine::MeshResource, Engine::ZsResource>("MeshResource")
                .endClass()

                .deriveClass <Engine::AnimationResource, Engine::ZsResource>("AnimationResource")
                .endClass()
            .endNamespace()
        .endNamespace();
    //Fill LUA namespace with resources
    for (unsigned int i = 0; i < game_data->resources->getResourcesSize(); i++) {
        //Obtain next resource
        auto res = game_data->resources->getResourceByIndex(i);
        //get resource label
        std::string res_name = res->resource_label;
        //Replace / . @  by _
        for (unsigned int str_i = 0; str_i < res_name.size(); str_i++) {
            if (res_name[str_i] == '/' || res_name[str_i] == '.' || res_name[str_i] == '@') res_name[str_i] = '_';
        }
        if (res->resource_type == RESOURCE_TYPE_AUDIO) {
            Engine::AudioResource* _res = static_cast<Engine::AudioResource*>(res);
            luabridge::getGlobalNamespace(state)
                .beginNamespace("zsres")
                .addVariable(res_name.c_str(), _res)
                .endNamespace();
        }
        if (res->resource_type == RESOURCE_TYPE_MESH) {
            Engine::MeshResource* _res = static_cast<Engine::MeshResource*>(res);
            luabridge::getGlobalNamespace(state)
                .beginNamespace("zsres")
                .addVariable(res_name.c_str(), _res)
                .endNamespace();
        }
        if (res->resource_type == RESOURCE_TYPE_TEXTURE) {
            Engine::TextureResource* _res = static_cast<Engine::TextureResource*>(res);
            luabridge::getGlobalNamespace(state)
                .beginNamespace("zsres")
                .addVariable(res_name.c_str(), _res)
                .endNamespace();
        }
    }
}

void EZSENSDK::bindSDKClasses(lua_State* state){
    using namespace Engine;

    luabridge::getGlobalNamespace(state)
        .beginNamespace("window")
        .addVariable("MODE_WINDOWED", &_mode_windowed, false)
        .addVariable("MODE_FULLSCREEN", &_mode_fullscreen, false)
        .addVariable("MODE_BORDERLESS", &_mode_borderless, false)
        .addFunction("setWindowSize", &EZSENSDK::Window::setWindowSize)
        .addFunction("setWindowMode", &EZSENSDK::Window::setWindowMode)
        .endNamespace()

        .beginNamespace("engine")
        .beginClass <Engine::GameObject>("GameObject")
        .addFunction("getLabel", &Engine::GameObject::getLabel)
        .addFunction("setLabel", &Engine::GameObject::setLabel)
        .addFunction("setActive", &Engine::GameObject::setActive)
        .addData("active", &Engine::GameObject::active, false)
        .addData("propsNum", &Engine::GameObject::props_num, false)
        .addFunction("getProperty", &Engine::GameObject::getPropertyPtrByTypeI)
        .addFunction("removeProperty", &Engine::GameObject::removeProperty)

        .addFunction("transform", &Engine::GameObject::getPropertyPtr<Engine::TransformProperty>)
        .addFunction("mesh", &Engine::GameObject::getPropertyPtr<Engine::MeshProperty>)
        .addFunction("audio", &Engine::GameObject::getPropertyPtr<Engine::AudioSourceProperty>)
        .addFunction("light", &Engine::GameObject::getPropertyPtr<Engine::LightsourceProperty>)
        .addFunction("tile", &Engine::GameObject::getPropertyPtr<Engine::TileProperty>)
        .addFunction("script", &Engine::GameObject::getPropertyPtr<Engine::ScriptGroupProperty>)
        .addFunction("rigidbody", &Engine::GameObject::getPropertyPtr<Engine::RigidbodyProperty>)
        .addFunction("character", &Engine::GameObject::getPropertyPtr<Engine::CharacterControllerProperty>)
        .addFunction("animation", &Engine::GameObject::getPropertyPtr<Engine::AnimationProperty>)
        .addFunction("material", &Engine::GameObject::getPropertyPtr<Engine::MaterialProperty>)
        .endClass()

        .beginClass <Engine::World>("World")
        .addFunction("findObject", &Engine::World::getGameObjectByLabel)
        .addFunction("instantiate", &World::Instantiate)
        //.addFunction("addFromPrefab", &World::addObjectsFromPrefab)
        .addFunction("removeObject", &World::removeObject)
        .addData("camera", &Engine::World::world_camera, true)
        .endClass()


        .deriveClass <Engine::ScriptGroupProperty, GameObjectProperty>("ScriptGroup")
        .addFunction("getScript", &ScriptGroupProperty::getScriptByName)
        .endClass()


        .endNamespace();



}
