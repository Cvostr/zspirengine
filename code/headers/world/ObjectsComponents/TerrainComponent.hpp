#pragma once

#include "../World.hpp"
#include "../Terrain.hpp"

namespace Engine {

    class TerrainProperty : public Engine::IGameObjectComponent {

    private:
        TerrainData data;
        char edit_mode;

        btRigidBody* rigidBody;
    public:

        std::vector<HeightmapTexturePair> textures;

        std::string file_label;
        int Width; //Width of terrain mesh
        int Length; //Height of terrain mesh
        int MaxHeight;

        bool castShadows;
        int textures_size;
        int grassType_size;

        int range; //Range of edit
        float editHeight; //Modifying height
        int textureid;
        int vegetableid;

        void addPropertyInterfaceToInspector();
        void onRender(Engine::Renderer* pipeline);

        void DrawMesh(Renderer* pipeline, unsigned int instances = 1);
        void DrawGrass(Renderer* pipeline);
        void onValueChanged();
        void onAddToObject();
        void copyTo(Engine::IGameObjectComponent* dest);
        void onUpdate(float deltaTime);
        void onStart();

        void onMouseMotion(int posX, int posY, int screenY, bool isLeftButtonHold, bool isCtrlHold);
        void getPickedVertexId(int posX, int posY, int screenY, unsigned char* data);

        void modifyTerrainVertex(unsigned char* gl_data, bool isCtrlHold);

        TerrainData* getTerrainData();

        void loadPropertyFromMemory(const char* data, GameObject* obj);
        void savePropertyToStream(ZsStream* stream, GameObject* obj);

        TerrainProperty();
        ~TerrainProperty();
    };
}