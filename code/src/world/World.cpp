#include "../../headers/world/World.h"
#include "../../headers/misc/randomg.h"

Engine::World::World(){
    objects.reserve(MAX_OBJS);
    objects.resize(0);

    physical_world = new PhysicalWorld(&phys_settngs);
}

Engine::World::~World() {
    delete physical_world;
}

Engine::GameObject* Engine::World::getGameObjectByStrId(std::string id){
    unsigned int objs_num = static_cast<unsigned int>(this->objects.size());
    for(unsigned int obj_it = 0; obj_it < objs_num; obj_it ++){ //Iterate over all objs in scene
        GameObject* obj_ptr = this->objects[obj_it]; //Get pointer to checking object
        if(obj_ptr->str_id.compare(id) == 0) //if labels are same
            return obj_ptr; //Return founded object
    }
    return nullptr; //if we haven't found one
}

Engine::GameObject* Engine::World::getGameObjectByLabel(std::string label){
    unsigned int objs_num = static_cast<unsigned int>(this->objects.size());
    for(unsigned int obj_it = 0; obj_it < objs_num; obj_it ++){ //Iterate over all objs in scene
        GameObject* obj_ptr = this->objects[obj_it]; //Get pointer to checking object
        if(!obj_ptr->alive) continue;
        if(obj_ptr->getLabelProperty()->label.compare(label) == 0) //if labels are same
            return obj_ptr; //Return founded object
    }
    return nullptr; //if we haven't found one
}

Engine::GameObject* Engine::World::getGameObjectByArrayIndex(unsigned int index) {
    unsigned int objs_num = static_cast<unsigned int>(this->objects.size());
    for (unsigned int obj_it = 0; obj_it < objs_num; obj_it++) { //Iterate over all objs in scene
        GameObject* obj_ptr = this->objects[obj_it]; //Get pointer to checking object
        if (!obj_ptr->alive) continue;
        if (obj_ptr->array_index == index) //if labels are same
            return obj_ptr; //Return founded object
    }
    return nullptr; //if we haven't found one
}

Engine::GameObject* Engine::World::addObject(GameObject obj){
    //Generating new object
    GameObject* newobj = new GameObject;
    *newobj = obj;
    //Getting index of free object space
    unsigned int free_index = static_cast<unsigned int>(this->objects.size());
    for(unsigned int obj_i = 0; obj_i < this->objects.size(); obj_i ++){
        GameObject* obj_ptr = this->objects[obj_i];
        if(obj_ptr->alive == false){
            free_index = static_cast<unsigned int>(obj_i);
        }
    }
    //all indices are busy by objects
    if(free_index == objects.size()){ 
        this->objects.push_back(newobj);
    }else{
        this->objects[free_index] = newobj;
    }

    GameObject* obj_ptr = this->objects[free_index];
    //Assigning pointer to world
    obj_ptr->world_ptr = this;

    return obj_ptr;
}

void Engine::World::removeObj(Engine::GameObjectLink& link) {
    Engine::GameObjectLink l = link;
    l.updLinkPtr()->alive = false; //Mark object as dead
    //Get amount of children
    unsigned int children_num = static_cast<unsigned int>(l.updLinkPtr()->children.size());

    for (unsigned int ch_i = 0; ch_i < children_num; ch_i++) { //Walk through all children an remove them
        Engine::GameObjectLink link = l.updLinkPtr()->children[children_num - ch_i - 1]; //Remove first of children because of trim
        removeObj(link);
    }
    //Remove all content in heap, related to object class object
    (l.ptr)->clearAll();
    if (l.ptr->hasParent == true) { //If object parented by other obj
        Engine::GameObject* parent = l.ptr->parent.updLinkPtr(); //Receive pointer to object's parent

        unsigned int children_am = static_cast<unsigned int>(parent->children.size()); //get children amount
        for (unsigned int i = 0; i < children_am; i++) { //Iterate over all children in object
            Engine::GameObjectLink* link_ptr = &parent->children[i];
            if (l.obj_str_id.compare(link_ptr->obj_str_id) == 0) { //if str_id in requested link compares to iteratable link
                parent->children[i].crack(); //Make link broken
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


void Engine::World::trimObjectsList(){
    for (unsigned int i = 0; i < objects.size(); i ++) { //Iterating over all objects
        if(objects[i]->alive == false){ //If object marked as deleted
            delete objects[i];
            for (unsigned int obj_i = i + 1; obj_i < objects.size(); obj_i ++) { //Iterate over all next chidren
                objects[obj_i - 1] = objects[obj_i]; //Move it to previous place
                objects[obj_i - 1]->array_index = obj_i - 1;

            }
            objects.resize(objects.size() - 1); //remove last position
        }
    }
}

Engine::GameObject* Engine::World::dublicateObject(GameObject* original, bool parent){
    GameObject _new_obj;//Create an empty
    GameObject* new_obj = addObject(_new_obj);

    //Copying properties data
    for(unsigned int prop_i = 0; prop_i < original->props_num; prop_i ++){
        //Get pointer to original property
        auto prop_ptr = original->properties[prop_i];
        //register new property in new object
        new_obj->addProperty(prop_ptr->type);
        //Get created property
        auto new_prop = new_obj->getPropertyPtrByType(prop_ptr->type);
        //start property copying
        prop_ptr->copyTo(new_prop);
    }

    if(original->hasParent){ //if original has parent
        TransformProperty* transform = new_obj->getTransformProperty();
        ZSVECTOR3 p_translation = ZSVECTOR3(0,0,0);
        ZSVECTOR3 p_scale = ZSVECTOR3(1,1,1);
        ZSVECTOR3 p_rotation = ZSVECTOR3(0,0,0);
        original->parent.ptr->getTransformProperty()->getAbsoluteParentTransform(p_translation, p_scale, p_rotation);
        transform->translation = transform->translation + p_translation;
        transform->scale = transform->scale * p_scale;
        transform->rotation = transform->rotation + p_rotation;
        if(parent == true)
            original->parent.ptr->addChildObject(new_obj->getLinkToThisObject());
    }

    //Set new name for object
    LabelProperty* label_prop = new_obj->getLabelProperty(); //Obtain pointer to label property
    std::string to_paste;
    //generate random string to add it after object label
    genRandomString(&to_paste, 3);
    //Add generated string to end of label
    label_prop->label = label_prop->label + "_" + to_paste;
    //Update pointer to LabelProperty
    new_obj->label_ptr = &label_prop->label;
    //Dublicate chilldren object
    unsigned int children_amount = static_cast<unsigned int>(original->children.size());
    //Iterate over all children
    for(unsigned int child_i = 0; child_i < children_amount; child_i ++){
        //Get pointer to original child object
        GameObjectLink link = original->children[child_i];
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
    for (unsigned int objs_i = 0; objs_i < objects.size(); objs_i++) {
        GameObject* obj_ptr = (GameObject*)objects[objs_i];
        removeObject(obj_ptr);
    }
    objects.clear();
}

void Engine::World::addObjectsFromPrefab(std::string file){

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
    object_ptr->world_ptr = this; //Assign pointer to world
    object_ptr->str_id.clear(); //Clear old string id
    while (bytes[iter] != ' ') {
        object_ptr->str_id += bytes[iter];
        iter++;
    }
    iter++;
    //Read ACTIVE and STATIC flags
    memcpy(&object_ptr->active, bytes + iter, sizeof(bool));
    iter += sizeof(bool);
    memcpy(&object_ptr->IsStatic, bytes + iter, sizeof(bool));
    iter += sizeof(bool);
    //Then do the same sh*t, iterate until "G_END" came up
    while (true) {
        prefix.clear();
        //Read prefix
        while (bytes[iter] == ' ' || bytes[iter] == '\n') {
            iter++;
        }
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
            memcpy(&amount, bytes + iter, sizeof(int));
            iter += sizeof(int);
            //Iterate over these children
            for (unsigned int ch_i = 0; ch_i < amount; ch_i++) { //Iterate over all written children to file
                std::string child_str_id;
                //Reading child string id
                while (bytes[iter] == ' ' || bytes[iter] == '\n') {
                    iter++;
                }
                while (bytes[iter] != ' ' && bytes[iter] != '\n') {
                    child_str_id += bytes[iter];
                    iter++;
                }
                //Create link for object
                GameObjectLink link;
                link.world_ptr = this; //Setting world pointer
                link.obj_str_id = child_str_id; //Setting string ID
                object_ptr->children.push_back(link); //Adding to object
            }
        }
        if (prefix.compare("G_PROPERTY") == 0) { //We found an property, zaeb*s'
            //Call function to load that property
            PROPERTY_TYPE type;
            iter++; //Skip space
            memcpy(&type, bytes + iter, sizeof(int));
            iter += sizeof(int);
            //Spawn new property with readed type
            object_ptr->addProperty(type);
            auto prop_ptr = object_ptr->getPropertyPtrByType(type); //get created property
            //Read ACTIVE flag
            memcpy(&prop_ptr->active, bytes + iter, sizeof(bool));
            iter += sizeof(bool);
            //Load property
            prop_ptr->loadPropertyFromMemory(bytes + iter, object_ptr);
        }
    }
}

void Engine::World::loadFromMemory(const char* bytes, unsigned int size, RenderSettings* settings_ptr) {
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
    memcpy(&version, bytes + iter, sizeof(int));
    iter += 4;
    //Read amount of objects
    memcpy(&objs_num, bytes + iter, sizeof(int));
    iter += 5;

    while (iter < size) { //until file is over
        std::string prefix;
        //read prefix
        while (bytes[iter] == ' ' || bytes[iter] == '\n') {
            iter++;
        }
        while (bytes[iter] != ' ' && bytes[iter] != '\n') {
            prefix += bytes[iter];
            iter++;
        }

        if (prefix.compare("RENDER_SETTINGS_AMB_COLOR") == 0) { //if it is render setting of ambient light color
            iter++;
            memcpy(&settings_ptr->ambient_light_color.r, bytes + iter, sizeof(int));
            iter += 4;
            memcpy(&settings_ptr->ambient_light_color.g, bytes + iter, sizeof(int));
            iter += 4;
            memcpy(&settings_ptr->ambient_light_color.b, bytes + iter, sizeof(int));
            iter += 4;
        }

        if (prefix.compare("G_OBJECT") == 0) { //if it is game object
            GameObject obj;
            //Call function to load object
            loadGameObjectFromMemory(&obj, bytes + iter, size - iter);
            //Add object to scene
            this->addObject(obj);
        }
    }
    //Now iterate over all objects and set depencies
    for (unsigned int obj_i = 0; obj_i < this->objects.size(); obj_i++) {
        GameObject* obj_ptr = this->objects[obj_i];
        for (unsigned int chi_i = 0; chi_i < obj_ptr->children.size(); chi_i++) { //Now iterate over all children
            GameObjectLink* child_ptr = &obj_ptr->children[chi_i];
            GameObject* child_go_ptr = child_ptr->updLinkPtr();
            child_go_ptr->parent = obj_ptr->getLinkToThisObject();
            child_go_ptr->hasParent = true;
        }
    }
}

Engine::Camera* Engine::World::getCameraPtr(){
    return &this->world_camera;
}
