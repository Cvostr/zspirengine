#include "../../../headers/world/go_properties.h"

Engine::TransformProperty::TransformProperty() {
    this->type = PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM;
    this->scale = ZSVECTOR3(1.f, 1.f, 1.f);
}

void Engine::TransformProperty::updateMatrix() {
    //Variables to store
    ZSVECTOR3 p_translation = ZSVECTOR3(0, 0, 0);
    ZSVECTOR3 p_scale = ZSVECTOR3(1, 1, 1);
    ZSVECTOR3 p_rotation = ZSVECTOR3(0, 0, 0);

    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property
    if (ptr != nullptr && ptr->hasParent) { //if object exist and dependent
        //Get parent's transform property
        TransformProperty* property = static_cast<TransformProperty*>(ptr->parent.updLinkPtr()->getTransformProperty());
        //Calculate parent transform offset
        property->getAbsoluteParentTransform(p_translation, p_scale, p_rotation);

    }

    abs_translation = p_translation + this->translation;
    abs_scale = p_scale * this->scale;
    abs_rotation = this->rotation + p_rotation;

    //Calculate translation matrix
    ZSMATRIX4x4 translation_mat = getTranslationMat(abs_translation);
    //Calculate scale matrix
    ZSMATRIX4x4 scale_mat = getScaleMat(abs_scale);
    //Calculate rotation matrix
    ZSMATRIX4x4 rotation_mat1 = getIdentity();
    getAbsoluteRotationMatrix(rotation_mat1);
    ZSMATRIX4x4 rotation_mat = getRotationMat(abs_rotation);
    //S * R * T
    this->transform_mat = scale_mat * rotation_mat * rotation_mat1 * translation_mat;

}
void Engine::TransformProperty::onValueChanged() {
    if ((this->go_link.updLinkPtr()) == nullptr) return;
    //Update transform matrices
    updateMatrix();
    //Get pointer to Base physics component
    PhysicalProperty* phys = static_cast<PhysicalProperty*>(go_link.updLinkPtr()->getPhysicalProperty());

    if (this->go_link.updLinkPtr()->isRigidbody()) {
        if (!phys->created) return;
        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(btScalar(abs_translation.X + phys->transform_offset.X),
            btScalar(abs_translation.Y + phys->transform_offset.Y),
            btScalar(abs_translation.Z + phys->transform_offset.Z)));

        startTransform.setRotation(btQuaternion(abs_rotation.X, abs_rotation.Y, abs_rotation.Z));


        phys->rigidBody->setWorldTransform(startTransform);
        phys->rigidBody->getMotionState()->setWorldTransform(startTransform);

        phys->shape->setLocalScaling(btVector3(btScalar(abs_scale.X),
            btScalar(abs_scale.Y),
            btScalar(abs_scale.Z)));

    }
}
void Engine::TransformProperty::getAbsoluteParentTransform(ZSVECTOR3& t, ZSVECTOR3& s, ZSVECTOR3& r) {
    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property

    t = t + this->translation;
    s = s * this->scale;
    r = r + this->rotation;

    if (ptr->hasParent) {
        GameObject* parent_p = ptr->parent.ptr;
        TransformProperty* property = static_cast<TransformProperty*>(parent_p->getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM));
        property->getAbsoluteParentTransform(t, s, r);
    }
}

void Engine::TransformProperty::getAbsoluteRotationMatrix(ZSMATRIX4x4& m) {
    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property

    if (ptr == nullptr) return;

    if (ptr->hasParent == true) {
        GameObject* parent_p = ptr->parent.ptr;
        TransformProperty* property = static_cast<TransformProperty*>(parent_p->getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM));

        ZSMATRIX4x4 rotation_mat1 = getRotationMat(property->rotation, ptr->getTransformProperty()->translation);
        m = rotation_mat1 * m;
        property->getAbsoluteRotationMatrix(m);
    }
}

void Engine::TransformProperty::setTranslation(ZSVECTOR3& new_translation) {
    this->translation = new_translation;
    this->onValueChanged();
}
void Engine::TransformProperty::setScale(ZSVECTOR3& new_scale) {
    this->scale = new_scale;
    this->onValueChanged();
}
void Engine::TransformProperty::setRotation(ZSVECTOR3& new_rotation) {
    float* ptr = &new_rotation.X;
    this->rotation = new_rotation;
    this->onValueChanged();
}

void Engine::TransformProperty::copyTo(GameObjectProperty* dest) {
    if (dest->type != this->type) return; //if it isn't transform
    //cast pointer and send data
    TransformProperty* _dest = static_cast<TransformProperty*>(dest);
    _dest->translation = translation;
    _dest->scale = scale;
    _dest->rotation = rotation;
    _dest->transform_mat = transform_mat;
}

void Engine::TransformProperty::onPreRender(RenderPipeline* pipeline) {
    this->updateMatrix();
    //Send transform matrix to transform buffer
    pipeline->transformBuffer->bind();
    pipeline->transformBuffer->writeData(sizeof(ZSMATRIX4x4) * 2, sizeof(ZSMATRIX4x4), &transform_mat);
}

void Engine::TransformProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {
    int result = 0;
    result = mgr->RegisterObjectType(TRANSFORM_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);
    assert(result >= 0);

    result = mgr->RegisterObjectProperty(TRANSFORM_PROP_TYPE_NAME, "Vec3 translation", offsetof(TransformProperty, translation));
    result = mgr->RegisterObjectProperty(TRANSFORM_PROP_TYPE_NAME, "Vec3 scale", offsetof(TransformProperty, scale));
    result = mgr->RegisterObjectProperty(TRANSFORM_PROP_TYPE_NAME, "Vec3 rotation", offsetof(TransformProperty, rotation));

    result = mgr->RegisterObjectMethod(TRANSFORM_PROP_TYPE_NAME, "void setTranslation(Vec3@)", asMETHOD(TransformProperty, setTranslation), asCALL_THISCALL);
    result = mgr->RegisterObjectMethod(TRANSFORM_PROP_TYPE_NAME, "void setScale(Vec3@)", asMETHOD(TransformProperty, setScale), asCALL_THISCALL);
    result = mgr->RegisterObjectMethod(TRANSFORM_PROP_TYPE_NAME, "void setRotation(Vec3@)", asMETHOD(TransformProperty, setRotation), asCALL_THISCALL);
    assert(result >=0 );
}

void Engine::TransformProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 0;
    offset += 1; //Skip space
    //Read Translation vector
    memcpy(&translation.X, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&translation.Y, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&translation.Z, data + offset, sizeof(float));
    offset += sizeof(float);
    //Read Scale vector
    memcpy(&scale.X, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&scale.Y, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&scale.Z, data + offset, sizeof(float));
    offset += sizeof(float);
    //Read Rotation vector
    memcpy(&rotation.X, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&rotation.Y, data + offset, sizeof(float));
    offset += sizeof(float);
    memcpy(&rotation.Z, data + offset, sizeof(float));
    offset += sizeof(float);
    //After everything is loaded, update matrices
    updateMatrix(); 
}