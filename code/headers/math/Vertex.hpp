#pragma once

#include "Vec3.hpp"
#include "Vec2.hpp"

#define MAX_BONE_PER_VERTEX 12

typedef struct ZSVERTEX {

    ZSVECTOR3 pos;

    ZSVECTOR2 uv;

    ZSVECTOR3 normal;

    ZSVECTOR3 tangent;

    ZSVECTOR3 bitangent;

    unsigned int ids[MAX_BONE_PER_VERTEX];
    float weights[MAX_BONE_PER_VERTEX];
    unsigned int bones_num;

    ZSVERTEX() : bones_num(0) {}
    ZSVERTEX(ZSVECTOR3 npos) : pos(npos.X, npos.Y, npos.Z), uv(0), bones_num(0) {}
    ZSVERTEX(ZSVECTOR3 npos, ZSVECTOR2 nuv) : pos(npos.X, npos.Y, npos.Z), uv(nuv.X, nuv.Y) , normal(0) {
        bones_num = 0;
    }
    ZSVERTEX(ZSVECTOR3 npos, ZSVECTOR2 nuv, ZSVECTOR3 nnormal) : pos(npos.X, npos.Y, npos.Z), uv(nuv.X, nuv.Y), normal(nnormal.X, nnormal.Y, nnormal.Z) {
        bones_num = 0;
        tangent = ZSVECTOR3(0);
        bitangent = ZSVECTOR3(0);
    }
    ZSVERTEX(ZSVECTOR3 npos, ZSVECTOR2 nuv, ZSVECTOR3 nnormal, ZSVECTOR3 ntangent, ZSVECTOR3 nbitangent)
        : pos(npos.X, npos.Y, npos.Z),
        uv(nuv.X, nuv.Y),
        normal(nnormal.X, nnormal.Y, nnormal.Z),
        tangent(ntangent.X, ntangent.Y, ntangent.Z),
        bitangent(nbitangent.X, nbitangent.Y, nbitangent.Z) {
        bones_num = 0;
    }

}ZSVERTEX;