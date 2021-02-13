#include "../../headers/render/Material.hpp"
#include "../../headers/vulkan/VKMaterial.hpp"
#include <fstream>
#include <iostream>
#include <GL/glew.h>
#include "../../headers/game.h"
#include "../../headers/engine/Resources.hpp"

Material* default3dmat;
Material* defaultTerrainMat;
static std::vector<MaterialTemplate*> MatGroups;
//Hack to support resources
extern ZSGAME_DATA* game_data;

extern ZSpireEngine* engine_ptr;

MaterialShaderProperty* MaterialTemplate::addProperty(int type){
    //Allocate property in heap
    MaterialShaderProperty* newprop_ptr = MtShProps::allocateProperty(type);
    //Push new material
    this->properties.push_back(newprop_ptr);
    //Return new material pointer from vector
    return properties[properties.size() - 1];
}

void MaterialTemplate::loadFromFile(const char* fpath){
    std::ifstream mat_shader_group;
    mat_shader_group.open(fpath);

    mat_shader_group.close();
}
MaterialTemplate::MaterialTemplate(Engine::Shader* shader, unsigned int UB_ConnectID, unsigned int UB_SIZE) :
    mShader(shader),
    mAcceptShadows(false)
{
    
    properties.resize(0);

    if(UB_SIZE == 0) return;
    //Generate uniform buffer
    mUniformBuffer = Engine::allocUniformBuffer();
    mUniformBuffer->init(UB_ConnectID, UB_SIZE);

}

void MaterialTemplate::setUB_Data(unsigned int offset, unsigned int size, void* data){
    mUniformBuffer->writeDataBuffered(offset, size, data);
}

MaterialTemplate* MtShProps::genDefaultMtShGroup(Engine::Shader* shader3d, Engine::Shader* skybox,
                                                        Engine::Shader* heightmap,
                                                        Engine::Shader* water){

    MaterialTemplate* default_group = allocMaterialTemplate(shader3d, 50, 48);
    {
        default_group->mAcceptShadows = true;
        default_group->str_path = "@default";
        default_group->Label = "Default 3D";

        MaterialShaderProperty* diff_color_prop = (default_group->addProperty(MATSHPROP_TYPE_COLOR));
        diff_color_prop->prop_caption = "Color"; //Set caption in Inspector
        diff_color_prop->prop_identifier = "c_diffuse"; //Identifier to save
        diff_color_prop->start_offset = 0;

        TextureMaterialShaderProperty* diff_texture_prop =
                static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        diff_texture_prop->slotToBind = 0;
        diff_texture_prop->prop_caption = "Diffuse"; //Set caption in Inspector
        diff_texture_prop->prop_identifier = "t_diffuse"; //Identifier to save
        diff_texture_prop->start_offset = 12;

        TextureMaterialShaderProperty* normal_texture_prop =
                static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        normal_texture_prop->slotToBind = 1;
        normal_texture_prop->prop_caption = "Normal";
        normal_texture_prop->prop_identifier = "t_normal"; //Identifier to save
        normal_texture_prop->start_offset = 16;

        TextureMaterialShaderProperty* specular_texture_prop =
                static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        specular_texture_prop->slotToBind = 2;
        specular_texture_prop->prop_caption = "Specular";
        specular_texture_prop->prop_identifier = "t_specular"; //Identifier to save
        specular_texture_prop->start_offset = 20;


        TextureMaterialShaderProperty* height_texture_prop =
                static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        height_texture_prop->slotToBind = 3;
        height_texture_prop->prop_caption = "Height";
        height_texture_prop->prop_identifier = "t_height"; //Identifier to save
        height_texture_prop->start_offset = 24;

        TextureMaterialShaderProperty* ao_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        ao_texture_prop->slotToBind = 4;
        ao_texture_prop->prop_caption = "Amb Occlusion";
        ao_texture_prop->prop_identifier = "t_occlusion"; //Identifier to save
        ao_texture_prop->start_offset = 28;

        MaterialShaderProperty* shininess_factor_prop = default_group->addProperty(MATSHPROP_TYPE_FLOAT);
        shininess_factor_prop->prop_caption = "Shininess";
        shininess_factor_prop->prop_identifier = "f_shininess"; //Identifier to save
        shininess_factor_prop->start_offset = 32;

        MaterialShaderProperty* uv_factor_prop = default_group->addProperty(MATSHPROP_TYPE_IVEC2);
        uv_factor_prop->prop_caption = "UV repeat";
        uv_factor_prop->prop_identifier = "i_uv_repeat"; //Identifier to save
        uv_factor_prop->start_offset = 36;

        ((VKMaterialTemplate*)default_group)->CreatePipeline();
        MtShProps::addMtShaderPropertyGroup(default_group);
    }
    {

        //Water
        MaterialTemplate* water_group = allocMaterialTemplate(water, 51, 32);
        water_group->mAcceptShadows = true;
        water_group->str_path = "@basewater";
        water_group->Label = "Water";

        TextureMaterialShaderProperty* reflection_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(water_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        reflection_texture_prop->slotToBind = 0;
        reflection_texture_prop->prop_caption = "Reflection"; //Set caption in Inspector
        reflection_texture_prop->prop_identifier = "t_reflection"; //Identifier to save
        reflection_texture_prop->start_offset = 0;


        TextureMaterialShaderProperty* distortion_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(water_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        distortion_texture_prop->slotToBind = 1;
        distortion_texture_prop->prop_caption = "Distortion"; //Set caption in Inspector
        distortion_texture_prop->prop_identifier = "t_distortion"; //Identifier to save
        distortion_texture_prop->start_offset = 4;

        TextureMaterialShaderProperty* normal_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(water_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        normal_texture_prop->slotToBind = 2;
        normal_texture_prop->prop_caption = "Normal"; //Set caption in Inspector
        normal_texture_prop->prop_identifier = "t_normal"; //Identifier to save
        normal_texture_prop->start_offset = 8;

        MaterialShaderProperty* distortion_factor_prop = (water_group->addProperty(MATSHPROP_TYPE_FLOAT));
        distortion_factor_prop->prop_caption = "Distort factor";
        distortion_factor_prop->prop_identifier = "f_distfactor"; //Identifier to save
        distortion_factor_prop->start_offset = 12;

        MaterialShaderProperty* diff_color_prop = (water_group->addProperty(MATSHPROP_TYPE_COLOR));
        diff_color_prop->prop_caption = "Color"; //Set caption in Inspector
        diff_color_prop->prop_identifier = "c_diffuse"; //Identifier to save
        diff_color_prop->start_offset = 16;

        MtShProps::addMtShaderPropertyGroup(water_group);
    }

//Default skybox material
    MaterialTemplate* default_sky_group = allocMaterialTemplate(skybox, 0, 0);
    default_sky_group->str_path = "@skybox";
    default_sky_group->Label = "Default Skybox";
    Texture3MaterialShaderProperty* sky_texture =
            static_cast<Texture3MaterialShaderProperty*>(default_sky_group->addProperty(MATSHPROP_TYPE_TEXTURE3));
    sky_texture->slotToBind = 0;
    sky_texture->prop_caption = "Sky";
    sky_texture->prop_identifier = "skytexture3"; //Identifier to save

    MtShProps::addMtShaderPropertyGroup(default_sky_group);

//Default terrain material
    MaterialTemplate* default_heightmap_group = allocMaterialTemplate(heightmap, 0, 0);
    default_heightmap_group->str_path = "@heightmap";
    default_heightmap_group->Label = "Default Heightmap";
    default_heightmap_group->mAcceptShadows = true;

    MtShProps::addMtShaderPropertyGroup(default_heightmap_group);

    //Create default base material
    default3dmat = allocMaterial(default_group);
    default3dmat->file_path = "@default";

    defaultTerrainMat = allocMaterial(default_heightmap_group);
    defaultTerrainMat->file_path = "@defaultHeightmap";

    return default_group;
}
MaterialTemplate* MtShProps::getDefaultMtShGroup(){
    return MatGroups[0];
}

void MtShProps::addMtShaderPropertyGroup(MaterialTemplate* group){
    
    //Check if this property already added
    if(getMtShaderPropertyGroup(group->str_path) == nullptr)
        MatGroups.push_back(group);
}
MaterialTemplate* MtShProps::getMtShaderPropertyGroup(std::string group_name){
    for(unsigned int group_i = 0; group_i < MatGroups.size(); group_i ++){
        MaterialTemplate* TemplatePtr = MatGroups[group_i];
        if(TemplatePtr->str_path.compare(group_name) == false)
            return TemplatePtr;
    }
    return nullptr;
}

MaterialTemplate* MtShProps::getMtShaderPropertyGroupByLabel(std::string group_label){
    for(unsigned int group_i = 0; group_i < MatGroups.size(); group_i ++){
        MaterialTemplate* TemplatePtr = MatGroups[group_i];
        if(TemplatePtr->Label.compare(group_label) == false)
            return TemplatePtr;
    }
    return nullptr;
}

unsigned int MtShProps::getMaterialShaderPropertyAmount(){
    return static_cast<unsigned int>(MatGroups.size());
}
MaterialTemplate* MtShProps::getMaterialTemplateByIndex(unsigned int index){
    return MatGroups[index];
}

void MtShProps::clearMtShaderGroups(){
   MatGroups.clear();
}
