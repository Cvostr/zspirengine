#ifndef zs_world
#define zs_world

#include <string>
#include <vector>
#include <fstream>

#include "../engine/resources.h"

#include "../render/zs-pipeline.h"

#define OBJ_PROPS_SIZE 11

enum PROPERTY_TYPE{
    GO_PROPERTY_TYPE_NONE,
    GO_PROPERTY_TYPE_TRANSFORM,
    GO_PROPERTY_TYPE_LABEL,
    GO_PROPERTY_TYPE_MESH,
    GO_PROPERTY_TYPE_LIGHTSOURCE,
    GO_PROPERTY_TYPE_AUDSOURCE,
    GO_PROPERTY_TYPE_MATERIAL,
    GO_PROPERTY_TYPE_SCRIPTGROUP,
    GO_PROPERTY_TYPE_COLLIDER,
    GO_PROPERTY_TYPE_TILE_GROUP = 1000,
    GO_PROPERTY_TYPE_TILE = 1001
};

namespace Engine {
class GameObject;
class World;
class GameObjectProperty;

class TransformProperty;

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
    ResourceManager* manager;
public:

    void loadFromFile(std::string file);
    GameObject* getGameObjectByStrId(std::string id);

    GameObject* addObject(GameObject obj);

    World(ResourceManager* manager);
};

class GameObject{
public:
    std::string str_id; //Object's unique string ID
    std::string* label_ptr;

    bool alive;
    bool hasParent;

    unsigned int props_num; //amount of properties
    World* world_ptr; //pointer to world, when object placed

    GO_RENDER_TYPE render_type;

    GameObjectLink parent;

    std::vector<GameObjectLink> children;
    GameObjectProperty* properties[OBJ_PROPS_SIZE];

    void loadProperty(std::ifstream* world_stream); //Loads one property from stream

    GameObjectProperty* allocProperty(int type);
    bool addProperty(int property); //Adds property with property ID
    GameObjectProperty* getPropertyPtrByType(PROPERTY_TYPE type);

    TransformProperty* getTransformProperty();

    GameObjectLink getLinkToThisObject();

    GameObject();
};



class GameObjectProperty{
public:
    PROPERTY_TYPE type; //type of property
    GameObjectLink go_link;

    World* world_ptr;

    virtual void copyTo(GameObjectProperty* dest);

    GameObjectProperty();
};

class LabelProperty : public GameObjectProperty {
public:
    std::string label; //Label of gameobject


    LabelProperty();
};

class TransformProperty : public GameObjectProperty {
public:
    ZSMATRIX4x4 transform_mat;

    ZSVECTOR3 translation;
    ZSVECTOR3 scale;
    ZSVECTOR3 rotation;

    void updateMat();
    void getAbsoluteParentTransform(ZSVECTOR3& t, ZSVECTOR3& s, ZSVECTOR3& r);
    //void copyTo(GameObjectProperty* dest);
    //void onPreRender(RenderPipeline* pipeline);
    void getAbsoluteRotationMatrix(ZSMATRIX4x4& m);

    void setTranslation(ZSVECTOR3 new_translation);

    TransformProperty();
};

}

#endif
