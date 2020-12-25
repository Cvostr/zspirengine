#include "../../headers/render/Material.hpp"
#include <iostream>
#include "../../headers/misc/misc.h"
#include "../../headers/game.h"

extern ZSpireEngine* engine_ptr;
//Hack to support resources
extern ZSGAME_DATA* game_data;

MaterialShaderProperty::MaterialShaderProperty(){
    type = MATSHPROP_TYPE_NONE;
    start_offset = 0;
}

MaterialShaderPropertyConf::MaterialShaderPropertyConf(){
    type = MATSHPROP_TYPE_NONE;
}

IntegerMtShPropConf::IntegerMtShPropConf(){
    type = MATSHPROP_TYPE_INTEGER;
    value = 0;
}

FloatMtShPropConf::FloatMtShPropConf(){
    type = MATSHPROP_TYPE_FLOAT;
    value = 0.0f;
}

Float3MtShPropConf::Float3MtShPropConf(){
    type = MATSHPROP_TYPE_FVEC3;
    value = Vec3(0.0f, 0.0f, 0.0f);
}

Float2MtShPropConf::Float2MtShPropConf(){
    type = MATSHPROP_TYPE_FVEC2;
    value = Vec2(0.0f, 0.0f);
}

Int2MtShPropConf::Int2MtShPropConf(){
    type = MATSHPROP_TYPE_IVEC2;
    value[0] = 0;
    value[1] = 0;
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
        
        case MATSHPROP_TYPE_TEXTURE3:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new Texture3MaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        default:{ //If type is transfrom
            _ptr = new MaterialShaderProperty; //Allocation of transform in heap
            _ptr->type = (MATSHPROP_TYPE)type;
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
    if (engine_ptr->engine_info->graphicsApi == VULKAN && group_ptr->render_shader->mCreated) {

        Engine::ZsVkPipelineConf Conf;
        Conf.LayoutInfo.DescrSetLayout->pushUniformBuffer((Engine::_vk_UniformBuffer*)game_data->pipeline->GetTransformUniformBuffer(), VK_SHADER_STAGE_VERTEX_BIT);
        //Conf.DescrSetLayout->pushUniformBuffer((Engine::_vk_UniformBuffer*)this->lightsBuffer, VK_SHADER_STAGE_FRAGMENT_BIT);
       // Conf.DescrSetLayout->pushUniformBuffer((Engine::_vk_UniformBuffer*)this->shadowBuffer, VK_SHADER_STAGE_ALL_GRAPHICS);
        Conf.LayoutInfo.DescrSetLayoutSampler->pushImageSampler(0);
        Conf.LayoutInfo.DescrSetLayoutSampler->pushImageSampler(1);
        Conf.hasDepth = true;
        Conf.cullFace = true;
        Conf.LayoutInfo.AddPushConstant(64, VK_SHADER_STAGE_VERTEX_BIT);
        
        this->Pipeline = new Engine::ZSVulkanPipeline;
        Pipeline->Create((Engine::_vk_Shader*)group_ptr->render_shader, game_data->vk_main->mMaterialsRenderPass, Conf);
    }
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

void Material::loadFromBuffer(char* buffer, unsigned int size) {
    //Define and open file stream
    unsigned int position = 0;

    char test_header[13];
    //Read header
    memcpy(test_header, &buffer[position], 12);
    test_header[12] = '\0'; //Terminate string

    if (strcmp(test_header, "ZSP_MATERIAL") == 1) //If it isn't zspire material
        return; //Go out, we have nothing to do

    position += 13;

    while (position <= size) { //While file not finished reading
        char prefix[7];
        prefix[6] = '\0';
        memcpy(prefix, &buffer[position], 6);

        if (strcmp(prefix, "_GROUP") && strcmp(prefix, "_ENTRY"))
            position++;

        if (strcmp(prefix, "_GROUP") == 0) { //if it is game object
            position += 7;
            std::string group_name;
            //Read shader group name
            readString(group_name, buffer, position);

            setPropertyGroup(MtShProps::getMtShaderPropertyGroup(group_name));
        }

        if (strcmp(prefix, "_ENTRY") == 0) { //if it is game object
            position += 7;
            std::string prop_identifier;
            readString(prop_identifier, buffer, position);

            for (unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i++) {
                MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
                MaterialShaderPropertyConf* conf_ptr = this->confs[prop_i];
                //check if compare
                if (prop_identifier.compare(prop_ptr->prop_identifier) == 0) {
                    switch (prop_ptr->type) {
                    case MATSHPROP_TYPE_NONE: {
                        break;
                    }
                    case MATSHPROP_TYPE_TEXTURE: {
                        //Cast pointer
                        TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);
                        readString(texture_conf->path, buffer, position);

                        position += 1;
                        break;
                    }

                    case MATSHPROP_TYPE_FLOAT: {
                        //Cast pointer
                        FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);

                        readBinaryValue(&float_conf->value, buffer + position, position);

                        break;
                    }

                    case MATSHPROP_TYPE_INTEGER: {
                        //Cast pointer
                        IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);

                        readBinaryValue(&int_conf->value, &buffer[position], position);
                        break;
                    }

                    case MATSHPROP_TYPE_COLOR: {
                        //Cast pointer
                        ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);
                        //Read color values
                        readBinaryValue(&color_conf->color.r, buffer + position, position);
                        readBinaryValue(&color_conf->color.g, buffer + position, position);
                        readBinaryValue(&color_conf->color.b, buffer + position, position);
                        position += 1;
                        break;
                    }

                    case MATSHPROP_TYPE_FVEC3: {
                        //Cast pointer
                        Float3MtShPropConf* fvec3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);
                        //Read float vector values
                        readBinaryValue(&fvec3_conf->value.X, buffer + position, position);
                        readBinaryValue(&fvec3_conf->value.Y, buffer + position, position);
                        readBinaryValue(&fvec3_conf->value.Z, buffer + position, position);
                        position += 1;
                        break;
                    }

                    case MATSHPROP_TYPE_FVEC2: {
                        //Cast pointer
                        Float2MtShPropConf* fvec2_conf = static_cast<Float2MtShPropConf*>(conf_ptr);
                        //Read float vector values
                        readBinaryValue(&fvec2_conf->value.X, buffer + position, position);
                        readBinaryValue(&fvec2_conf->value.Y, buffer + position, position);
                        position += 1;
                        break;
                    }

                    case MATSHPROP_TYPE_IVEC2: {
                        //Cast pointer
                        Int2MtShPropConf* ivec2_conf = static_cast<Int2MtShPropConf*>(conf_ptr);
                        //Read float vector values
                        readBinaryValue(&ivec2_conf->value[0], buffer + position, position);
                        readBinaryValue(&ivec2_conf->value[1], buffer + position, position);

                        position += 1;
                        break;
                    }

                    case MATSHPROP_TYPE_TEXTURE3: {
                        //Cast pointer
                        Texture3MtShPropConf* texture3_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);

                        for (int i = 0; i < 6; i++) {
                            readString(texture3_conf->texture_str[i], buffer, position);
                        }

                        break;
                    }
                    }
                }
            }
        }
    }
}

void Material::loadFromFile(std::string fpath) {
    std::cout << "Loading Material " << fpath << std::endl;

    this->file_path = fpath;

    std::ifstream mat_stream;
    //Open stream
    mat_stream.open(fpath, std::ifstream::in | std::ifstream::ate);

    if (!mat_stream) { //File isn't opened
        std::cout << "Error opening file " << fpath;
        return;
    }

    //Allocate space for file
    unsigned int fileSize = static_cast<unsigned int>(mat_stream.tellg());
    char* data = new char[fileSize];
    mat_stream.seekg(0);
    //Read file
    mat_stream.read(data, fileSize);
    //Parse it from buffer
    loadFromBuffer(data, fileSize);
    //Free buffer
    delete[] data;

    mat_stream.close(); //close material stream
}