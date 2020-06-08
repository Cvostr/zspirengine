#ifndef engine_manager
#define engine_manager

#include "../engine_types.h"

typedef struct Project{
    std::string label; //Label of the project
    std::string root_path; //Project root folder

    int version; //Project version
    ZSPERSPECTIVE perspective; //3D or 2D

    std::string startup_scene;
}Project;


class EngineComponentManager{
private:

public:
    int WIDTH; //current width of window
    int HEIGHT; //current height of window
    float deltaTime; //Time, spent to draw last frame
    ZSGAME_DESC* game_desc_ptr; //pointer to description of project
    Project* project_struct_ptr;

    void setDpMetrics(int W, int H);
    void setProjectStructPtr(Project* ptr);

    virtual void init();
    virtual void updateWindowSize(int W, int H);

    EngineComponentManager();
    virtual ~EngineComponentManager();
};

#endif
