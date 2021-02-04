#include "../../headers/world/Camera.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;
extern ZSpireEngine* engine_ptr;

Engine::Camera::Camera() : mCameraPos(0),
                           mCameraFront(1, 0, 0),
                           mCameraUp(0, 1, 0),
                           mViewScale(1),
                           mProjectionType(ZSCAMERA_PROJECTION_PERSPECTIVE),
                           mNearZ(0.1f),
                           mFarZ(100.0f),
                           mFOV(45),
                           mViewport(0, 0, 640, 480),
                            mReflectionMatrix(1)
{
    updateProjectionMat();
    //updateViewMat();

       isAlListenerCamera = false;
}

void Engine::Camera::setFOV(float FOV){
    this->mFOV = FOV;
    updateProjectionMat();
}

void Engine::Camera::setZplanes(float nearZ, float farZ){
    this->mNearZ = nearZ;
    this->mFarZ = farZ;
    updateProjectionMat();
}

void Engine::Camera::setProjectionType(ZSCAMERAPROJECTIONTYPE type){
    mProjectionType = type;
    updateProjectionMat();
}

void Engine::Camera::setViewport(ZSVIEWPORT viewport){
    this->mViewport = viewport;
    updateProjectionMat();
}

void Engine::Camera::setViewport(unsigned int Width, unsigned int Height) {
    mViewport.startX = 0;
    mViewport.startY = 0;

    mViewport.endX = Width;
    mViewport.endY = Height;
}

void Engine::Camera::updateProjectionMat(){
    if(mProjectionType == ZSCAMERA_PROJECTION_PERSPECTIVE){
        float aspect = static_cast<float>((mViewport.endX - mViewport.startX)) / static_cast<float>(mViewport.endY - mViewport.startY);
        if (engine_ptr != nullptr) {
            if (engine_ptr->engine_info->graphicsApi == VULKAN) {
                mProjectionMatrix = getPerspectiveVulkan(mFOV, aspect, mNearZ, mFarZ);
            }
            else
                mProjectionMatrix = getPerspective(mFOV, aspect, mNearZ, mFarZ);
        }else
            mProjectionMatrix = getPerspective(mFOV, aspect, mNearZ, mFarZ);
    }else{
        mProjectionMatrix = getOrthogonal(0, static_cast<float>(mViewport.endX - mViewport.startX),
                             0, static_cast<float>(mViewport.endY - mViewport.startY),
            mNearZ, mFarZ);
    }
    mUiProjection = getOrthogonal(0, static_cast<float>(mViewport.endX - mViewport.startX),
                            0, static_cast<float>(mViewport.endY - mViewport.startY));

    
}

void Engine::Camera::updateViewMat(){
    if(isAlListenerCamera){
        game_data->oal_manager->setListenerPos(mCameraPos);
        game_data->oal_manager->setListenerOri(mCameraFront, mCameraUp);
    }
    mViewMatrix = mReflectionMatrix * getScaleMat(mViewScale) *
        matrixLookAt(mCameraPos, (mCameraPos + mCameraFront), mCameraUp);
}

void Engine::Camera::setPosition(const Vec3& pos){

    this->mCameraPos = pos;
    updateViewMat();
}

void Engine::Camera::setFront(const Vec3& front){
    this->mCameraFront = front;
    updateViewMat();
}

void Engine::Camera::setUp(const Vec3& up){
    this->mCameraUp = up;
    updateViewMat();
}

void Engine::Camera::setViewScale(const Vec3& scale) {
    this->mViewScale = scale;
    updateViewMat();
}

Vec3 Engine::Camera::getCameraRightVec(){
    return vCross(mCameraFront, mCameraUp);
}