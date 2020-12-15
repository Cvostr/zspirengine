#include "../../headers/world/World.hpp"
#include "../../headers/misc/misc.h"
#include "../../headers/world/ObjectsComponents/ZPScriptComponent.hpp"
#include "../../headers/game.h"

extern ZSGAME_DATA* game_data;

Engine::World::World(){
    objects.reserve(MAX_OBJS);

    physical_world = new PhysicalWorld(&phys_settngs);
}

Engine::World::~World() {
    delete physical_world;
}

Engine::GameObject* Engine::World::getGameObjectByStrId(std::string id){
    size_t objs_num = objects.size();
    for(size_t obj_it = 0; obj_it < objs_num; obj_it ++){ //Iterate over all objs in scene
        GameObject* obj_ptr = this->objects[obj_it]; //Get pointer to checking object
        if(obj_ptr->str_id.compare(id) == 0) //if labels are same
            return obj_ptr; //Return founded object
    }
    return nullptr; //if we haven't found one
}

Engine::GameObject* Engine::World::getGameObjectByLabel(const std::string& label){
    size_t objs_num = objects.size();
    for(size_t obj_it = 0; obj_it < objs_num; obj_it ++){ //Iterate over all objs in scene
        GameObject* obj_ptr = this->objects[obj_it]; //Get pointer to checking object
        if(!obj_ptr->mAlive) continue;
        if (obj_ptr->getLabelProperty() == nullptr)
            continue;
        if(obj_ptr->getLabelProperty()->label.compare(label) == 0) //if labels are same
            return obj_ptr; //Return founded object
    }
    return nullptr; //if we haven't found one
}

Engine::GameObject* Engine::World::getGameObjectByArrayIndex(unsigned int index) {
    size_t objs_num = objects.size();
    for (size_t obj_it = 0; obj_it < objs_num; obj_it++) { //Iterate over all objs in scene
        GameObject* obj_ptr = this->objects[obj_it]; //Get pointer to checking object
        if (!obj_ptr->mAlive) continue;
        if (obj_ptr->array_index == index) //if labels are same
            return obj_ptr; //Return founded object
    }
    return nullptr; //if we haven't found one
}

Engine::GameObject* Engine::World::addObject(const GameObject& obj){
    //Generating new object
    GameObject* newobj = new GameObject;
    *newobj = obj;
    //Getting index of free object space
    unsigned int free_index = getFreeObjectSpaceIndex();
    //all indices are busy by objects
    if(free_index == objects.size()){ 
        this->objects.push_back(newobj);
        newobj->array_index = static_cast<int>(objects.size() - 1);
    }else{
        this->objects[free_index] = newobj;
        newobj->array_index = free_index;
    }

    GameObject* obj_ptr = this->objects[free_index];
    //Assigning pointer to world
    obj_ptr->mWorld = this;

    return obj_ptr;
}

void Engine::World::removeObj(Engine::GameObjectLink& link) {
    Engine::GameObjectLink l = link;
    Engine::GameObject* ObjectPtr = l.updLinkPtr();
    if (ObjectPtr == nullptr)
        return;
    ObjectPtr->mAlive = false; //Mark object as dead
    //Get amount of children
    size_t children_num = l.updLinkPtr()->mChildren.size();

    for (size_t ch_i = 0; ch_i < children_num; ch_i++) { //Walk through all children an remove them
        Engine::GameObjectLink link = l.updLinkPtr()->mChildren[children_num - ch_i - 1]; //Remove first of children because of trim
        removeObj(link);
    }
    //Remove all content in heap, related to object class object
    l.ptr->clearAll();
    if (l.ptr->hasParent == true) { //If object parented by other obj
        Engine::GameObject* parent = l.ptr->mParent.updLinkPtr(); //Receive pointer to object's parent

        size_t children_am = parent->mChildren.size(); //get children amount
        for (size_t i = 0; i < children_am; i++) { //Iterate over all children in object
            Engine::GameObjectLink* link_ptr = &parent->mChildren[i];
            if (l.obj_str_id.compare(link_ptr->obj_str_id) == 0) { //if str_id in requested link compares to iteratable link
                parent->mChildren[i].crack(); //Make link broken
            }
        }
        //Remove cracked link from vector
        parent->trimChildrenArray(); 
    }
}

void Engine::World::removeObject(GameObject* obj) {
    Engine::GameObjectLink link = obj->getLinkToThisObject();
    //call object remove
    removeObj(link);
}

Engine::GameObject* Engine::World::newObject() {
    Engine::GameObject obj; //Creating base gameobject
    int add_num = 0; //Declaration of addititonal integer
    getAvailableNumObjLabel("GameObject_", &add_num);

    obj.mWorld = this;
    obj.addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_LABEL);
    obj.setLabel("GameObject_" + std::to_string(add_num)); //Assigning label to object

    obj.addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM);
    return this->addObject(obj); //Return pointer to new object
}

void Engine::World::trimObjectsList(){
    for (size_t i = 0; i < objects.size(); i ++) { //Iterating over all objects
        if(objects[i]->mAlive == false){ //If object marked as deleted
            delete objects[i];
            for (size_t obj_i = i + 1; obj_i < objects.size(); obj_i ++) { //Iterate over all next chidren
                objects[obj_i - 1] = objects[obj_i]; //Move it to previous place
                objects[obj_i - 1]->array_index = obj_i - 1;

            }
            objects.pop_back(); //remove last position
        }
    }
}

Engine::GameObject* Engine::World::dublicateObject(GameObject* original, bool parent){
    GameObject _new_obj;//Create an empty
    GameObject* new_obj = addObject(_new_obj);

    //Copying properties data
    for(unsigned int prop_i = 0; prop_i < original->props_num; prop_i ++){
        //Get pointer to original property
        auto prop_ptr = original->mComponents[prop_i];
        //register new property in new object
        new_obj->addProperty(prop_ptr->type);
        //Get created property
        auto new_prop = new_obj->getPropertyPtrByType(prop_ptr->type);
        //start property copying
        prop_ptr->copyTo(new_prop);
    }
    //Copying scripts data
    for (unsigned int script_i = 0; script_i < original->scripts_num; script_i++) {
        //Get pointer to original property
        auto prop_ptr = original->mScripts[script_i];
        //register new property in new object
        new_obj->addScript();
        //Get created property
        auto new_prop = new_obj->mScripts[script_i];
        //start property copying
        prop_ptr->copyTo(new_prop);
    }

    if(original->hasParent){ //if original has parent
        TransformProperty* transform = new_obj->getTransformProperty();
        Vec3 p_translation = Vec3(0,0,0);
        Vec3 p_scale = Vec3(1,1,1);
        Vec3 p_rotation = Vec3(0,0,0);
        original->mParent.ptr->getTransformProperty()->getAbsoluteParentTransform(p_translation, p_scale, p_rotation);
        transform->translation = transform->translation + p_translation;
        transform->scale = transform->scale * p_scale;
        transform->rotation = transform->rotation + p_rotation;
        if(parent == true)
            original->mParent.ptr->addChildObject(new_obj->getLinkToThisObject());
    }

    //Set new name for object
    LabelProperty* label_prop = new_obj->getLabelProperty(); //Obtain pointer to label property
    std::string to_paste;
    //generate random string to add it after object label
    genRandomString(&to_paste, 3);
    //Add generated string to end of label
    new_obj->setLabel(label_prop->label + "_" + to_paste);
    //Dublicate chilldren object
    unsigned int children_amount = static_cast<unsigned int>(original->mChildren.size());
    //Iterate over all children
    for(unsigned int child_i = 0; child_i < children_amount; child_i ++){
        //Get pointer to original child object
        GameObjectLink link = original->mChildren[child_i];
        //create new child obect by dublication of original
        GameObject* new_child = dublicateObject(link.ptr, false);
        //parenting
        new_obj->addChildObject(new_child->getLinkToThisObject());
    }

    return new_obj;
}
Engine::GameObject* Engine::World::Instantiate(GameObject* original){
    return this->dublicateObject(original);
}

void Engine::World::clear() {
    //iterate over all objects and purge them all
    for (size_t objs_i = 0; objs_i < objects.size(); objs_i++) {
        GameObject* obj_ptr = objects[objs_i];
        removeObject(obj_ptr);
    }
    objects.clear();
}

void Engine::World::loadFromFile(std::string file, RenderSettings* settings_ptr){
    std::ifstream stream;
    stream.open(file, std::ifstream::binary | std::ifstream::ate); //open world file in binary mode
    unsigned int size = static_cast<unsigned int>(stream.tellg());
    
    stream.seekg(0);
    char* data = new char[size];
    stream.read(data, size);
    //Process World
    loadFromMemory(data, size, settings_ptr);
    //Free allocated data
    delete[] data;
    //Close world file stream
    stream.close();
}

void Engine::World::loadGameObjectFromMemory(GameObject* object_ptr, const char* bytes, unsigned int left_bytes) {
    std::string prefix;
    unsigned int iter = 1;
    object_ptr->mWorld = this; //Assign pointer to world
    object_ptr->str_id.clear(); //Clear old string id
    readString(object_ptr->str_id, bytes, iter);
    //Read ACTIVE and STATIC flags
    readBinaryValue(&object_ptr->mActive, bytes + iter, iter);
    readBinaryValue(&object_ptr->IsStatic, bytes + iter, iter);
    //Then do the same sh*t, iterate until "G_END" came up
    while (true) {

        prefix.clear();
        //Read prefix
        skipSpaces(bytes, iter);

        while (bytes[iter] != ' ' && bytes[iter] != '\n' && iter < left_bytes) {
            if(bytes[iter] != '\0')
                prefix += bytes[iter];
            iter++;
        }

        if (prefix.compare("G_END") == 0) { //If end reached
            break; //Then end this infinity loop
        }
        if (prefix.compare("G_CHI") == 0) { //Ops, it is chidren header
            unsigned int amount = 0;
            iter++;
            //Read amount of children of object
            readBinaryValue(&amount, bytes + iter, iter);
            //Iterate over these children
            for (unsigned int ch_i = 0; ch_i < amount; ch_i++) { //Iterate over all written children to file
                std::string child_str_id;
                readString(child_str_id, bytes, iter);
                //Create link for object
                GameObjectLink link;
                link.world_ptr = this; //Setting world pointer
                link.obj_str_id = child_str_id; //Setting string ID
                object_ptr->mChildren.push_back(link); //Adding to object
            }
        }
        if (prefix.compare("G_PROPERTY") == 0) { //We found an property, zaeb*s'
            //Call function to load that property
            PROPERTY_TYPE type;
            iter++; //Skip space
            //Read property type
            readBinaryValue(&type, bytes + iter, iter);
            //Spawn new property with readed type
            object_ptr->addProperty(type);
            auto prop_ptr = object_ptr->getPropertyPtrByType(type); //get created property
            //Read ACTIVE flag
            readBinaryValue(&prop_ptr->active, bytes + iter, iter);
            //Load property
            prop_ptr->loadPropertyFromMemory(bytes + iter, object_ptr);
        }
        if (prefix.compare("G_SCRIPT") == 0) { //We found an script, zaeb*s'
            //Call function to load that property
            iter++; //Skip space
            //Spawn new property with readed type
            object_ptr->addScript();
            auto script_ptr = object_ptr->mScripts[object_ptr->scripts_num - 1]; //get created property
            //Read ACTIVE flag
            readBinaryValue(&script_ptr->active, bytes + iter, iter);
            //Load property
            script_ptr->loadPropertyFromMemory(bytes + iter, object_ptr);
        }
    }
}

void Engine::World::loadFromMemory(const char* bytes, unsigned int size, RenderSettings* settings_ptr) {
    //Clear all objects
    clear();
    unsigned int iter = 0;
    std::string test_header;
    //Read header
    while (bytes[iter] != ' ') {
        test_header += bytes[iter];
        iter++;
    }

    if (test_header.compare("ZSP_SCENE") != 0) //If it isn't zspire scene
        return; //Go out, we have nothing to do
    iter++;
    int version = 0; //define version on world file
    int objs_num = 0; //define number of objects
    //Parse Version Integer
    readBinaryValue(&version, bytes + iter, iter);
    //Read amount of objects
    readBinaryValue(&objs_num, bytes + iter, iter);

    while (iter < size) { //until file is over
        std::string prefix;
        //read prefix
        skipSpaces(bytes, iter);
        while (bytes[iter] != ' ' && bytes[iter] != '\n') {
            prefix += bytes[iter];
            iter++;
        }

        if (prefix.compare("RENDER_SETTINGS_AMB_COLOR") == 0) { //if it is render setting of ambient light color
            iter++;
            readBinaryValue(&settings_ptr->ambient_light_color.r, bytes + iter, iter);
            readBinaryValue(&settings_ptr->ambient_light_color.g, bytes + iter, iter);
            readBinaryValue(&settings_ptr->ambient_light_color.b, bytes + iter, iter);
        }

        if (prefix.compare("G_OBJECT") == 0) { //if it is game object
            GameObject obj;
            //Call function to load object
            loadGameObjectFromMemory(&obj, bytes + iter, size - iter);
            if(obj.getLabelProperty() != nullptr)
                //Add object to scene
                this->addObject(obj);
        }
    }
    //Now iterate over all objects and set depencies
    for (size_t obj_i = 0; obj_i < this->objects.size(); obj_i++) {
        GameObject* obj_ptr = this->objects[obj_i];
        for (size_t chi_i = 0; chi_i < obj_ptr->mChildren.size(); chi_i++) { //Now iterate over all children
            GameObjectLink* child_ptr = &obj_ptr->mChildren[chi_i];
            GameObject* child_go_ptr = child_ptr->updLinkPtr();
            if (child_go_ptr == nullptr) {
                game_data->out_manager->spawnRuntimeError(RuntimeErrorType::RE_TYPE_SCENE_OPEN_ERROR);
                continue;
            }
            child_go_ptr->mParent = obj_ptr->getLinkToThisObject();
            child_go_ptr->hasParent = true;
        }
    }
}

Engine::Camera* Engine::World::getCameraPtr(){
    return &this->world_camera;
}

void Engine::World::call_onStart() {
    for (unsigned int object_i = 0; object_i < objects.size(); object_i++) {
        Engine::GameObject* object_ptr = objects[object_i];
        object_ptr->onStart();
    }
}

void Engine::World::call_onStop() {
    for (unsigned int object_i = 0; object_i < objects.size(); object_i++) {
        Engine::GameObject* object_ptr = objects[object_i];
        object_ptr->onStop();
    }
}

void Engine::World::call_onScriptChanged() {
    size_t objs_num = objects.size();
    for (size_t obj_it = 0; obj_it < objs_num; obj_it++) {
        Engine::GameObject* obj = objects[obj_it];
        unsigned int scripts_num = obj->scripts_num;
        for (unsigned int s_i = 0; s_i < scripts_num; s_i++) {
            ZPScriptProperty* script_prop = obj->mScripts[s_i];
            script_prop->OnScriptChanges();
        }

    }
}

bool Engine::World::isObjectLabelUnique(const std::string& label) {
    size_t objs_num = objects.size();
    int ret_amount = 0;
    for (size_t obj_it = 0; obj_it < objs_num; obj_it++) { //Iterate over all objs in scene
        Engine::GameObject* obj_ptr = this->objects[obj_it]; //Get pointer to checking object
        //if object was destroyed
        if (!obj_ptr->mAlive) continue;
        if (obj_ptr->getLabelPtr()->compare(label) == 0) {
            ret_amount += 1;
            if (ret_amount > 1) return false;
        }
    }
    return true;
}

void Engine::World::getAvailableNumObjLabel(std::string label, int* result) {
    unsigned int objs_num = static_cast<unsigned int>(this->objects.size());
    std::string tocheck_str = label + std::to_string(*result); //Calculating compare string
    bool hasEqualName = false; //true if we already have this obj
    for (unsigned int obj_it = 0; obj_it < objs_num; obj_it++) { //Iterate over all objs in scene
        Engine::GameObject* obj_ptr = this->objects[obj_it]; //Get pointer to checking object
        if (!obj_ptr->mAlive) continue;
        if (obj_ptr->getLabelPtr()->compare(tocheck_str) == 0) //If label on object is same
            hasEqualName = true; //Then we founded equal name
    }
    if (hasEqualName == true) {
        *result += 1;
        getAvailableNumObjLabel(label, result);
    }
}
int Engine::World::getFreeObjectSpaceIndex() {
    size_t index_to_push = objects.size(); //Set free index to objects amount
    size_t objects_num = objects.size();
    for (unsigned int objs_i = 0; objs_i < objects_num; objs_i++) {
        if (objects[objs_i]->mAlive == false) { //if object deleted
            index_to_push = objs_i; //set free index to index of deleted object
            break;
        }
    }

    //return result
    return static_cast<int>(index_to_push);
}

void Engine::World::processPrefabObject(Engine::GameObject* object_ptr, std::vector<Engine::GameObject>* objects_array) {
    unsigned int props_amount = object_ptr->props_num;
    //iterate over all props and update gameobject links
    for (unsigned int prop_i = 0; prop_i < props_amount; prop_i++) {
        Engine::IGameObjectComponent* prop_ptr = object_ptr->mComponents[prop_i];
        prop_ptr->go_link.obj_str_id = object_ptr->str_id; //set new string id

        prop_ptr->go_link.updLinkPtr();
    }
    //Obtain amount of objects
    size_t children_amount = object_ptr->mChildren.size();
    //Iterate over all objects
    for (size_t chi_i = 0; chi_i < children_amount; chi_i++) {
        Engine::GameObjectLink link = object_ptr->mChildren[chi_i];
        //find object with same string name as in link
        for (unsigned int obj_i = 0; obj_i < objects_array->size(); obj_i++) {
            Engine::GameObject* _object_ptr = &objects_array->at(obj_i);
            if (_object_ptr->str_id.compare(link.obj_str_id) == 0) { //we found object
                genRandomString(&_object_ptr->str_id, 15); //generate new string ID
                object_ptr->mChildren[chi_i].obj_str_id = _object_ptr->str_id; //update string id in children array
                _object_ptr->mParent.obj_str_id = object_ptr->str_id; //update string ID of parent link
                _object_ptr->hasParent = true;

                processPrefabObject(_object_ptr, objects_array);
            }
        }
    }
}

Engine::GameObject* Engine::World::addObjectsFromPrefab(char* data, unsigned int size) {
    unsigned int iter = 0;
    std::string test_header;
    //Read header
    while (data[iter] != ' ' && data[iter] != '\n') {
        test_header += data[iter];
        iter++;
    }

    if (test_header.compare("ZSPIRE_PREFAB") != 0) //If it isn't zspire scene
        return nullptr; //Go out, we have nothing to do
    iter++;
    //array for objects
    std::vector<Engine::GameObject> mObjects;

    while (iter < size) { //until file is over
        std::string prefix;
        //read prefix
        skipSpaces(data, iter);
        while (data[iter] != ' ' && data[iter] != '\n') {
            prefix += data[iter];
            iter++;
        }
        //if we found an object
        if (!prefix.compare("G_OBJECT")) {
            Engine::GameObject obj;
            this->loadGameObjectFromMemory(&obj, data + iter, size - iter);
            mObjects.push_back(obj);
        }
    }

    genRandomString(&mObjects[0].str_id, 15); //generate new string ID for first object
    processPrefabObject(&mObjects[0], &mObjects);
    //iterate over all objects and push them to world
    for (size_t obj_i = 0; obj_i < mObjects.size(); obj_i++) {
        std::string label = mObjects[obj_i].getLabel();
        int add_num = 0; //Declaration of addititonal integer
        getAvailableNumObjLabel(label, &add_num);
        //Set new label to object
        *mObjects[obj_i].getLabelPtr() += std::to_string(add_num); 
        //Spawn new object
        Engine::GameObject* newObjPtr = this->addObject(mObjects[obj_i]);
    }
    Engine::GameObject* object = this->getGameObjectByStrId(mObjects[0].str_id);

    for (size_t obj_i = 1; obj_i < mObjects.size(); obj_i++) {
        Engine::GameObject* object_ptr = this->getGameObjectByStrId(mObjects[obj_i].str_id);
        object_ptr->mParent.world_ptr = this;
    }
    object->setMeshSkinningRootNodeRecursively(object);
    return object;
}

void Engine::World::putToShapshot(WorldSnapshot* snapshot) {
    //iterate over all objects in scene
    for (size_t objs_num = 0; objs_num < this->objects.size(); objs_num++) {
        //Obtain pointer to object
        Engine::GameObject* obj_ptr = this->objects[objs_num];
        if (obj_ptr->mAlive == false) continue;
        //Iterate over all properties in object and copy them into snapshot
        for (unsigned int prop_i = 0; prop_i < obj_ptr->props_num; prop_i++) {
            auto prop_ptr = obj_ptr->mComponents[prop_i];
            auto new_prop = Engine::allocProperty(prop_ptr->type);
            new_prop->go_link = prop_ptr->go_link;
            prop_ptr->copyTo(new_prop);
            snapshot->props.push_back(new_prop);
        }
        //Iterate over all scripts in objects and copy them into snapshot
        for (unsigned int script_i = 0; script_i < obj_ptr->scripts_num; script_i++) {
            Engine::ZPScriptProperty* script_ptr = static_cast<Engine::ZPScriptProperty*>
                (obj_ptr->mScripts[script_i]);
            Engine::ZPScriptProperty* script_prop = static_cast<Engine::ZPScriptProperty*>
                (Engine::allocProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_AGSCRIPT));
            script_prop->go_link = script_ptr->go_link;
            script_ptr->copyTo(script_prop);
            snapshot->scripts.push_back(script_prop);
        }
        //Decalare new object
        Engine::GameObject newobj;
        //Copy object data
        obj_ptr->copyTo(&newobj);
        snapshot->objects.push_back(newobj);
    }
}

Engine::WorldSnapshot::WorldSnapshot() {

}
void Engine::WorldSnapshot::clear() {
    this->objects.clear(); //clear all object
    //iterate over all properties
    for (size_t prop_it = 0; prop_it < props.size(); prop_it++) {
        delete props[prop_it];
    }
    for (size_t script_it = 0; script_it < scripts.size(); script_it++) {
        delete scripts[script_it];
    }
    props.clear();
    scripts.clear();
}
