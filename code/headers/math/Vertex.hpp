#pragma once

#include "Vec3.hpp"
#include "Vec2.hpp"

#define MAX_BONE_PER_VERTEX 12

typedef struct ZSVERTEX {

    Vec3 pos;

    Vec2 uv;

    Vec3 normal;

    Vec3 tangent;

    Vec3 bitangent;

    unsigned int ids[MAX_BONE_PER_VERTEX];
    float weights[MAX_BONE_PER_VERTEX];
    unsigned int bones_num;

    ZSVERTEX() : bones_num(0) {}
    ZSVERTEX(Vec3 npos) : pos(npos.X, npos.Y, npos.Z), uv(0), bones_num(0) {}
    ZSVERTEX(Vec3 npos, Vec2 nuv) :
        pos(npos.X, npos.Y, npos.Z),
        uv(nuv.X, nuv.Y), 
        normal(0),
        bones_num(0)
    {
    }
    ZSVERTEX(Vec3 npos, Vec2 nuv, Vec3 nnormal) : 
        pos(npos.X, npos.Y, npos.Z), uv(nuv.X, nuv.Y),
        normal(nnormal.X, nnormal.Y, nnormal.Z),
        bones_num(0)
    {
        tangent = Vec3(0);
        bitangent = Vec3(0);
    }
    ZSVERTEX(Vec3 npos, Vec2 nuv, Vec3 nnormal, Vec3 ntangent, Vec3 nbitangent)
        : pos(npos.X, npos.Y, npos.Z),
        uv(nuv.X, nuv.Y),
        normal(nnormal.X, nnormal.Y, nnormal.Z),
        tangent(ntangent.X, ntangent.Y, ntangent.Z),
        bitangent(nbitangent.X, nbitangent.Y, nbitangent.Z),
        bones_num(0)
    {
        
    }

}ZSVERTEX;