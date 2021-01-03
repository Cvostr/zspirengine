#include "../../../headers/world/ObjectsComponents/LightSourceComponent.hpp"

Engine::LightsourceProperty::LightsourceProperty() : light_type(LIGHTSOURCE_TYPE::LIGHTSOURCE_TYPE_DIRECTIONAL),
                                                     intensity(1.f),
                                                     range(10.0f),
                                                     spot_angle(12.5f)
{
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_LIGHTSOURCE;
}


void Engine::LightsourceProperty::copyTo(Engine::IGameObjectComponent* dest) {
    if (dest->type != this->type) return; //if it isn't Lightsource, then exit

    //Do base things
    IGameObjectComponent::copyTo(dest);

    LightsourceProperty* _dest = static_cast<LightsourceProperty*>(dest);
    _dest->color = color;
    _dest->intensity = intensity;
    _dest->range = range;
    _dest->light_type = light_type;
}

void Engine::LightsourceProperty::onPreRender(Engine::Renderer* pipeline) {
    Engine::TransformProperty* transform_prop = go_link.updLinkPtr()->getPropertyPtr<Engine::TransformProperty>();

    pipeline->addLight(this); //put light pointer to vector

    //check, if light transformation changed
    if ((this->last_pos != transform_prop->abs_translation || this->last_rot != transform_prop->rotation)) {
        Vec3* rot_vec_ptr = &transform_prop->rotation;
        this->direction = _getDirection(rot_vec_ptr->X, rot_vec_ptr->Y, rot_vec_ptr->Z);
        //store new transform values
        this->last_pos = transform_prop->abs_translation;
        this->last_rot = transform_prop->abs_rotation;
    }
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
    readBinaryValue(&cl_r, data + offset, offset);
    readBinaryValue(&cl_g, data + offset, offset);
    readBinaryValue(&cl_b, data + offset, offset);

    color = RGBAColor(cl_r, cl_g, cl_b);
}

void Engine::LightsourceProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    stream->writeBinaryValue(&light_type);
    stream->writeBinaryValue(&intensity);
    stream->writeBinaryValue(&range);
    stream->writeBinaryValue(&spot_angle);

    stream->writeBinaryValue(&color.r);
    stream->writeBinaryValue(&color.g);
    stream->writeBinaryValue(&color.b);
}

void Engine::LightsourceProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    mgr->RegisterObjectType(LIGHTSOURCE_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

    mgr->RegisterObjectProperty(LIGHTSOURCE_PROP_TYPE_NAME, "float intensity", offsetof(LightsourceProperty, intensity));
    mgr->RegisterObjectProperty(LIGHTSOURCE_PROP_TYPE_NAME, "float range", offsetof(LightsourceProperty, range));
    mgr->RegisterObjectProperty(LIGHTSOURCE_PROP_TYPE_NAME, "float spot_angle", offsetof(LightsourceProperty, spot_angle));
    mgr->RegisterObjectProperty(LIGHTSOURCE_PROP_TYPE_NAME, std::string(RGBACOLOR_TYPE_NAME) + " color", offsetof(LightsourceProperty, color));
}