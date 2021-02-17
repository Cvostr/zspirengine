#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "../engine/Resources.hpp"
#include "../misc/misc.h"
#include "../misc/SmArray.hpp"
#include "Camera.hpp"

#include "Physics.h"
#include "../render/Renderer.hpp"

#include "../Scripting/AngelScriptMgr.h"

#include "../math/BB3.hpp"

#define OBJ_PROPS_SIZE 10
#define OBJ_SCRIPT_PROPS_SIZE 5
#define MAX_OBJS 15000

enum class PROPERTY_TYPE{
    GO_PROPERTY_TYPE_NONE,
    GO_PROPERTY_TYPE_TRANSFORM,
    GO_PROPERTY_TYPE_LABEL,
    GO_PROPERTY_TYPE_MESH,
    GO_PROPERTY_TYPE_LIGHTSOURCE,
    GO_PROPERTY_TYPE_AUDSOURCE,
    GO_PROPERTY_TYPE_MATERIAL,
    GO_PROPERTY_TYPE_AGSCRIPT,
    GO_PROPERTY_TYPE_COLLIDER,
    GO_PROPERTY_TYPE_RIGIDBODY,
    GO_PROPERTY_TYPE_SKYBOX,
    GO_PROPERTY_TYPE_SHADOWCASTER,
    GO_PROPERTY_TYPE_TERRAIN,
    GO_PROPERTY_TYPE_CHARACTER_CONTROLLER,
    GO_PROPERTY_TYPE_NODE,
    GO_PROPERTY_TYPE_ANIMATION,
    GO_PROPERTY_TYPE_TRIGGER,
    GO_PROPERTY_TYPE_WINDZONE,
    GO_PROPERTY_TYPE_PARTICLE_EMITTER,

    GO_PROPERTY_TYPE_CAMERA,

    GO_PROPERTY_TYPE_TILE_GROUP = 1000,
    GO_PROPERTY_TYPE_TILE = 1001
};

enum class COLLIDER_TYPE {COLLIDER_TYPE_NONE,
                    COLLIDER_TYPE_BOX,
                    COLLIDER_TYPE_CUBE,
                    COLLIDER_TYPE_SPHERE,
                    COLLIDER_TYPE_CONVEX_HULL,
                    COLLIDER_TYPE_MESH};

typedef unsigned long long ViewMask;

namespace Engine {
class GameObject;
class GameObjectSnapshot;
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

class IGameObjectComponent{
public:
    PROPERTY_TYPE type; //type of property
    GameObjectLink go_link; //link to object, that holds this property
    bool active; //Is property working
    //Pointer to World class
    World* mWorld;
    //Func to change active state
    void setActive(bool active);
    //Check, if Property is Active
    bool isActive();
    //Virtual func to copy all property's values
    virtual void copyTo(IGameObjectComponent* dest);
    //Virtual func to define property behaviour on scene start
    virtual void onStart(){}
    //Virtual func to define property behaviour on scene stop
    virtual void onStop(){}
    //Virtual func to define property behaviour each frame
    virtual void onUpdate(float deltaTime){}
    virtual void onPreRender(Renderer* pipeline = nullptr){}
    virtual void onRender(Renderer* pipeline = nullptr){}
    virtual void onAddToObject(){}
    virtual void onObjectDeleted(){}
    virtual void loadPropertyFromMemory(const char* data, GameObject* obj);
    virtual void savePropertyToStream(ZsStream* stream, GameObject* obj);
    virtual void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);
    virtual void onTrigger(Engine::GameObject* obj){}
    virtual void onTriggerEnter(Engine::GameObject* obj){}
    virtual void onTriggerExit(Engine::GameObject* obj){}
    //Editor specific functions
    virtual void addPropertyInterfaceToInspector(){}
    virtual void onValueChanged(){}

    IGameObjectComponent();
    virtual ~IGameObjectComponent();
};

template<class T>
int bindGameObjectPropertySDK(AGScriptMgr* mgr, const char* obj_type) {
    int result = 0;

    T* prop = new T;
    prop->bindObjectPropertyToAngel(mgr);
    delete prop;

    result = mgr->RegisterObjectMethod(obj_type, "void setActive(bool)", asMETHOD(IGameObjectComponent, setActive), asCALL_THISCALL);
    return result;
}

class WorldSnapshot {
public:
    std::vector<Engine::GameObject> objects;
    std::vector<Engine::IGameObjectComponent*> props;
    std::vector<Engine::IGameObjectComponent*> scripts;

    WorldSnapshot();
    void clear();
};

class World{
private:
public:
    std::vector<int> picked_objs_ids;

    PhysicalWorldSettings phys_settngs;
    PhysicalWorld* physical_world;

    SmArray<GameObject*> objects;//Vector, containing all gameobjects

    void loadFromMemory(const char* bytes, unsigned int size, RenderSettings* settings_ptr);
    void loadFromFile(std::string file, RenderSettings* settings_ptr);
    void loadGameObjectFromMemory(GameObject* object_ptr, const char* bytes, unsigned int left_bytes);

    GameObject* getGameObjectByStrId(std::string id);
    GameObject* getGameObjectByLabel(const std::string& label);
    GameObject* getGameObjectByArrayIndex(unsigned int index);
    GameObject* addObject(const GameObject& obj);
    Engine::GameObject* newObject(); //Add new object to world
    void removeObj(Engine::GameObjectLink& link); //Remove object from world
    void removeObject(GameObject* obj); //Remove object from world
    GameObject* dublicateObject(GameObject* original, bool parent = true);
    GameObject* Instantiate(GameObject* original);

    void processPrefabObject(Engine::GameObject* object_ptr, std::vector<Engine::GameObject>* objects_array);
    GameObject* addObjectsFromPrefab(char* data, unsigned int size);
    GameObject* addObjectsFromPrefab(PrefabResource* Prefab);

    void call_onStart();
    void call_onStop();
    void call_onScriptChanged();

    //Destroys all data inside
    void clear();

    void trimObjectsList();
    int getFreeObjectSpaceIndex();
    bool isObjectLabelUnique(const std::string& label); //Get amount of objects with this label
    void getAvailableNumObjLabel(std::string label, int* result);

    void putToShapshot(WorldSnapshot* snapshot);

    GameObject* RayTestFirstObject(Vec3 pos, Vec3 dir, float far) {
        return static_cast<GameObject*>(physical_world->RayTestFirstObject(pos, dir, far));
    }

    World();
    ~World();
};

class ZPScriptComponent;
class PhysicalProperty;

class GameObject{
public:
    //Index in objects vector
    int array_index;
    std::string str_id; //Object's unique string ID
    std::string* getLabelPtr();

    ViewMask mViewMask;
    bool mActive; //if true, object will be active in scene
    bool mAlive; //false, if object was remove
    bool hasParent;
    bool IsStatic;

    unsigned int props_num; //amount of properties
    unsigned int scripts_num;

    World* mWorld; //pointer to world, when object placed

    BoundingBox3 mBoundingBox;

    GameObjectLink mParent;

    std::vector<GameObjectLink> mChildren; //Vector to store links to children of object
    GameObject* getChild(unsigned int index);
    //Pointers to properties and scripts
    IGameObjectComponent* mComponents[OBJ_PROPS_SIZE];
    ZPScriptComponent* mScripts[OBJ_SCRIPT_PROPS_SIZE];
    //Allocate property in this object
    
    bool addScript();
    bool addProperty(PROPERTY_TYPE property); //Adds property with property ID
    //returns pointer to property by property type
    Engine::IGameObjectComponent* getPropertyPtrByType(PROPERTY_TYPE type);
    Engine::IGameObjectComponent* getPropertyPtrByTypeI(int property);
    //Add object link as child
    void addChildObject(GameObject* obj, bool updTransform = true);
    void addChildObject(GameObjectLink link, bool updTransform = true);
    //unparent object from this
    void removeChildObject(GameObjectLink link);
    void removeChildObject(GameObject* obj);
    int getAliveChildrenAmount(); //Gets current amount of children objects (exclude removed chidren)
    //Remove property with type
    void removeProperty(int index);
    void removeProperty(Engine::IGameObjectComponent* pProp);
    void removeScript(int index);
    void removeScript(Engine::IGameObjectComponent* pProp);
    asIScriptObject* getScriptObjectWithName(const std::string& name);

    GameObject* getChildObjectWithNodeLabel(const std::string& label);
    void setMeshSkinningRootNodeRecursively(GameObject* rootNode);

    void clearAll(); //Release all associated memory with this object
    //remove deleted children from vector
    void trimChildrenArray();

    const std::string& getLabel();
    void setLabel(const std::string& label);
    void setActive(bool active) { mActive = active; }
    bool isActive();
    void setViewMask(ViewMask Mask) { mViewMask = Mask; }
    ViewMask getViewMask() { return mViewMask; }
    size_t getChildrenNum(){ return mChildren.size(); }

    template<typename T>
    T* getPropertyPtr(){
        unsigned int props = static_cast<unsigned int>(this->props_num);
        for(unsigned int prop_i = 0; prop_i < props; prop_i ++){
            auto property_ptr = this->mComponents[prop_i];
            if(typeid( *property_ptr) == typeid(T)){ //If object already has one
                return static_cast<T*>(property_ptr); //return it
            }
        }
        return nullptr;
    }

    TransformProperty* getTransformProperty();
    LabelProperty* getLabelProperty();

    GameObjectLink getLinkToThisObject();
    void copyTo(GameObject* dest);
    void processObject(Renderer* pipeline); //On render pipeline wish to work with object

    void onStart(); //calls onStart() on all properties
    void onStop(); //calls onStop() on all properties
    void onUpdate(int deltaTime); //calls onUpdate on all properties
    void onPreRender(Renderer* pipeline); //calls onPreRender on all properties
    void onRender(Renderer* pipeline); //calls onRender on all properties
    void onTrigger(GameObject* obj);
    void onTriggerEnter(GameObject* obj);
    void onTriggerExit(GameObject* obj);
    //true, if object has rigidbody component
    bool isRigidbody();
    PhysicalProperty* getPhysicalProperty();
    const BoundingBox3& getBoundingBox();

    bool hasMesh(); //Check if gameobject has mesh property and mesh inside
    bool hasTerrain(); //Check if gameobject has terrain inside
    bool hasLightsource();
    bool hasMaterial();

    void DrawMesh(Renderer* pipeline);
    void DrawMeshInstanced(Renderer* pipeline, unsigned int inst_num);
    void setSkinningMatrices(Renderer* pipeline);
    unsigned int getBonesCount();

    //---------------------FOR EDITOR USE--------------------------------------------------------
    void pick(); //Mark object and its children picked
    void saveProperties(ZsStream* stream); //Writes properties content at end of stream
    void saveProperty(IGameObjectComponent* prop, ZsStream* stream);
    void putToSnapshot(GameObjectSnapshot* snapshot);
    void recoverFromSnapshot(Engine::GameObjectSnapshot* snapshot);

    GameObject();
    ~GameObject();
};

class GameObjectSnapshot {
public:
    GameObject reserved_obj; //class object
    std::vector<Engine::GameObjectLink> children; //Vector to store links to children of object
    std::vector<GameObjectSnapshot> children_snapshots;
    Engine::IGameObjectComponent* properties[OBJ_PROPS_SIZE]; //pointers to properties of object
    Engine::ZPScriptComponent* mScripts[OBJ_SCRIPT_PROPS_SIZE];

    Engine::GameObjectLink parent_link;

    int props_num; //number of properties
    int scripts_num; //number of scripts

    int obj_array_ind; //index in objects array

    void clear();
    GameObjectSnapshot();
};

class LabelProperty : public IGameObjectComponent {
public:
    std::string label; //Label of gameobject

    void addPropertyInterfaceToInspector();
    void onValueChanged();
    void copyTo(Engine::IGameObjectComponent* dest);
    void loadPropertyFromMemory(const char* data, Engine::GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);

    LabelProperty();
    ~LabelProperty();
};

class TransformProperty : public IGameObjectComponent {
public:
    Mat4 transform_mat;

    Vec3 translation;
    Vec3 scale;
    Vec3 rotation;

    Vec3 abs_translation;
    Vec3 abs_scale;
    Vec3 abs_rotation;

    void updateMatrix();
    void getAbsoluteParentTransform(Vec3& t, Vec3& s, Vec3& r);
    void copyTo(IGameObjectComponent* dest);
    void onPreRender(Renderer* pipeline);
    void getAbsoluteRotationMatrix(Mat4& m);

    void setTranslation(const Vec3& new_translation);
    void setScale(const Vec3& new_scale);
    void setRotation(const Vec3& new_rotation);
    //Editor stuff
    void onValueChanged();
    void addPropertyInterfaceToInspector();

    void bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr);
    void loadPropertyFromMemory(const char* data, GameObject* obj);
    void savePropertyToStream(ZsStream* stream, GameObject* obj);

    TransformProperty();
};

IGameObjectComponent* allocProperty(PROPERTY_TYPE type);

template <typename T>
T* ASGetPropertyPtr(GameObject* Obj) {
    return Obj->getPropertyPtr<T>();
}

}
