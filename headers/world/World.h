#ifndef zs_world
#define zs_world

#include <string>
#include <vector>
#include <fstream>

#include "../render/zs-pipeline.h"

namespace Engine {
class GameObject;
class World;

class GameObjectLink{
public:
    Engine::World* world_ptr;
    std::string obj_str_id;
    Engine::GameObject* ptr;

    Engine::GameObject* updLinkPtr(); //Updates pointer to object and returns it
    bool isEmpty(); //Check, if this link doesn't link to some object
    void crack(); //Make this link empty

    GameObjectLink();
};

class World{
private:
    std::vector<GameObject> objects;
public:

    void loadFromFile(std::string file);
    GameObject* getGameObjectByStrId(std::string id);

    World();
};

class GameObject{
public:
    std::string str_id;
    GO_RENDER_TYPE render_type;

    std::vector<GameObjectLink> children;

    GameObject();
};

}

#endif
