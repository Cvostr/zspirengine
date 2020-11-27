#pragma once

#include "../engine_types.h"

typedef struct Project{
    std::string label; //Label of the project
    std::string root_path; //Project root folder

    int version; //Project version
    ZSPERSPECTIVE perspective; //3D or 2D

    std::string startup_scene;
}Project;


class IEngineComponent{
private:

public:
    int WIDTH; //current width of window
    int HEIGHT; //current height of window
    float deltaTime; //Time, spent to draw last frame
    ZSGAME_DESC* game_desc_ptr; //pointer to description of project
    Project* project_struct_ptr;

    void setDpMetrics(int W, int H);
    void setProjectStructPtr(Project* ptr);

    virtual void OnCreate();
    virtual void OnUpdateWindowSize(int W, int H);

    IEngineComponent();
    virtual ~IEngineComponent();
};