#pragma once

#include "../engine_types.h"

class IEngineComponent{
private:

public:
    virtual void OnUpdate();
    virtual void OnCreate();
    virtual void OnUpdateWindowSize(int W, int H);

    IEngineComponent();
    virtual ~IEngineComponent();
};