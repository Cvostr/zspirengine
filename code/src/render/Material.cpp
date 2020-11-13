#include "../../headers/render/Material.hpp"

MaterialShaderProperty::MaterialShaderProperty(){
    type = MATSHPROP_TYPE_NONE;
    start_offset = 0;
}

MaterialShaderPropertyConf::MaterialShaderPropertyConf(){
    type = MATSHPROP_TYPE_NONE;
}

//Integer stuff
IntegerMaterialShaderProperty::IntegerMaterialShaderProperty(){
    type = MATSHPROP_TYPE_INTEGER;
}
IntegerMtShPropConf::IntegerMtShPropConf(){
    type = MATSHPROP_TYPE_INTEGER;
    value = 0;
}
//Float stuff
FloatMaterialShaderProperty::FloatMaterialShaderProperty(){
    type = MATSHPROP_TYPE_FLOAT;
}
FloatMtShPropConf::FloatMtShPropConf(){
    type = MATSHPROP_TYPE_FLOAT;
    value = 0.0f;
}
//Float3 stuff
Float3MaterialShaderProperty::Float3MaterialShaderProperty(){
    type = MATSHPROP_TYPE_FVEC3;
}
Float3MtShPropConf::Float3MtShPropConf(){
    type = MATSHPROP_TYPE_FVEC3;
    value = ZSVECTOR3(0.0f, 0.0f, 0.0f);
}
//Float2 stuff
Float2MaterialShaderProperty::Float2MaterialShaderProperty(){
    type = MATSHPROP_TYPE_FVEC2;
}
Float2MtShPropConf::Float2MtShPropConf(){
    type = MATSHPROP_TYPE_FVEC2;
    value = ZSVECTOR2(0.0f, 0.0f);
}
//Int2 stuff
Int2MaterialShaderProperty::Int2MaterialShaderProperty(){
    type = MATSHPROP_TYPE_IVEC2;
}
Int2MtShPropConf::Int2MtShPropConf(){
    type = MATSHPROP_TYPE_IVEC2;
    value[0] = 0;
    value[1] = 0;
}
//Color stuff
ColorMaterialShaderProperty::ColorMaterialShaderProperty(){
    type = MATSHPROP_TYPE_COLOR;
}
ColorMtShPropConf::ColorMtShPropConf(){
    type = MATSHPROP_TYPE_COLOR;
}
//Textures3D stuff
Texture3MaterialShaderProperty::Texture3MaterialShaderProperty(){
    type = MATSHPROP_TYPE_TEXTURE3;
    this->slotToBind = 0;
}
//Configuration class
Texture3MtShPropConf::Texture3MtShPropConf(){
    this->type = MATSHPROP_TYPE_TEXTURE3;
    //Allocate 3D texture
    texture3D = Engine::allocTexture3D();

    for(int i = 0; i < 6; i ++){
        this->texture_str[i] = "@none";
    }
}
//Textures stuff
TextureMaterialShaderProperty::TextureMaterialShaderProperty(){
    type = MATSHPROP_TYPE_TEXTURE;

    this->slotToBind = 0;
}
//Configuration class
TextureMtShPropConf::TextureMtShPropConf(){
    this->type = MATSHPROP_TYPE_TEXTURE;

    this->path = "@none";
    this->texture = nullptr;
}

MaterialShaderProperty* MtShProps::allocateProperty(int type){
    MaterialShaderProperty* _ptr = nullptr;
    switch (type) {
        case MATSHPROP_TYPE_TEXTURE:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new TextureMaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_INTEGER:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new IntegerMaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FLOAT:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new FloatMaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FVEC3:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new Float3MaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FVEC2:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new Float2MaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_COLOR:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new ColorMaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_TEXTURE3:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new Texture3MaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_IVEC2:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new Int2MaterialShaderProperty); //Allocation of transform in heap
            break;
        }

    }
    return _ptr;
}
MaterialShaderPropertyConf* MtShProps::allocatePropertyConf(int type){
    MaterialShaderPropertyConf* _ptr = nullptr;
    switch (type) {
        case MATSHPROP_TYPE_TEXTURE:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new TextureMtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_INTEGER:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new IntegerMtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FLOAT:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new FloatMtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FVEC3:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new Float3MtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FVEC2:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new Float2MtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_COLOR:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new ColorMtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_TEXTURE3:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new Texture3MtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_IVEC2:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new Int2MtShPropConf); //Allocation of transform in heap
            break;
        }
    }
    return _ptr;
}


void Material::clear(){
    for(unsigned int prop_i = 0; prop_i < this->confs.size(); prop_i ++){
        delete this->confs[prop_i];
    }
    confs.clear(); //Resize array to 0
}

void Material::setPropertyGroup(MtShaderPropertiesGroup* group_ptr){
    this->clear(); //clear all confs, first
    //Iterate over all properties in group
    for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){
        //Obtain pointer to property in group
        MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
        //Add PropertyConf with the same type
        this->addPropertyConf(prop_ptr->type);
    }
    //store pointer of picked group
    this->group_ptr = group_ptr;
    //store string id of picked group
    this->group_str = group_ptr->str_path;
}

Material::Material(){
    setPropertyGroup(MtShProps::getDefaultMtShGroup());
}

Material::Material(std::string shader_group_str){
    setPropertyGroup(MtShProps::getMtShaderPropertyGroup(shader_group_str));
}

Material::Material(MtShaderPropertiesGroup* _group_ptr){
    setPropertyGroup(_group_ptr);
}

Material::~Material(){

}

MaterialShaderPropertyConf* Material::addPropertyConf(int type){
    //Allocate new property
    MaterialShaderPropertyConf* newprop_ptr =
            static_cast<MaterialShaderPropertyConf*>(MtShProps::allocatePropertyConf(type));
    //Push pointer to vector
    this->confs.push_back(newprop_ptr);

    return confs[confs.size() - 1];
}

