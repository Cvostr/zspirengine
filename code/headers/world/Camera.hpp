#pragma once

#include "../render/Math.hpp"
#include "../audio/SoundSource.hpp"

enum ZSCAMERAPROJECTIONTYPE {ZSCAMERA_PROJECTION_NONE,
                            ZSCAMERA_PROJECTION_ORTHOGONAL,
                            ZSCAMERA_PROJECTION_PERSPECTIVE};


namespace Engine{
    typedef struct ZSVIEWPORT{
        unsigned int startX;
        unsigned int startY;

        unsigned int endX;
        unsigned int endY;

        ZSVIEWPORT(unsigned int _startX, unsigned int _startY, unsigned int _endX, unsigned int _endY){
            startX = _startX;
            startY = _startY;
            endX = _endX;
            endY = _endY;
        }

        ZSVIEWPORT(){}

    }ZSVIEWPORT;

    class Camera{
        protected:
            Mat4 proj; //Calculated Projection matrix
            Mat4 ui_proj;
            Mat4 view; //Calculated View matrix

            ZSCAMERAPROJECTIONTYPE proj_type;

            bool isMoving;

        public:
            Vec3 camera_pos; //Camera position vector
            Vec3 camera_up; //Camera up direction vector
            Vec3 camera_front; //Camera front direction vector

            float FOV; //Field of view
            float nearZ; //Nearest Z occlusion plane
            float farZ;
            float orthogonal_factor;

            ZSVIEWPORT viewport;

            bool isAlListenerCamera;
            Vec3 _dest_pos;

            Vec3 getCameraPosition();
            Vec3 getCameraUpVec();
            Vec3 getCameraRightVec();
            Vec3 getCameraFrontVec();

            Vec3 getCameraViewCenterPos();
            ZSVIEWPORT getViewport();

            void updateProjectionMat();
            void updateViewMat();

            void setPosition(const Vec3& pos);
            void setFront(const Vec3& front);
            void setUp(const Vec3& up);

            void setFOV(float FOV);
            void setZplanes(float nearZ, float farZ);
            void setViewport(ZSVIEWPORT viewport);

            void setProjectionType(ZSCAMERAPROJECTIONTYPE type);
            void startMoving();
            void stopMoving();

            Mat4 getViewMatrix();
            Mat4 getProjMatrix();
            Mat4 getUiProjMatrix();
            void updateTick(float deltaTime);

            Camera();
    };
}