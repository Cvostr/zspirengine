#include "../../../headers/world/go_properties.h"

Engine::NodeProperty::NodeProperty() {
    type = PROPERTY_TYPE::GO_PROPERTY_TYPE_NODE;

    scale = ZSVECTOR3(1.f, 1.f, 1.f);
    translation = ZSVECTOR3(0.f, 0.f, 0.f);
    rotation = ZSQUATERNION(0.f, 0.f, 0.f, 0.f);
}

void Engine::NodeProperty::copyTo(GameObjectProperty* dest) {
    if (dest->type != this->type) return; //if it isn't Node property

    //Do base things
    GameObjectProperty::copyTo(dest);
    NodeProperty* _dest = static_cast<NodeProperty*>(dest);

    _dest->transform_mat = this->transform_mat;
    _dest->node_label = this->node_label;
}

void Engine::NodeProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 1;
    while (data[offset] != ' ' && data[offset] != '\n') {
        node_label += data[offset];
        offset++;
    }
    //Skip 1 byte
    offset++;
    //Now read node matrix
    for (unsigned int m_i = 0; m_i < 4; m_i++) {
        for (unsigned int m_j = 0; m_j < 4; m_j++) {
            float* m_v = &transform_mat.m[m_i][m_j];
            memcpy(m_v, data + offset, sizeof(float));
            offset += sizeof(float);
        }
    }
}
