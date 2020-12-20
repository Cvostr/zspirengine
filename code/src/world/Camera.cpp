#include "../../headers/world/Camera.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::Camera::Camera() : camera_pos(0),
                           camera_front(1, 0, 0),
                           camera_up(0, 1, 0),
                           proj_type(ZSCAMERA_PROJECTION_PERSPECTIVE),
                           nearZ(0.1f),
                           farZ(100.0f),
                           FOV(45),
                           orthogonal_factor(1.f),
                           viewport(0, 0, 640, 480)
{
    updateProjectionMat();
    //updateViewMat();

    isMoving = false;
    isAlListenerCamera = false;
}

void Engine::Camera::setFOV(float FOV){
    this->FOV = FOV;
    updateProjectionMat();
}

void Engine::Camera::setZplanes(float nearZ, float farZ){
    this->nearZ = nearZ;
    this->farZ = farZ;
    updateProjectionMat();
}

void Engine::Camera::setProjectionType(ZSCAMERAPROJECTIONTYPE type){
    proj_type = type;
    updateProjectionMat();
}

void Engine::Camera::setViewport(ZSVIEWPORT viewport){
    this->viewport = viewport;
    updateProjectionMat();
}

void Engine::Camera::updateProjectionMat(){
    if(proj_type == ZSCAMERA_PROJECTION_PERSPECTIVE){
        float aspect = static_cast<float>((viewport.endX - viewport.startX)) / static_cast<float>(viewport.endY - viewport.startY);
        proj = getPerspective(FOV, aspect, nearZ, farZ);
    }else{
        proj = getOrthogonal(0, static_cast<float>(viewport.endX - viewport.startX) * orthogonal_factor,
                             0, static_cast<float>(viewport.endY - viewport.startY) * orthogonal_factor,
                             nearZ, farZ);
    }
    ui_proj = getOrthogonal(0, static_cast<float>(viewport.endX - viewport.startX),
                            0, static_cast<float>(viewport.endY - viewport.startY));
}

void Engine::Camera::updateViewMat(){
    if(isAlListenerCamera){
        game_data->oal_manager->setListenerPos(camera_pos);
        game_data->oal_manager->setListenerOri(camera_front, camera_up);
    }
    view = matrixLookAt(camera_pos, (camera_pos + camera_front), camera_up);
}

void Engine::Camera::setPosition(const Vec3& pos){

    this->camera_pos = pos;
    updateViewMat();
}

void Engine::Camera::setFront(const Vec3& front){
    this->camera_front = front;
    updateViewMat();
}

void Engine::Camera::setUp(const Vec3& up){
    this->camera_up = up;
    updateViewMat();
}

Mat4 Engine::Camera::getViewMatrix(){
    return this->view;
}
Mat4 Engine::Camera::getProjMatrix(){
    return this->proj;
}
Mat4 Engine::Camera::getUiProjMatrix(){
    return this->ui_proj;
}

Engine::ZSVIEWPORT Engine::Camera::getViewport(){
    return viewport;
}

Vec3 Engine::Camera::getCameraPosition() {
    return this->camera_pos;
}

Vec3 Engine::Camera::getCameraFrontVec(){
    return this->camera_front;
}

Vec3 Engine::Camera::getCameraRightVec(){
    return vCross(camera_front, camera_up);
}

Vec3 Engine::Camera::getCameraUpVec(){
    return camera_up;
}

Vec3 Engine::Camera::getCameraViewCenterPos(){
    if(this->proj_type == ZSCAMERA_PROJECTION_ORTHOGONAL){

        int viewport_y = static_cast<int>(viewport.endY - viewport.startY) / 2;
        int viewport_x = (viewport.endX - viewport.startX) / 2;
        viewport_x *= -1;
        Vec3 result = camera_pos + Vec3(static_cast<float>(viewport_x), static_cast<float>(viewport_y), 0);
        return result;
    }
    return this->camera_pos;
}

void Engine::Camera::updateTick(float deltaTime){
    if(!isMoving) return;

    Vec3 real_dest = (_dest_pos - (5) * camera_front);

    if(getDistance(camera_pos, real_dest) < 2){
        isMoving = false;
    }else{
        Vec3 delta = real_dest - camera_pos;
        float dist = getDistance(camera_pos, real_dest);

        Vec3 toMove = delta / dist;
        toMove = toMove * (deltaTime) * 80;
        camera_pos = camera_pos + toMove;
        updateViewMat();
    }
}

void Engine::Camera::startMoving(){
    if(proj_type == ZSCAMERA_PROJECTION_ORTHOGONAL){
        int viewport_size_x = static_cast<int>(viewport.endX) - static_cast<int>(viewport.startX);
        int viewport_size_y = static_cast<int>(viewport.endY) - static_cast<int>(viewport.startY);
        viewport_size_y *= -1;
        Vec3 to_add = Vec3(static_cast<float>(viewport_size_x) / 2.f, static_cast<float>(viewport_size_y) / 2.f, 0);
        _dest_pos = _dest_pos + to_add;
    }

    this->isMoving = true;
}

void Engine::Camera::stopMoving(){
    this->isMoving = false;
}
