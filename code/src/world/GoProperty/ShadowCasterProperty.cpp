#include "../../../headers/world/go_properties.h"

Engine::ShadowCasterProperty::ShadowCasterProperty() :
    TextureSize(2048),

    mShadowBias(0.005f),
    nearPlane(1.0f),
    farPlane(75.0f),
    projection_viewport(20),

    mShadowBuffer(0),
    mShadowDepthTexture(0),
    mCascadesNum(3),

    initialized(false)
{
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_SHADOWCASTER;
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

void Engine::ShadowCasterProperty::copyTo(Engine::IGameObjectComponent* dest) {
    if (dest->type != this->type) return; //if it isn't script group

    //Do base things
    IGameObjectComponent::copyTo(dest);

    ShadowCasterProperty* _dest = static_cast<ShadowCasterProperty*>(dest);
    _dest->farPlane = this->farPlane;
    _dest->nearPlane = this->nearPlane;
    _dest->mShadowBias = this->mShadowBias;
    _dest->TextureSize = this->TextureSize;
    _dest->mCascadesNum = this->mCascadesNum;
    _dest->projection_viewport = this->projection_viewport;
}

void Engine::ShadowCasterProperty::setTextureSize(int TextureSize) {
    this->TextureSize = TextureSize;
    reinitialize();
}

void Engine::ShadowCasterProperty::SetCascadesAmount(int CascadesNum) {
    this->mCascadesNum = CascadesNum;
    reinitialize();
}

void Engine::ShadowCasterProperty::setTexture() {
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D_ARRAY, this->mShadowDepthTexture);
}

void Engine::ShadowCasterProperty::onValueChanged() {
    if (mCascadesNum > 6)
        mCascadesNum = 6;
    reinitialize();
}

void Engine::ShadowCasterProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;

    readBinaryValue(&TextureSize, data + offset, offset);
    readBinaryValue(&mCascadesNum, data + offset, offset);
    readBinaryValue(&mShadowBias, data + offset, offset);
    readBinaryValue(&nearPlane, data + offset, offset);
    readBinaryValue(&farPlane, data + offset, offset);
    readBinaryValue(&projection_viewport, data + offset, offset);
}

void Engine::ShadowCasterProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    //write collider type
    stream->writeBinaryValue(&TextureSize);
    stream->writeBinaryValue(&mCascadesNum);
    stream->writeBinaryValue(&mShadowBias);
    stream->writeBinaryValue(&nearPlane);
    stream->writeBinaryValue(&farPlane);
    stream->writeBinaryValue(&projection_viewport);
}

void Engine::ShadowCasterProperty::onPreRender(Engine::RenderPipeline* pipeline) {
    pipeline->getRenderSettings()->shadowcaster_obj_ptr = this->go_link.updLinkPtr();
}

void Engine::ShadowCasterProperty::reinitialize() {
    glDeleteTextures(1, &mShadowDepthTexture);
    glDeleteFramebuffers(1, &mShadowBuffer);
    //Reinitialize texture
    init();
}

void Engine::ShadowCasterProperty::init() {
    //Generate framebuffer for texture
    glGenFramebuffers(1, &this->mShadowBuffer);
    //Binding framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, mShadowBuffer);
    //Generate texture
    glGenTextures(1, &this->mShadowDepthTexture); 
    glBindTexture(GL_TEXTURE_2D_ARRAY, mShadowDepthTexture);
    //Configuring texture
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, this->TextureSize, this->TextureSize, mCascadesNum, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    
    //Connecting depth texture to framebuffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->mShadowDepthTexture, 0);
    //We won't render color
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    //Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->initialized = true;
}

void Engine::ShadowCasterProperty::onObjectDeleted() {
    glViewport(0, 0, TextureSize, TextureSize);
    glBindFramebuffer(GL_FRAMEBUFFER, mShadowBuffer); //Bind framebuffer
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Engine::ShadowCasterProperty::Draw(Engine::Camera* cam, RenderPipeline* pipeline) {
    if (!isRenderAvailable()) {
        onObjectDeleted();
        return;
    }

    Engine::LightsourceProperty* light = this->go_link.updLinkPtr()->getPropertyPtr<Engine::LightsourceProperty>();
    //Change Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, mShadowBuffer); //Bind framebuffer
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glDisable(GL_CULL_FACE);
    //Bind shadow uniform buffer
    pipeline->shadowBuffer->bind();
    //Send BIAS value
    pipeline->shadowBuffer->writeData(0, sizeof(float), &mShadowBias);
    //Send Width of shadow texture
    pipeline->shadowBuffer->writeData(sizeof(float), sizeof(int), &this->TextureSize);
    //Send number of cascades of shadow texture
    pipeline->shadowBuffer->writeData(12, sizeof(int), &this->mCascadesNum);
    //Use shadowmap shader to draw objects
    pipeline->getShadowmapShader()->Use();

    int dists[] = { 0, 40, 90, 130, 170, 210 };
    //iterate over all cascades
    for (int i = 0; i < mCascadesNum; i++) {
        ZSVECTOR3 cam_pos = cam->getCameraPosition() + cam->getCameraFrontVec() * static_cast<float>(dists[i]);
        Mat4 matview = matrixLookAt(cam_pos, cam_pos + light->direction * -1, ZSVECTOR3(0, 1, 0));

        float w = static_cast<float>(40 * (1 + i));
        this->LightProjectionMat = getOrthogonal(-w, w, -w, w, nearPlane, farPlane);

        Mat4 mat = matview * LightProjectionMat;

        pipeline->shadowBuffer->bind();
        unsigned int offset = 16 + sizeof(Mat4) * i;
        pipeline->shadowBuffer->writeData(offset, sizeof(Mat4), &mat);
        unsigned int DistOffset = 400 + sizeof(int) * i;
        pipeline->shadowBuffer->writeData(DistOffset, sizeof(int), &dists[i]);
    }
    glViewport(0, 0, TextureSize, TextureSize);
    //Render to depth all scene
    pipeline->renderShadowDepth(this->go_link.world_ptr, mCascadesNum);

    glFrontFace(GL_CCW);
}

void Engine::ShadowCasterProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    mgr->RegisterObjectType(SHADOWCAST_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

    mgr->RegisterObjectMethod(SHADOWCAST_PROP_TYPE_NAME, "void SetTextureSize(int)", asMETHOD(ShadowCasterProperty, setTextureSize), asCALL_THISCALL);
    mgr->RegisterObjectMethod(SHADOWCAST_PROP_TYPE_NAME, "void SetCascadesAmount(int)", asMETHOD(ShadowCasterProperty, SetCascadesAmount), asCALL_THISCALL);
}