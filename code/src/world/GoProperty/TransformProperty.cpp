#include "../../../headers/world/World.hpp"
#include "../../../headers/world/ObjectsComponents/PhysicalComponent.hpp"


Engine::TransformProperty::TransformProperty() : scale(1.f)
{
    this->type = PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM;
}

void Engine::TransformProperty::updateMatrix() {
    //Variables to store
    Vec3 p_translation = Vec3(0, 0, 0);
    Vec3 p_scale = Vec3(1, 1, 1);
    Vec3 p_rotation = Vec3(0, 0, 0);

    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property
    if (ptr != nullptr && ptr->hasParent) { //if object exist and dependent
        //Get parent's transform property
        TransformProperty* property = static_cast<TransformProperty*>(ptr->mParent.updLinkPtr()->getTransformProperty());
        //Calculate parent transform offset
        property->getAbsoluteParentTransform(p_translation, p_scale, p_rotation);

    }

    abs_translation = p_translation + this->translation;
    abs_translation = abs_translation ;

    abs_scale = p_scale * this->scale;
    abs_rotation = this->rotation + p_rotation;

    //Calculate translation matrix
    Mat4 translation_mat = getTranslationMat(abs_translation);
    //Calculate scale matrix
    Mat4 scale_mat = getScaleMat(abs_scale) ;
    //Calculate rotation matrix
    Mat4 rotation_mat1 = getIdentity();
    getAbsoluteRotationMatrix(rotation_mat1);
    Mat4 rotation_mat = getRotationMat(abs_rotation);
    //S * R * T
    this->transform_mat = scale_mat * rotation_mat * rotation_mat1 * translation_mat;

    //transform_mat.m[3][2] *= Zcoeff;

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
void Engine::TransformProperty::getAbsoluteParentTransform(Vec3& t, Vec3& s, Vec3& r) {
    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property

    t = t + this->translation;
    s = s * this->scale;
    r = r + this->rotation;

    if (ptr->hasParent) {
        GameObject* parent_p = ptr->mParent.ptr;
        TransformProperty* property = static_cast<TransformProperty*>(parent_p->getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM));
        property->getAbsoluteParentTransform(t, s, r);
    }
}

void Engine::TransformProperty::getAbsoluteRotationMatrix(Mat4& m) {
    GameObject* ptr = go_link.updLinkPtr(); //Pointer to object with this property

    if (ptr == nullptr) return;

    if (ptr->hasParent == true) {
        GameObject* parent_p = ptr->mParent.ptr;
        TransformProperty* property = static_cast<TransformProperty*>(parent_p->getPropertyPtrByType(PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM));

        Mat4 rotation_mat1 = getRotationMat(property->rotation, ptr->getTransformProperty()->translation);
        m = rotation_mat1 * m;
        property->getAbsoluteRotationMatrix(m);
    }
}

void Engine::TransformProperty::setTranslation(const Vec3& new_translation) {
    this->translation = new_translation;
    this->onValueChanged();
}
void Engine::TransformProperty::setScale(const Vec3& new_scale) {
    this->scale = new_scale;
    this->onValueChanged();
}
void Engine::TransformProperty::setRotation(const Vec3& new_rotation) {
    this->rotation = new_rotation;
    this->onValueChanged();
}

void Engine::TransformProperty::copyTo(IGameObjectComponent* dest) {
    if (dest->type != this->type) return; //if it isn't transform
    //cast pointer and send data
    TransformProperty* _dest = static_cast<TransformProperty*>(dest);
    _dest->translation = translation;
    _dest->scale = scale;
    _dest->rotation = rotation;
    _dest->transform_mat = transform_mat;
}

void Engine::TransformProperty::onPreRender(Renderer* pipeline) {
    this->updateMatrix();
}

void Engine::TransformProperty::bindObjectPropertyToAngel(Engine::AGScriptMgr* mgr) {

    mgr->RegisterObjectType(TRANSFORM_PROP_TYPE_NAME, 0, asOBJ_REF | asOBJ_NOCOUNT);

    mgr->RegisterObjectProperty(TRANSFORM_PROP_TYPE_NAME, "Vec3 translation", offsetof(TransformProperty, translation));
    mgr->RegisterObjectProperty(TRANSFORM_PROP_TYPE_NAME, "Vec3 scale", offsetof(TransformProperty, scale));
    mgr->RegisterObjectProperty(TRANSFORM_PROP_TYPE_NAME, "Vec3 rotation", offsetof(TransformProperty, rotation));

    mgr->RegisterObjectMethod(TRANSFORM_PROP_TYPE_NAME, "void setTranslation(const Vec3 &in)", asMETHOD(TransformProperty, setTranslation), asCALL_THISCALL);
    mgr->RegisterObjectMethod(TRANSFORM_PROP_TYPE_NAME, "void setScale(const Vec3 &in)", asMETHOD(TransformProperty, setScale), asCALL_THISCALL);
    mgr->RegisterObjectMethod(TRANSFORM_PROP_TYPE_NAME, "void setRotation(const Vec3 &in)", asMETHOD(TransformProperty, setRotation), asCALL_THISCALL);
}

void Engine::TransformProperty::loadPropertyFromMemory(const char* data, GameObject* obj) {
    unsigned int offset = 0;
    offset += 1; //Skip space
    //Read Translation vector
    readBinaryValue(&translation.X, data + offset, offset);
    readBinaryValue(&translation.Y, data + offset, offset);
    readBinaryValue(&translation.Z, data + offset, offset);
    //Read Scale vector
    readBinaryValue(&scale.X, data + offset, offset);
    readBinaryValue(&scale.Y, data + offset, offset);
    readBinaryValue(&scale.Z, data + offset, offset);
    //Read Rotation vector
    readBinaryValue(&rotation.X, data + offset, offset);
    readBinaryValue(&rotation.Y, data + offset, offset);
    readBinaryValue(&rotation.Z, data + offset, offset);
    //After everything is loaded, update matrices
    updateMatrix(); 
}

void Engine::TransformProperty::savePropertyToStream(ZsStream* stream, GameObject* obj) {
    stream->writeBinaryValue(&translation.X);   //Writing position X
    stream->writeBinaryValue(&translation.Y);   //Writing position Y
    stream->writeBinaryValue(&translation.Z);   //Writing position Z

    stream->writeBinaryValue(&scale.X); //Writing scale X
    stream->writeBinaryValue(&scale.Y); //Writing scale Y
    stream->writeBinaryValue(&scale.Z); //Writing scale Z

    stream->writeBinaryValue(&rotation.X);  //Writing rotation X
    stream->writeBinaryValue(&rotation.Y);  //Writing rotation Y
    stream->writeBinaryValue(&rotation.Z);  //Writing rotation Z
}