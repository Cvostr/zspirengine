#include <iostream>
#include "../../headers/Scripting/zsensdk.h"


void ZSENSDK::Debug::Log(std::string text){
    std::cout << "SCRIPT: " << text << std::endl;
}


ZSVECTOR3 ZSENSDK::Math::vnormalize(ZSVECTOR3 vec){
    ZSVECTOR3 result = vec;
    vNormalize(&result);
    return result;
}

ZSVECTOR3 ZSENSDK::Math::vadd(ZSVECTOR3 v1, ZSVECTOR3 v2){
    return v1 + v2;
}

void ZSENSDK::bindSDK(lua_State* state){

}
void ZSENSDK::bindKeyCodesSDK(lua_State* state){

}
