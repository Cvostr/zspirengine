#ifndef zs_camera
#define zs_camera

#include "../render/zs-math.h"


enum ZSCAMERAPROJECTIONTYPE {ZSCAMERA_PROJECTION_NONE,
                            ZSCAMERA_PROJECTION_ORTHOGONAL,
                            ZSCAMERA_PROJECTION_PERSPECTIVE};

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

namespace Engine{
    class Camera{
        protected:
            ZSMATRIX4x4 proj; //Calculated Projection matrix
            ZSMATRIX4x4 view; //Calculated View matrix

            ZSCAMERAPROJECTIONTYPE proj_type;

        public:
            ZSVECTOR3 camera_pos; //Camera position vector
            ZSVECTOR3 camera_up; //Camera up direction vector
            ZSVECTOR3 camera_front; //Camera front direction vector
            ZSVIEWPORT viewport;
            float FOV; //Field of view
            float nearZ; //Nearest Z occlusion plane
            float farZ;

            ZSVECTOR3 getCameraPosition();
            ZSVECTOR3 getCameraUpVec();
            ZSVECTOR3 getCameraRightVec();
            ZSVECTOR3 getCameraFrontVec();

            ZSVECTOR3 getCameraViewCenterPos();
            ZSVIEWPORT getViewport();

            void updateProjectionMat();
            void updateViewMat();

            void setPosition(ZSVECTOR3 pos);
            void setFront(ZSVECTOR3 front);
            void setUp(ZSVECTOR3 up);

            void setFOV(float FOV);
            void setZplanes(float nearZ, float farZ);
            void setViewport(ZSVIEWPORT viewport);

            void setProjectionType(ZSCAMERAPROJECTIONTYPE type);

            ZSMATRIX4x4 getViewMatrix();
            ZSMATRIX4x4 getProjMatrix();
            void updateTick();

            Camera();
    };
}

#endif
