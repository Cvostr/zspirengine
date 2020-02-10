#ifndef zs_world
#define zs_world

#include <string>
#include <vector>
#include <fstream>

#include "../engine/resources.h"
#include "zs-camera.h"

#include "Physics.h"
#include "../render/zs-pipeline.h"

#define OBJ_PROPS_SIZE 11
#define MAX_OBJS 15000

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
    GO_PROPERTY_TYPE_RIGIDBODY,
    GO_PROPERTY_TYPE_SKYBOX,
    GO_PROPERTY_TYPE_SHADOWCASTER,
    GO_PROPERTY_TYPE_TERRAIN,
    GO_PROPERTY_TYPE_CHARACTER_CONTROLLER,
    GO_PROPERTY_TYPE_NODE,
    GO_PROPERTY_TYPE_ANIMATION,
    GO_PROPERTY_TYPE_TILE_GROUP = 1000,
    GO_PROPERTY_TYPE_TILE = 1001
};

enum COLLIDER_TYPE {COLLIDER_TYPE_NONE,
                    COLLIDER_TYPE_BOX,
                    COLLIDER_TYPE_CUBE,
                    COLLIDER_TYPE_SPHERE,
                    COLLIDER_TYPE_CONVEX_HULL,
                    COLLIDER_TYPE_MESH};

namespace Engine {
class GameObject;
class World;

class TransformProperty;
class LabelProperty;

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

class GameObjectProperty{
public:
    PROPERTY_TYPE type; //type of property
    GameObjectLink go_link; //link to object, that holds this property
    bool active; //Is property working

    World* world_ptr;

    void setActive(bool active);
    bool isActive();

    virtual void copyTo(GameObjectProperty* dest);
    virtual void onUpdate(float deltaTime);
    virtual void onPreRender(RenderPipeline* pipeline = nullptr);
    virtual void onRender(RenderPipeline* pipeline = nullptr);
    virtual void onAddToObject();
    virtual void onObjectDeleted();
    virtual void onTrigger(Engine::GameObject* obj);
    //Editor specific functions
    virtual void addPropertyInterfaceToInspector();
    virtual void onValueChanged();

    GameObjectProperty();
    virtual ~GameObjectProperty();
};

class World{
private:
public:
    PhysicalWorldSettings phys_settngs;
    PhysicalWorld* physical_world;

    std::vector<GameObject> objects;
    void loadFromFile(std::string file, RenderSettings* settings_ptr);
    void loadGameObject(GameObject* object_ptr, std::ifstream* world_stream);

    GameObject* getGameObjectByStrId(std::string id);
    GameObject* getGameObjectByLabel(std::string label);
    GameObject* addObject(GameObject obj);
    void removeObject(GameObject* object);
    GameObject* dublicateObject(GameObject* original, bool parent = true);
    GameObject* Instantiate(GameObject* original);
    void addObjectsFromPrefab(std::string file);

    Engine::Camera* getCameraPtr();
    Engine::Camera world_camera;

    World();
};

class GameObject{
public:
    //Index in objects vector
    int array_index;
    std::string str_id; //Object's unique string ID
    std::string* label_ptr;

    bool active; //if true, object will be active in scene
    bool alive;
    bool hasParent;
    bool IsStatic;

    unsigned int props_num; //amount of properties
    World* world_ptr; //pointer to world, when object placed

    GameObjectLink parent;

    std::vector<GameObjectLink> children; //Vector to store links to children of object
    GameObjectProperty* properties[OBJ_PROPS_SIZE];

    void loadProperty(std::ifstream* world_stream); //Loads one property from stream

    GameObjectProperty* allocProperty(int type);
    bool addProperty(int property); //Adds property with property ID
    GameObjectProperty* getPropertyPtrByType(PROPERTY_TYPE type);

    void addChildObject(GameObjectLink link);
    void removeChildObject(GameObjectLink link);
    void removeProperty(int index);

    void clearAll();
    //remove deleted children from vector
    void trimChildrenArray();

    std::string getLabel();
    void setLabel(std::string label);
    void setActive(bool active);

    template<typename T>
    T* getPropertyPtr(){
        unsigned int props = static_cast<unsigned int>(this->props_num);
        for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
            auto property_ptr = this->properties[prop_i];
            if(typeid( *property_ptr) == typeid(T)){ //If object already has one
                return static_cast<T*>(property_ptr); //return it
            }
        }
        return nullptr;
    }

    GameObject* getChildObjectWithNodeLabel(std::string label);

    TransformProperty* getTransformProperty();
    LabelProperty* getLabelProperty();

    GameObjectLink getLinkToThisObject();
    void copyTo(GameObject* dest);
    void processObject(RenderPipeline* pipeline); //On render pipeline wish to work with object
    void Draw(RenderPipeline* pipeline); //On render pipeline wish to draw the object
    void onUpdate(int deltaTime); //calls onUpdate on all properties
    void onPreRender(RenderPipeline* pipeline); //calls onPreRender on all properties
    void onRender(RenderPipeline* pipeline); //calls onRender on all properties
    bool isRigidbody();

    GameObject();
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

    ZSVECTOR3 abs_translation;
    ZSVECTOR3 abs_scale;
    ZSVECTOR3 abs_rotation;

    void updateMat();
    void getAbsoluteParentTransform(ZSVECTOR3& t, ZSVECTOR3& s, ZSVECTOR3& r);
    void copyTo(GameObjectProperty* dest);
    void onPreRender(RenderPipeline* pipeline);
    void getAbsoluteRotationMatrix(ZSMATRIX4x4& m);

    void setTranslation(ZSVECTOR3 new_translation);
    void setScale(ZSVECTOR3 new_scale);
    void setRotation(ZSVECTOR3 new_rotation);

    TransformProperty();
};

class ColliderProperty : public GameObjectProperty{
public:
    void onAddToObject(); //will register in world
    void onObjectDeleted(); //unregister in world
    void copyTo(GameObjectProperty* dest);

    TransformProperty* getTransformProperty();

    bool isTrigger;
    COLLIDER_TYPE coll_type;

    ColliderProperty();
};


}

#endif
