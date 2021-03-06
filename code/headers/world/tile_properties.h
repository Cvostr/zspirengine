#ifndef TILE_PROPERTIES_H
#define TILE_PROPERTIES_H

#include "World.hpp"

namespace Engine {

typedef struct TileGeometry{
    int tileWidth;
    int tileHeight;

    TileGeometry(){
        tileWidth = 128;
        tileHeight = 128;
    }

}TileGeometry;

typedef struct TileAnimation{
    bool isAnimated;
    int framesX; //columns in atlas
    int framesY; //rows in atlas
    int frame_time; //time to change the frame

    TileAnimation(){
        isAnimated = false;

        framesX = 1; //defaultly we have 1 frame
        framesY = 1;

        frame_time = 1000;
    }

}TileAnimation;

typedef struct TileAnimationState{
    int current_time;
    int current_frame;
    bool playing = false;

    int cur_frameX;
    int cur_frameY;

    TileAnimationState() {
        current_frame = 0; //we start from 0 frame
        current_time = 0;

        cur_frameX = 0;
        cur_frameY = 0;

        playing = false; //we not playing that
    }
}TileAnimationState;

class TileGroupProperty : public Engine::IGameObjectComponent {
public:
    int tiles_amount_X; //Tiles to add
    int tiles_amount_Y;

    bool isCreated;
    Engine::TileGeometry geometry; //Tile to add geometry

    std::string diffuse_relpath; //Diffuse texture to generate
    std::string mesh_string; //Mesh to generate

    void copyTo(Engine::IGameObjectComponent* dest);
    void addPropertyInterfaceToInspector();

    void process();
    void clear();

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);

    TileGroupProperty();
};

class TileProperty : public Engine::IGameObjectComponent {
public:
    Engine::TileGeometry geometry;
    //Defines color texture
    Engine::TextureResource* texture_diffuse;
    std::string diffuse_relpath;
    //Defines texture, that will cover tile
    Engine::TextureResource* texture_transparent;
    std::string transparent_relpath;
    //Defines animation
    Engine::TileAnimation anim_property;
    Engine::TileAnimationState anim_state;

    void addPropertyInterfaceToInspector();
    void onValueChanged(); //Update texture pointer
    void updTexturePtr();
    void copyTo(Engine::IGameObjectComponent* dest);
    void onUpdate(float deltaTime);
    void onRender(Engine::Renderer* pipeline);

    void playAnim();
    void stopAnim();
    void setDiffuseTexture(std::string texture);

    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);

    TileProperty();
};
}

#endif // TILE_PROPERTIES_H
