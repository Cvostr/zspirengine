#ifndef engine_manager
#define engine_manager

class EngineComponentManager{
private:

public:
    int WIDTH;
    int HEIGHT;

    void setDpMetrics(int W, int H);

    float deltaTime;

    virtual void init();
    virtual void destroy();
    virtual void updateWindowSize(int W, int H);

    EngineComponentManager();
    virtual ~EngineComponentManager();
};

#endif
