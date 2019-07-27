#include "../../headers/world/zs-camera.h"

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
        proj = getOrthogonal(0, static_cast<float>(viewport.endX - viewport.startX), 0, static_cast<float>(viewport.endY - viewport.startY), nearZ, farZ);
    }
}

void Engine::Camera::updateViewMat(){
    view = matrixLookAt(camera_pos, (camera_pos + camera_front), camera_up);
}

void Engine::Camera::setPosition(ZSVECTOR3 pos){
    this->camera_pos = pos;
    updateViewMat();
}

void Engine::Camera::setFront(ZSVECTOR3 front){
    this->camera_front = front;
    updateViewMat();
}
            
void Engine::Camera::setUp(ZSVECTOR3 up){
    this->camera_up = up;
    updateViewMat();
}

ZSMATRIX4x4 Engine::Camera::getViewMatrix(){
    return this->view;
}
ZSMATRIX4x4 Engine::Camera::getProjMatrix(){
    return this->proj;
}

ZSVIEWPORT Engine::Camera::getViewport(){
    return viewport;
}

ZSVECTOR3 Engine::Camera::getCameraPosition() {
	return this->camera_pos;
}

ZSVECTOR3 Engine::Camera::getCameraFrontVec(){
    return this->camera_front;
}

ZSVECTOR3 Engine::Camera::getCameraRightVec(){
    return vCross(camera_front, camera_up);
}

ZSVECTOR3 Engine::Camera::getCameraViewCenterPos(){
    if(this->proj_type == ZSCAMERA_PROJECTION_ORTHOGONAL){

        int viewport_y = static_cast<int>(viewport.endY - viewport.startY) / 2;
        int viewport_x = (viewport.endX - viewport.startX) / 2;
        viewport_x *= -1;
        ZSVECTOR3 result = camera_pos + ZSVECTOR3(viewport_x, (viewport_y), 0);
        return result;
    }
    return this->camera_pos;
}

Engine::Camera::Camera(){

    camera_pos = ZSVECTOR3(0,0,0);
    camera_front = ZSVECTOR3(1,0,0);
    camera_up = ZSVECTOR3(0,1,0);

    proj_type = ZSCAMERA_PROJECTION_PERSPECTIVE;
    nearZ = 0.1f;
    farZ = 100.0f;
    FOV = 45;
    viewport = ZSVIEWPORT(0,0, 640, 480);

    updateProjectionMat();
    updateViewMat();

}


