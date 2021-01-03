#pragma once

#include <typeinfo>

#define ZS_PI 3.14159265f

#include "../math/Vec2.hpp"

#include "../math/Vec3.hpp"

#include "../math/Vec4.hpp"

#include "../math/Quat.hpp"

#include "../math/Mat4.hpp"

#include "../math/Vertex.hpp"

#include "../math/RGBColor.hpp"


//Converts degrees into radians
float DegToRad(float degrees);
	
	
template<typename T>
int sign(T value){
    if(typeid (value) == typeid (int) || typeid (value) == typeid (float))
        return (static_cast<float>(value) > 0) ? 1 : -1;
    return 1;
}

