#pragma once

#include "../World.hpp"
#include "../../math/Mat4.hpp"

namespace Engine {
    class ShadowCasterProperty : public Engine::IGameObjectComponent {
    private:
        bool initialized;

        unsigned int mShadowBuffer;
        unsigned int mShadowDepthTexture;

        Mat4 LightProjectionMat;
        Mat4 LightViewMat[3];

        void reinitialize();
    public:
        int TextureSize;
        float mShadowBias;
        float nearPlane;
        float farPlane;
        int projection_viewport;
        int mCascadesNum;

        void addPropertyInterfaceToInspector();
        void onPreRender(Engine::Renderer* pipeline);
        void copyTo(Engine::IGameObjectComponent* dest);
        void onValueChanged();
        void init();
        void onObjectDeleted();
        void Draw(Engine::Camera* cam, Renderer* pipeline);
        void setTexture();
        void setTextureSize(int TextureSize);
        void SetCascadesAmount(int CascadesNum);
        bool isRenderAvailable();

        //function to read shadowcaster property from memory
        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);
        void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);

        ShadowCasterProperty();
    };
}