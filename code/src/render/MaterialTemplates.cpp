#include <render/Material.hpp>
#include <vulkan/VKMaterial.hpp>
#include <fstream>
#include <iostream>
#include <GL/glew.h>
#include <game.h>
#include <engine/Resources.hpp>

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

void TextureMtShPropConf::SetTexture(std::string TextureName) {
    this->path = TextureName;
    this->texture = game_data->resources->getTextureByLabel(TextureName);
}

void MaterialTemplate::loadFromFile(const char* fpath){
    std::ifstream mat_shader_group;
    mat_shader_group.open(fpath);

    mat_shader_group.close();
}
MaterialTemplate::MaterialTemplate(Engine::Shader* shader, unsigned int UB_SIZE) :
    mShader(shader),
    mAcceptShadows(false)
{
    
    properties.resize(0);

}

MaterialTemplate* MtShProps::genDefaultMtShGroup(Engine::Shader* shader3d, Engine::Shader* skybox,
                                                        Engine::Shader* heightmap,
                                                        Engine::Shader* water, Engine::Shader* particle_shader){

    
    //Default 3D
    MaterialTemplate* default_group = allocMaterialTemplate(shader3d, 48);
    {
        default_group->mAcceptShadows = true;
        default_group->str_path = "@default";
        default_group->Label = "Default 3D";

        MaterialShaderProperty* diff_color_prop = (default_group->addProperty(MATSHPROP_TYPE_COLOR));
        diff_color_prop->mPropCaption = "Color"; //Set caption in Inspector
        diff_color_prop->mPropId = "c_diffuse"; //Identifier to save
        diff_color_prop->start_offset = 0;

        TextureMaterialShaderProperty* diff_texture_prop =
                static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        diff_texture_prop->slotToBind = 0;
        diff_texture_prop->mPropCaption = "Diffuse"; //Set caption in Inspector
        diff_texture_prop->mPropId = "t_diffuse"; //Identifier to save
        diff_texture_prop->start_offset = 12;

        TextureMaterialShaderProperty* normal_texture_prop =
                static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        normal_texture_prop->slotToBind = 1;
        normal_texture_prop->mPropCaption = "Normal";
        normal_texture_prop->mPropId = "t_normal"; //Identifier to save
        normal_texture_prop->start_offset = 16;

        TextureMaterialShaderProperty* specular_texture_prop =
                static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        specular_texture_prop->slotToBind = 2;
        specular_texture_prop->mPropCaption = "Specular";
        specular_texture_prop->mPropId = "t_specular"; //Identifier to save
        specular_texture_prop->start_offset = 20;


        TextureMaterialShaderProperty* height_texture_prop =
                static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        height_texture_prop->slotToBind = 3;
        height_texture_prop->mPropCaption = "Height";
        height_texture_prop->mPropId = "t_height"; //Identifier to save
        height_texture_prop->start_offset = 24;

        TextureMaterialShaderProperty* ao_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        ao_texture_prop->slotToBind = 4;
        ao_texture_prop->mPropCaption = "Amb Occlusion";
        ao_texture_prop->mPropId = "t_occlusion"; //Identifier to save
        ao_texture_prop->start_offset = 28;

        MaterialShaderProperty* shininess_factor_prop = default_group->addProperty(MATSHPROP_TYPE_FLOAT);
        shininess_factor_prop->mPropCaption = "Shininess";
        shininess_factor_prop->mPropId = "f_shininess"; //Identifier to save
        shininess_factor_prop->start_offset = 32;

        MaterialShaderProperty* uv_factor_prop = default_group->addProperty(MATSHPROP_TYPE_IVEC2);
        uv_factor_prop->mPropCaption = "UV repeat";
        uv_factor_prop->mPropId = "i_uv_repeat"; //Identifier to save
        uv_factor_prop->start_offset = 36;

        ((VKMaterialTemplate*)default_group)->CreatePipeline();
        MtShProps::addMtShaderPropertyGroup(default_group);
    }
    //Water
    {

        MaterialTemplate* water_group = allocMaterialTemplate(water, 32);
        water_group->mAcceptShadows = true;
        water_group->str_path = "@basewater";
        water_group->Label = "Water";

        TextureMaterialShaderProperty* reflection_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(water_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        reflection_texture_prop->slotToBind = 0;
        reflection_texture_prop->mPropCaption = "Reflection"; //Set caption in Inspector
        reflection_texture_prop->mPropId = "t_reflection"; //Identifier to save
        reflection_texture_prop->start_offset = 0;


        TextureMaterialShaderProperty* distortion_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(water_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        distortion_texture_prop->slotToBind = 1;
        distortion_texture_prop->mPropCaption = "Distortion"; //Set caption in Inspector
        distortion_texture_prop->mPropId = "t_distortion"; //Identifier to save
        distortion_texture_prop->start_offset = 4;

        TextureMaterialShaderProperty* normal_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(water_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        normal_texture_prop->slotToBind = 2;
        normal_texture_prop->mPropCaption = "Normal"; //Set caption in Inspector
        normal_texture_prop->mPropId = "t_normal"; //Identifier to save
        normal_texture_prop->start_offset = 8;

        MaterialShaderProperty* distortion_factor_prop = (water_group->addProperty(MATSHPROP_TYPE_FLOAT));
        distortion_factor_prop->mPropCaption = "Distort factor";
        distortion_factor_prop->mPropId = "f_distfactor"; //Identifier to save
        distortion_factor_prop->start_offset = 12;

        MaterialShaderProperty* diff_color_prop = (water_group->addProperty(MATSHPROP_TYPE_COLOR));
        diff_color_prop->mPropCaption = "Color"; //Set caption in Inspector
        diff_color_prop->mPropId = "c_diffuse"; //Identifier to save
        diff_color_prop->start_offset = 16;

        MaterialShaderProperty* time_prop = (water_group->addProperty(MATSHPROP_TYPE_FLOAT));
        time_prop->mPropId = "f_time"; //Identifier to save
        time_prop->start_offset = 32;
        time_prop->mSaveInFile = false;
        time_prop->mShowInEditor = false;

        MtShProps::addMtShaderPropertyGroup(water_group);
    }
    //Default skybox material
    {
        MaterialTemplate* default_sky_group = allocMaterialTemplate(skybox, 20);
        default_sky_group->str_path = "@skybox";
        default_sky_group->Label = "Default Skybox";
        Texture3MaterialShaderProperty* sky_texture =
            static_cast<Texture3MaterialShaderProperty*>(default_sky_group->addProperty(MATSHPROP_TYPE_TEXTURE3));
        sky_texture->slotToBind = 0;
        sky_texture->mPropCaption = "Sky";
        sky_texture->mPropId = "skytexture3"; //Identifier to save

        MaterialShaderProperty* tint_color_prop = (default_sky_group->addProperty(MATSHPROP_TYPE_COLOR));
        tint_color_prop->mPropCaption = "Tint Color"; //Set caption in Inspector
        tint_color_prop->mPropId = "c_tint"; //Identifier to save
        tint_color_prop->start_offset = 0;

        MaterialShaderProperty* shininess_factor_prop = default_sky_group->addProperty(MATSHPROP_TYPE_FLOAT);
        shininess_factor_prop->mPropCaption = "Exposure";
        shininess_factor_prop->mPropId = "f_exposure"; //Identifier to save
        shininess_factor_prop->start_offset = 12;

        MtShProps::addMtShaderPropertyGroup(default_sky_group);
    }
    //Default terrain material
    MaterialTemplate* default_heightmap_group = allocMaterialTemplate(heightmap, 0);
    {
        default_heightmap_group->str_path = "@heightmap";
        default_heightmap_group->Label = "Default Heightmap";
        default_heightmap_group->mAcceptShadows = true;

        MtShProps::addMtShaderPropertyGroup(default_heightmap_group);
    }
    //Create default base material
    default3dmat = allocMaterial(default_group);
    default3dmat->file_path = "@default";

    defaultTerrainMat = allocMaterial(default_heightmap_group);
    defaultTerrainMat->file_path = "@defaultHeightmap";

    MaterialTemplate* particle_group = allocMaterialTemplate(particle_shader, 20);
    {
        particle_group->mAcceptShadows = false;
        particle_group->str_path = "@default_particle";
        particle_group->Label = "Default Particle";

        TextureMaterialShaderProperty* diffuse_prop =
            static_cast<TextureMaterialShaderProperty*>(particle_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        diffuse_prop->slotToBind = 0;
        diffuse_prop->mPropCaption = "Diffuse";
        diffuse_prop->mPropId = "t_diffuse"; //Identifier to save
        diffuse_prop->start_offset = 16;

        MaterialShaderProperty* diff_color_prop = (particle_group->addProperty(MATSHPROP_TYPE_COLOR));
        diff_color_prop->mPropCaption = "Color"; //Set caption in Inspector
        diff_color_prop->mPropId = "c_diffuse"; //Identifier to save
        diff_color_prop->start_offset = 0;

        ((VKMaterialTemplate*)particle_group)->CreatePipeline();
        MtShProps::addMtShaderPropertyGroup(particle_group);
    }

    return default_group;
}

MaterialTemplate* MtShProps::genDefaultMtShGroup2D(Engine::Shader* tile_shader) {
    MaterialTemplate* default_group = allocMaterialTemplate(tile_shader, 0);
    {
        default_group->mAcceptShadows = false;
        default_group->str_path = "@default";
        default_group->Label = "Default 2D";

        TextureMaterialShaderProperty* diffuse_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        diffuse_prop->slotToBind = 0;
        diffuse_prop->mPropCaption = "Diffuse";
        diffuse_prop->mPropId = "t_diffuse"; //Identifier to save
        diffuse_prop->start_offset = 20;

        TextureMaterialShaderProperty* transparent_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        transparent_prop->slotToBind = 1;
        transparent_prop->mPropCaption = "Transparent";
        transparent_prop->mPropId = "t_transparent"; //Identifier to save
        transparent_prop->start_offset = 24;

        ((VKMaterialTemplate*)default_group)->CreatePipeline();
        MtShProps::addMtShaderPropertyGroup(default_group);
    }

    default3dmat = allocMaterial(default_group);
    default3dmat->file_path = "@default";

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
