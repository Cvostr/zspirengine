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

    shadowBuffer = 0;
    shadowDepthTexture = 0;
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

void Engine::ShadowCasterProperty::onPreRender(Engine::RenderPipeline* pipeline) {
    pipeline->getRenderSettings()->shadowcaster_obj_ptr = static_cast<void*>(this->go_link.updLinkPtr());
}

void Engine::ShadowCasterProperty::init() {
    glGenFramebuffers(1, &this->shadowBuffer);//Generate framebuffer for texture
    glGenTextures(1, &this->shadowDepthTexture); //Generate texture

    glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
    //Configuring texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->TextureWidth * 3, this->TextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    //Binding framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    //Connecting depth texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadowDepthTexture, 0);
    //We won't render color
    glDrawBuffer(false);
    glReadBuffer(false);
    //Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->initialized = true;
}

void Engine::ShadowCasterProperty::onObjectDeleted() {
    glViewport(0, 0, TextureWidth, TextureHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer); //Bind framebuffer
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Engine::ShadowCasterProperty::Draw(Engine::Camera* cam, RenderPipeline* pipeline) {
    if (!isRenderAvailable()) {
        onObjectDeleted();
        return;
    }

    Engine::LightsourceProperty* light = this->go_link.updLinkPtr()->getPropertyPtr<Engine::LightsourceProperty>();
    //Change Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer); //Bind framebuffer
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glDisable(GL_CULL_FACE);
    //Bind shadow uniform buffer
    pipeline->shadowBuffer->bind();
    //Send BIAS value
    pipeline->shadowBuffer->writeData(sizeof(ZSMATRIX4x4) * 4, 4, &shadow_bias);
    //Send Width of shadow texture
    pipeline->shadowBuffer->writeData(sizeof(ZSMATRIX4x4) * 4 + 8, 4, &this->TextureWidth);
    //Send Height of shadow texture
    pipeline->shadowBuffer->writeData(sizeof(ZSMATRIX4x4) * 4 + 12, 4, &this->TextureHeight);
    //Use shadowmap shader to draw objects
    pipeline->getShadowmapShader()->Use();

    float dists[] = { 40, 70, 110 };
    //iterate over all cascades
    for (unsigned int i = 0; i < 3; i++) {
        ZSVECTOR3 cam_pos = cam->getCameraPosition() - cam->getCameraFrontVec() * 15 * (1.f + (float)i);
        ZSMATRIX4x4 matview = matrixLookAt(cam_pos, cam_pos + light->direction * -1, ZSVECTOR3(0, 1, 0));

        float w = projection_viewport * (1 + i) * 1.5f;
        this->LightProjectionMat = getOrthogonal(-w, w, -w, w, nearPlane, farPlane);

        ZSMATRIX4x4 mat = matview * LightProjectionMat;

        pipeline->shadowBuffer->bind();
        pipeline->shadowBuffer->writeData(0, sizeof(ZSMATRIX4x4), &mat);
        uint64_t offset = (1 + uint64_t(i));
        pipeline->shadowBuffer->writeData(sizeof(ZSMATRIX4x4) * offset, sizeof(ZSMATRIX4x4), &mat);

        glViewport(TextureWidth * i, 0, TextureWidth, TextureHeight);
        //Render to depth all scene
        pipeline->renderDepth(this->go_link.world_ptr);
    }

    glFrontFace(GL_CCW);
}