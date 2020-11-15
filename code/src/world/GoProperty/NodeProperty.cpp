#include "../../../headers/world/go_properties.h"

Engine::NodeProperty::NodeProperty() : scale (1.f)
{
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE;
}

void Engine::NodeProperty::copyTo(IGameObjectComponent* dest) {
    if (dest->type != this->type) return; //if it isn't Node property

    //Do base things
    IGameObjectComponent::copyTo(dest);
    NodeProperty* _dest = static_cast<NodeProperty*>(dest);

    _dest->transform_mat = this->transform_mat;
    _dest->node_label = this->node_label;
}

void Engine::NodeProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    readString(node_label, data, offset);
    //Now read node matrix
    for (unsigned int m_i = 0; m_i < 4; m_i++) {
        for (unsigned int m_j = 0; m_j < 4; m_j++) {
            float* m_v = &transform_mat.m[m_i][m_j];
            readBinaryValue(m_v, data + offset, offset);
        }
    }
}

void Engine::NodeProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    //Write node name
    *stream << node_label << '\0';
    //Write node transform matrix
    for (unsigned int m_i = 0; m_i < 4; m_i++) {
        for (unsigned int m_j = 0; m_j < 4; m_j++) {
            float m_v = transform_mat.m[m_i][m_j];
            stream->writeBinaryValue(&m_v);
        }
    }
}