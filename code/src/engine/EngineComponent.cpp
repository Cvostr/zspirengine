#include "../../headers/engine/EngineComponent.h"

IEngineComponent::IEngineComponent(){

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

void IEngineComponent::init(){

}

void IEngineComponent::updateWindowSize(int W, int H){

}
