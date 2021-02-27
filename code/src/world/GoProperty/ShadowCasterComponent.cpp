#include <world/ObjectsComponents/LightSourceComponent.hpp>
#include <world/ObjectsComponents/ShadowCasterComponent.hpp>

extern ZSpireEngine* engine_ptr;

int sizes[] = {20, 50, 80, 130, 160, 190};

Engine::ShadowCasterProperty::ShadowCasterProperty() :
    TextureSize(2048),

    mShadowBias(0.005f),
    nearPlane(0.1f),
    farPlane(75.0f),
    projection_viewport(20),

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
    if(Framebuffer)
        Framebuffer->GetDepthTexture()->Use(27);
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
    Framebuffer->Destroy();
    Framebuffer->AddDepth(mCascadesNum, TextureFormat::FORMAT_DEPTH_32);
    Framebuffer->Create();
}

void Engine::ShadowCasterProperty::init() {
    Framebuffer = new GLframebuffer(this->TextureSize, this->TextureSize);
    
    //Add depth texture array
    Framebuffer->AddDepth(mCascadesNum, TextureFormat::FORMAT_DEPTH_32);
    Framebuffer->Create();

    this->initialized = true;
}

int Engine::ShadowCasterProperty::GetSuitableCascadesAmount(BoundingBox3& ObjectBB) {

    float longestDistance = ObjectBB.GetLongestDistance(mCamera->getCameraPosition());

    int InstNum = mCascadesNum;

    if (mCamera != nullptr) {

        for (int cascade_i = 0; 0 < mCascadesNum; cascade_i++) {
            if (longestDistance < sizes[cascade_i] * 2) {
                InstNum = cascade_i + 1;
                break;
            }
        }
    }


    return InstNum;
}

void Engine::ShadowCasterProperty::SendShadowParamsToShaders(Engine::Camera* cam, Renderer* pipeline) {
    Engine::LightsourceComponent* light = this->go_link.updLinkPtr()->getPropertyPtr<Engine::LightsourceComponent>();
    this->mCamera = cam;

    //Check camera pointer
    if (cam == nullptr)
        return;

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

 
    //Send all dists
    for (int i = 0; i < 6; i++) {
        unsigned int DistOffset = 416 + sizeof(int) * i;
        int Dist = sizes[i] * 2 - 2;
        pipeline->shadowBuffer->writeDataBuffered(DistOffset, sizeof(int), &Dist);
    }
    //iterate over all cascades
    for (int i = 0; i < mCascadesNum; i++) {
        Vec3 cam_pos = cam->getCameraPosition() + cam->getCameraFrontVec() * static_cast<float>(sizes[i]);
        Mat4 matview = matrixLookAt(cam_pos, cam_pos + light->direction * -1, Vec3(0, 1, 0));

        float w = sizes[i];
        Mat4 LightProjectionMat = getOrthogonal(-w, w, -w, w, -10, farPlane);

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

    SendShadowParamsToShaders(cam, pipeline);

    //Change Framebuffer
    Framebuffer->bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glDisable(GL_CULL_FACE);
    

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