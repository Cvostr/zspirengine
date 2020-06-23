#include "../../../headers/world/go_properties.h"

Engine::ShadowCasterProperty::ShadowCasterProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_SHADOWCASTER;

    initialized = false;

    TextureWidth = 2048;
    TextureHeight = 2048;

    shadow_bias = 0.005f;

    nearPlane = 1.0f;
    farPlane = 75.0f;

    projection_viewport = 20;
}

bool Engine::ShadowCasterProperty::isRenderAvailable() {
    if (!this->active)
        return false;
    if (!this->initialized)
        init();
    if (this->go_link.updLinkPtr() == nullptr)
        return false;
    //Get lightsource property of object
    Engine::LightsourceProperty* light = this->go_link.updLinkPtr()->getPropertyPtr<Engine::LightsourceProperty>();

    if (light == nullptr)
        return false;
    //if light gameobject disabled
    if (!light->isActive())
        return false;
    return true;
}

void Engine::ShadowCasterProperty::copyTo(Engine::GameObjectProperty* dest) {
    if (dest->type != this->type) return; //if it isn't script group

    //Do base things
    GameObjectProperty::copyTo(dest);

    ShadowCasterProperty* _dest = static_cast<ShadowCasterProperty*>(dest);
    _dest->farPlane = this->farPlane;
    _dest->nearPlane = this->nearPlane;
    _dest->shadow_bias = this->shadow_bias;
    _dest->TextureWidth = this->TextureWidth;
    _dest->TextureHeight = this->TextureHeight;
    _dest->projection_viewport = this->projection_viewport;
}

void Engine::ShadowCasterProperty::setTextureSize() {
    glDeleteTextures(1, &this->shadowDepthTexture);
    glDeleteFramebuffers(1, &shadowBuffer);

    init();
}

void Engine::ShadowCasterProperty::setTexture() {
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, this->shadowDepthTexture);
}

void Engine::ShadowCasterProperty::onValueChanged() {
    //Update shadowmap texture
    setTextureSize();
}

void Engine::ShadowCasterProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;

    memcpy(&TextureWidth, data + offset, sizeof(int));
    offset += sizeof(int);
    memcpy(&TextureHeight, data + offset, sizeof(int));
    offset += sizeof(int);
    memcpy(&shadow_bias, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&nearPlane, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&farPlane, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&projection_viewport, data + offset, sizeof(float));
    offset += sizeof(float);
}