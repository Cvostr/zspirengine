#include "../../../headers/world/go_properties.h"

void Engine::LightsourceProperty::copyTo(Engine::GameObjectProperty* dest) {
    if (dest->type != this->type) return; //if it isn't Lightsource, then exit

    //Do base things
    GameObjectProperty::copyTo(dest);

    LightsourceProperty* _dest = static_cast<LightsourceProperty*>(dest);
    _dest->color = color;
    _dest->intensity = intensity;
    _dest->range = range;
    _dest->light_type = light_type;
}

void Engine::LightsourceProperty::onPreRender(Engine::RenderPipeline* pipeline) {
    Engine::TransformProperty* transform_prop = go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();

    pipeline->addLight(this); //put light pointer to vector

    //check, if light transformation changed
    if ((this->last_pos != transform_prop->abs_translation || this->last_rot != transform_prop->rotation)) {
        ZSVECTOR3* rot_vec_ptr = &transform_prop->rotation;
        this->direction = _getDirection(rot_vec_ptr->X, rot_vec_ptr->Y, rot_vec_ptr->Z);
        //store new transform values
        this->last_pos = transform_prop->abs_translation;
        this->last_rot = transform_prop->abs_rotation;
    }
}

Engine::LightsourceProperty::LightsourceProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_LIGHTSOURCE;
    active = true;
    light_type = LIGHTSOURCE_TYPE::LIGHTSOURCE_TYPE_DIRECTIONAL; //base type is directional

    intensity = 1.0f; //base light instensity is 1
    range = 10.0f;
    spot_angle = 12.5f;
}

void Engine::LightsourceProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    //Read Type of Light Source
    readBinaryValue<LIGHTSOURCE_TYPE>(&light_type, data + offset, offset);
    //Read Intensity of source
    readBinaryValue<float>(&intensity, data + offset, offset);
    //Read Range of source
    readBinaryValue<float>(&range, data + offset, offset);
    readBinaryValue<float>(&spot_angle, data + offset, offset);

    int cl_r;
    int cl_g;
    int cl_b;
    //Read light color
    memcpy(&cl_r, data + offset, sizeof(int));
    offset += sizeof(float);
    memcpy(&cl_g, data + offset, sizeof(int));
    offset += sizeof(float);
    memcpy(&cl_b, data + offset, sizeof(int));
    offset += sizeof(float);

    color = ZSRGBCOLOR(cl_r, cl_g, cl_b);
}

void Engine::LightsourceProperty::savePropertyToStream(std::ofstream* stream, GameObject* obj) {
    stream->write(reinterpret_cast<char*>(&light_type), sizeof(Engine::LIGHTSOURCE_TYPE));
    stream->write(reinterpret_cast<char*>(&intensity), sizeof(float));
    stream->write(reinterpret_cast<char*>(&range), sizeof(float));
    stream->write(reinterpret_cast<char*>(&spot_angle), sizeof(float));

    stream->write(reinterpret_cast<char*>(&color.r), sizeof(int));
    stream->write(reinterpret_cast<char*>(&color.g), sizeof(int));
    stream->write(reinterpret_cast<char*>(&color.b), sizeof(int));
}

void Engine::LightsourceProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    mgr->RegisterObjectType(LIGHTSOURCE_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

    mgr->RegisterObjectProperty(LIGHTSOURCE_PROP_TYPE_NAME, "float intensity", offsetof(LightsourceProperty, intensity));
    mgr->RegisterObjectProperty(LIGHTSOURCE_PROP_TYPE_NAME, "float range", offsetof(LightsourceProperty, range));
    mgr->RegisterObjectProperty(LIGHTSOURCE_PROP_TYPE_NAME, "float spot_angle", offsetof(LightsourceProperty, spot_angle));
    mgr->RegisterObjectProperty(LIGHTSOURCE_PROP_TYPE_NAME, "rgbColor color", offsetof(LightsourceProperty, color));
}