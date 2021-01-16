#include "../../../headers/world/ObjectsComponents/LightSourceComponent.hpp"
#include "../../../headers/world/ObjectsComponents/ShadowCasterComponent.hpp"

extern ZSpireEngine* engine_ptr;

Engine::ShadowCasterProperty::ShadowCasterProperty() :
    TextureSize(2048),

    mShadowBias(0.005f),
    nearPlane(0.1f),
    farPlane(75.0f),
    projection_viewport(20),

    mShadowBuffer(0),
    mShadowDepthTexture(0),
    mCascadesNum(3),
    mPcfNum(1),
    mShadowStrength(1.f),

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
    Engine::LightsourceComponent* light = this->go_link.updLinkPtr()->getPropertyPtr<Engine::LightsourceComponent>();

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
    _dest->mPcfNum = this->mPcfNum;
    _dest->mShadowStrength = this->mShadowStrength;
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
    glActiveTexture(GL_TEXTURE27);
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
    readBinaryValue(&mPcfNum, data + offset, offset);
    readBinaryValue(&mShadowStrength, data + offset, offset);
    readBinaryValue(&mShadowBias, data + offset, offset);
    readBinaryValue(&nearPlane, data + offset, offset);
    readBinaryValue(&farPlane, data + offset, offset);
    readBinaryValue(&projection_viewport, data + offset, offset);
}

void Engine::ShadowCasterProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    //write collider type
    stream->writeBinaryValue(&TextureSize);
    stream->writeBinaryValue(&mCascadesNum);
    stream->writeBinaryValue(&mPcfNum);
    stream->writeBinaryValue(&mShadowStrength);
    stream->writeBinaryValue(&mShadowBias);
    stream->writeBinaryValue(&nearPlane);
    stream->writeBinaryValue(&farPlane);
    stream->writeBinaryValue(&projection_viewport);
}

void Engine::ShadowCasterProperty::onPreRender(Engine::Renderer* pipeline) {
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

void Engine::ShadowCasterProperty::SendShadowParamsToShaders(Engine::Camera* cam, Renderer* pipeline) {
    Engine::LightsourceComponent* light = this->go_link.updLinkPtr()->getPropertyPtr<Engine::LightsourceComponent>();

    //Send BIAS value
    pipeline->shadowBuffer->writeDataBuffered(0, sizeof(float), &mShadowBias);
    //Send Width of shadow texture
    pipeline->shadowBuffer->writeDataBuffered(sizeof(float), sizeof(int), &this->TextureSize);
    //Send number of cascades of shadow texture
    pipeline->shadowBuffer->writeDataBuffered(12, sizeof(int), &this->mCascadesNum);
    //Send number of pcf passes
    pipeline->shadowBuffer->writeDataBuffered(16, sizeof(int), &this->mPcfNum);
    //Send strength value
    pipeline->shadowBuffer->writeDataBuffered(20, sizeof(int), &this->mShadowStrength);
    //Use shadowmap shader to draw objects
    pipeline->getShadowmapShader()->Use();

    int dists[] = { 0, 40, 100, 140, 200, 250};
    //Send all dists
    for (int i = 0; i < 6; i++) {
        unsigned int DistOffset = 416 + sizeof(int) * i;
        pipeline->shadowBuffer->writeDataBuffered(DistOffset, sizeof(int), &dists[i]);
    }
    //iterate over all cascades
    for (int i = 0; i < mCascadesNum; i++) {
        Vec3 cam_pos = cam->getCameraPosition() + cam->getCameraFrontVec() * static_cast<float>(dists[i]);
        Mat4 matview = matrixLookAt(cam_pos, cam_pos + light->direction * -1, Vec3(0, 1, 0));

        float w = static_cast<float>(40 * (1 + i));
        this->LightProjectionMat = getOrthogonal(-w, w, -w, w, nearPlane, farPlane);

        if (engine_ptr != nullptr) {
            if (engine_ptr->engine_info->graphicsApi == VULKAN) {
                LightProjectionMat = getOrthogonalVulkan(-w, w, -w, w, nearPlane, farPlane);
            }
        }

        Mat4 mat = matview * LightProjectionMat;

        unsigned int offset = 32 + sizeof(Mat4) * i;
        pipeline->shadowBuffer->writeDataBuffered(offset, sizeof(Mat4), &mat);
    }

    //Bind shadow uniform buffer
    pipeline->shadowBuffer->bind();
    pipeline->shadowBuffer->updateBufferedData();
}

void Engine::ShadowCasterProperty::Draw(Engine::Camera* cam, Renderer* pipeline) {
    if (!isRenderAvailable()) {
        onObjectDeleted();
        return;
    }

    //Change Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, mShadowBuffer); //Bind framebuffer
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glDisable(GL_CULL_FACE);
    
    SendShadowParamsToShaders(cam, pipeline);

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