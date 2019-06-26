#ifndef engine_manager
#define engine_manager

#include "../engine_types.h"

class EngineComponentManager{
private:

public:
    int WIDTH; //current width of window
    int HEIGHT; //current height of window
    float deltaTime; //Time, spent to draw last frame
    ZSGAME_DESC* game_desc_ptr; //pointer to description of project

    void setDpMetrics(int W, int H);

    virtual void init();
    virtual void updateWindowSize(int W, int H);

    EngineComponentManager();
    virtual ~EngineComponentManager();
};

#endif
