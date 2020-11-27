#include "../../headers/engine/EngineComponent.h"

IEngineComponent::IEngineComponent():
    WIDTH(0),
    HEIGHT(0)
{

}

IEngineComponent::~IEngineComponent(){

}

void IEngineComponent::setDpMetrics(int W, int H){
    this->WIDTH = W;
    this->HEIGHT = H;
}

void IEngineComponent::setProjectStructPtr(Project* ptr){
    this->project_struct_ptr = ptr;
}

void IEngineComponent::OnCreate(){

}

void IEngineComponent::OnUpdateWindowSize(int W, int H){

}
