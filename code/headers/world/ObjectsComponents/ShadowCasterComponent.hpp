#pragma once

#include "../World.hpp"
#include "../../math/Mat4.hpp"
#include "../../ogl/GLFramebuffer.hpp"

namespace Engine {
    class ShadowCasterProperty : public Engine::IGameObjectComponent {
    private:
        bool initialized;

        GLframebuffer* Framebuffer;
        Camera* mCamera;

        void reinitialize();
    public:
        int TextureSize;
        float mShadowBias;
        float nearPlane;
        float farPlane;
        int projection_viewport;
        int mCascadesNum;
        int mPcfNum;
        float mShadowStrength;

        void addPropertyInterfaceToInspector();
        void onPreRender(Engine::Renderer* pipeline);
        void copyTo(Engine::IGameObjectComponent* dest);
        void onValueChanged();
        void init();
        void Draw(Engine::Camera* cam, Renderer* pipeline);
        void SendShadowParamsToShaders(Engine::Camera* cam, Renderer* pipeline);
        void setTexture();
        void setTextureSize(int TextureSize);
        void SetCascadesAmount(int CascadesNum);
        bool isRenderAvailable();

        int GetSuitableCascadesAmount(BoundingBox3& ObjectBB);

        //function to read shadowcaster property from memory
        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);
        void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

        ShadowCasterProperty();
    };
}