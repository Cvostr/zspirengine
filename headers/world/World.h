#ifndef zs_world
#define zs_world

#include <string>
#include <vector>
#include <fstream>
namespace Engine {
class GameObject;

class World{
private:
    std::vector<GameObject> objects;
public:

    void loadFromFile(std::string file);

    World();
};

class GameObject{
public:
    std::string str_id;

    GameObject();
};

}

#endif
