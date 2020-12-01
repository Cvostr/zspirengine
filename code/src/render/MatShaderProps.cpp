#include "../../headers/render/Material.hpp"
#include <fstream>
#include <iostream>
#include <GL/glew.h>
#include "../../headers/game.h"
#include "../../headers/engine/Resources.hpp"

Material* default3dmat;
Material* defaultTerrainMat;
static std::vector<MtShaderPropertiesGroup*> MatGroups;
//Hack to support resources
extern ZSGAME_DATA* game_data;

MaterialShaderProperty* MtShaderPropertiesGroup::addProperty(int type){
    //Allocate property in heap
    MaterialShaderProperty* newprop_ptr = MtShProps::allocateProperty(type);
    //Push new material
    this->properties.push_back(newprop_ptr);
    //Return new material pointer from vector
    return properties[properties.size() - 1];
}

void MtShaderPropertiesGroup::loadFromFile(const char* fpath){
    std::ifstream mat_shader_group;
    mat_shader_group.open(fpath);

    mat_shader_group.close();
}
MtShaderPropertiesGroup::MtShaderPropertiesGroup(Engine::Shader* shader, const char* UB_CAPTION, unsigned int UB_ConnectID, unsigned int UB_SIZE){
    render_shader = shader;

    if(UB_SIZE == 0 || strlen(UB_CAPTION) == 0) return;
    this->UB_ConnectID = UB_ConnectID;
    //First of all, tell shader uniform buffer point
    //Get Index of buffer
    //shader->setUniformBufferBinding(UB_CAPTION, UB_ConnectID);

    //Generate uniform buffer
    this->UB_ID = Engine::allocUniformBuffer();
    this->UB_ID->init(UB_ConnectID, UB_SIZE);

    acceptShadows = false;
    properties.resize(0);
}

void MtShaderPropertiesGroup::setUB_Data(unsigned int offset, unsigned int size, void* data){

    this->UB_ID->writeData(offset, size, data);
}

MtShaderPropertiesGroup* MtShProps::genDefaultMtShGroup(Engine::Shader* shader3d, Engine::Shader* skybox,
                                                        Engine::Shader* heightmap,
                                                        Engine::Shader* water){

    MtShaderPropertiesGroup* default_group = new MtShaderPropertiesGroup(shader3d, "Default3d", 50, 48);
    {
        default_group->acceptShadows = true;
        default_group->str_path = "@default";
        default_group->groupCaption = "Default 3D";

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

        MtShProps::addMtShaderPropertyGroup(default_group);
    }
    {

        //Water
        MtShaderPropertiesGroup* water_group = new MtShaderPropertiesGroup(water, "WaterData", 51, 32);
        water_group->acceptShadows = true;
        water_group->str_path = "@basewater";
        water_group->groupCaption = "Water";

        TextureMaterialShaderProperty* reflection_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(water_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        reflection_texture_prop->slotToBind = 0;
        reflection_texture_prop->prop_caption = "Reflection"; //Set caption in Inspector
        reflection_texture_prop->prop_identifier = "t_reflection"; //Identifier to save
        reflection_texture_prop->start_offset = 0;

        TextureMaterialShaderProperty* refraction_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(water_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        refraction_texture_prop->slotToBind = 1;
        refraction_texture_prop->prop_caption = "Refraction"; //Set caption in Inspector
        refraction_texture_prop->prop_identifier = "t_refraction"; //Identifier to save
        refraction_texture_prop->start_offset = 4;

        TextureMaterialShaderProperty* distortion_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(water_group->addProperty(MATSHPROP_TYPE_TEXTURE));
        distortion_texture_prop->slotToBind = 2;
        distortion_texture_prop->prop_caption = "Distortion"; //Set caption in Inspector
        distortion_texture_prop->prop_identifier = "t_distortion"; //Identifier to save
        distortion_texture_prop->start_offset = 8;

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
    MtShaderPropertiesGroup* default_sky_group = new MtShaderPropertiesGroup(skybox, "", 0, 0);
    default_sky_group->str_path = "@skybox";
    default_sky_group->groupCaption = "Default Skybox";
    Texture3MaterialShaderProperty* sky_texture =
            static_cast<Texture3MaterialShaderProperty*>(default_sky_group->addProperty(MATSHPROP_TYPE_TEXTURE3));
    sky_texture->slotToBind = 0;
    sky_texture->prop_caption = "Sky";
    sky_texture->prop_identifier = "skytexture3"; //Identifier to save

    MtShProps::addMtShaderPropertyGroup(default_sky_group);

//Default terrain material
    MtShaderPropertiesGroup* default_heightmap_group = new MtShaderPropertiesGroup(heightmap, "", 0, 0);
    default_heightmap_group->str_path = "@heightmap";
    default_heightmap_group->groupCaption = "Default Heightmap";
    default_heightmap_group->acceptShadows = true;

    MtShProps::addMtShaderPropertyGroup(default_heightmap_group);

    //Create default base material
    default3dmat = new Material(default_group);
    default3dmat->file_path = "@default";

    defaultTerrainMat = new Material(default_heightmap_group);
    defaultTerrainMat->file_path = "@defaultHeightmap";

    return default_group;
}
MtShaderPropertiesGroup* MtShProps::getDefaultMtShGroup(){
    return MatGroups[0];
}

void MtShProps::addMtShaderPropertyGroup(MtShaderPropertiesGroup* group){
    //Check if this property already added
    if(getMtShaderPropertyGroup(group->str_path) == nullptr)
        MatGroups.push_back(group);
}
MtShaderPropertiesGroup* MtShProps::getMtShaderPropertyGroup(std::string group_name){
    for(unsigned int group_i = 0; group_i < MatGroups.size(); group_i ++){
        MtShaderPropertiesGroup* group_ptr = MatGroups[group_i];
        if(group_ptr->str_path.compare(group_name) == false)
            return group_ptr;
    }
    return nullptr;
}

MtShaderPropertiesGroup* MtShProps::getMtShaderPropertyGroupByLabel(std::string group_label){
    for(unsigned int group_i = 0; group_i < MatGroups.size(); group_i ++){
        MtShaderPropertiesGroup* group_ptr = MatGroups[group_i];
        if(group_ptr->groupCaption.compare(group_label) == false)
            return group_ptr;
    }
    return nullptr;
}

unsigned int MtShProps::getMaterialShaderPropertyAmount(){
    return static_cast<unsigned int>(MatGroups.size());
}
MtShaderPropertiesGroup* MtShProps::getMtShaderPropertiesGroupByIndex(unsigned int index){
    return MatGroups[index];
}

void MtShProps::clearMtShaderGroups(){
   MatGroups.clear();
}

void Material::saveToFile(){
    //if it is a default material, then quit function
    if(file_path[0] == '@') return;

    ZsStream mat_stream;
    mat_stream.open(file_path, std::ofstream::out);

    if(!mat_stream){ //File isn't opened
        std::cout << "Error opening output stream " << file_path;
        return;
    }

    //Write material header
    mat_stream << "ZSP_MATERIAL\n";
    //Write group string
    mat_stream << "_GROUP " << (this->group_str + '\0') << "\n";
    for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){
        //Obtain pointers to prop and prop's configuration
        MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = this->confs[prop_i];
        //write entry header
        mat_stream << "_ENTRY " << (prop_ptr->prop_identifier + '\0'); //Write identifier

        switch(prop_ptr->type){
            case MATSHPROP_TYPE_NONE:{
                break;
            }
            case MATSHPROP_TYPE_TEXTURE:{
                //Cast pointer
                TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeString(texture_conf->path);
                break;
            }
            case MATSHPROP_TYPE_FLOAT:{
                //Cast pointer
                FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeBinaryValue(&float_conf->value);
                break;
            }
            case MATSHPROP_TYPE_INTEGER:{
                //Cast pointer
                IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeBinaryValue(&int_conf->value);
                break;
            }
            case MATSHPROP_TYPE_COLOR:{
                //Cast pointer
                ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeBinaryValue(&color_conf->color.r);
                mat_stream.writeBinaryValue(&color_conf->color.g);
                mat_stream.writeBinaryValue(&color_conf->color.b);
                break;
            }
            case MATSHPROP_TYPE_FVEC3:{
                //Cast pointer
                Float3MtShPropConf* fvec3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeBinaryValue(&fvec3_conf->value.X);
                mat_stream.writeBinaryValue(&fvec3_conf->value.Y);
                mat_stream.writeBinaryValue(&fvec3_conf->value.Z);
                break;
            }
            case MATSHPROP_TYPE_FVEC2:{
                //Cast pointer
                Float2MtShPropConf* fvec2_conf = static_cast<Float2MtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeBinaryValue(&fvec2_conf->value.X);
                mat_stream.writeBinaryValue(&fvec2_conf->value.Y);
                break;
            }
            case MATSHPROP_TYPE_IVEC2:{
                //Cast pointer
                Int2MtShPropConf* ivec2_conf = static_cast<Int2MtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeBinaryValue(&ivec2_conf->value[0]);
                mat_stream.writeBinaryValue(&ivec2_conf->value[1]);
                break;
            }
            case MATSHPROP_TYPE_TEXTURE3:{
                //Cast pointer
                Texture3MtShPropConf* tex3_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);
                //Write value
                for (int i = 0; i < 6; i++)
                    mat_stream.writeString(tex3_conf->texture_str[i]);

                break;
            }
        }
    mat_stream << "\n"; //Write divider
    }
    mat_stream.close(); //close stream
}

void Material::loadFromBuffer(char* buffer, unsigned int size){
    //Define and open file stream
    unsigned int position = 0;

    char test_header[13];
    //Read header
    memcpy(test_header, &buffer[position], 12);
    test_header[12] = '\0'; //Terminate string

    if(strcmp(test_header, "ZSP_MATERIAL") == 1) //If it isn't zspire material
        return; //Go out, we have nothing to do

    position += 13;

    while(position <= size){ //While file not finished reading
        char prefix[7];
        prefix[6] = '\0';
        memcpy(prefix, &buffer[position], 6);

        if(strcmp(prefix, "_GROUP") && strcmp(prefix, "_ENTRY"))
            position ++;

        if(strcmp(prefix, "_GROUP") == 0){ //if it is game object
            position += 7;
            std::string group_name;
            //Read shader group name
            readString(group_name, buffer, position);
            position += 1;

            setPropertyGroup(MtShProps::getMtShaderPropertyGroup(group_name));
        }

        if(strcmp(prefix, "_ENTRY") == 0){ //if it is game object
            position += 7;
            std::string prop_identifier;
            readString(prop_identifier, buffer, position);

            for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){
                MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
                MaterialShaderPropertyConf* conf_ptr = this->confs[prop_i];
                //check if compare
                if(prop_identifier.compare(prop_ptr->prop_identifier) == 0){
                    switch(prop_ptr->type){
                        case MATSHPROP_TYPE_NONE:{
                            break;
                        }
                        case MATSHPROP_TYPE_TEXTURE:{
                            //Cast pointer
                            TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);
                            readString(texture_conf->path, buffer, position);

                            position += 1;
                            break;
                        }

                        case MATSHPROP_TYPE_FLOAT:{
                            //Cast pointer
                            FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);

                            readBinaryValue(&float_conf->value, buffer + position, position);

                            break;
                        }

                        case MATSHPROP_TYPE_INTEGER:{
                            //Cast pointer
                            IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);

                            readBinaryValue(&int_conf->value, &buffer[position], position);
                            break;
                        }

                        case MATSHPROP_TYPE_COLOR:{
                            //Cast pointer
                            ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);
                            //Read color values
                            readBinaryValue(&color_conf->color.r, buffer + position, position);
                            readBinaryValue(&color_conf->color.g, buffer + position, position);
                            readBinaryValue(&color_conf->color.b, buffer + position, position);
                            position += 1;
                            break;
                        }

                        case MATSHPROP_TYPE_FVEC3:{
                            //Cast pointer
                            Float3MtShPropConf* fvec3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);
                            //Read float vector values
                            readBinaryValue(&fvec3_conf->value.X, buffer + position, position);
                            readBinaryValue(&fvec3_conf->value.Y, buffer + position, position);
                            readBinaryValue(&fvec3_conf->value.Z, buffer + position, position);
                            position += 1;
                            break;
                        }

                        case MATSHPROP_TYPE_FVEC2:{
                            //Cast pointer
                            Float2MtShPropConf* fvec2_conf = static_cast<Float2MtShPropConf*>(conf_ptr);
                            //Read float vector values
                            readBinaryValue(&fvec2_conf->value.X, buffer + position, position);
                            readBinaryValue(&fvec2_conf->value.Y, buffer + position, position);
                            position += 1;
                            break;
                        }

                        case MATSHPROP_TYPE_IVEC2:{
                            //Cast pointer
                            Int2MtShPropConf* ivec2_conf = static_cast<Int2MtShPropConf*>(conf_ptr);
                            //Read float vector values
                            readBinaryValue(&ivec2_conf->value[0], buffer + position, position);
                            readBinaryValue(&ivec2_conf->value[1], buffer + position, position);

                            position += 1;
                            break;
                        }

                        case MATSHPROP_TYPE_TEXTURE3:{
                            //Cast pointer
                            Texture3MtShPropConf* texture3_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);

                            for(int i = 0; i < 6; i ++){
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

void Material::loadFromFile(std::string fpath){
    std::cout << "Loading Material " << fpath << std::endl;

    this->file_path = fpath;

    std::ifstream mat_stream;
    //Open stream
    mat_stream.open(fpath, std::ifstream::in | std::ifstream::ate);

    if(!mat_stream){ //File isn't opened
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

void Material::applyMatToPipeline(){
    Engine::Shader* shader = this->group_ptr->render_shader;
    if(shader == nullptr)
        return;
    shader->Use();
    if(group_ptr->UB_ID != nullptr)
        group_ptr->UB_ID->bind();
    //iterate over all properties, send them all!
    for(unsigned int prop_i = 0; prop_i < group_ptr->properties.size(); prop_i ++){
        MaterialShaderProperty* prop_ptr = group_ptr->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = confs[prop_i];
        switch(prop_ptr->type){
            case MATSHPROP_TYPE_NONE:{
                break;
            }
            case MATSHPROP_TYPE_TEXTURE:{
                //Cast pointer
                TextureMaterialShaderProperty* texture_p = static_cast<TextureMaterialShaderProperty*>(prop_ptr);
                TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);

                int db = 0;
                //If correct path is set to texture
                if(texture_conf->path.compare("@none")){
                    //if texture isn't loaded
                    if(texture_conf->texture == nullptr){
                        //Try to find texture
                        texture_conf->texture = game_data->resources->getTextureByLabel(texture_conf->path);
                        if (texture_conf->texture == nullptr) break; //No resource with that name, exiting
                    }
                    //Set opengl texture
                    db = 1;
                    Engine::TextureResource* tex_ptr = static_cast<Engine::TextureResource*>(texture_conf->texture);
                    tex_ptr->Use(texture_p->slotToBind);
                }
                //Set texture state
                group_ptr->setUB_Data(texture_p->start_offset, 4, &db);

                break;
            }
            case MATSHPROP_TYPE_FLOAT:{
                FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);

                //set float to buffer
                group_ptr->setUB_Data(prop_ptr->start_offset, 4, &float_conf->value);

                break;
            }
            case MATSHPROP_TYPE_INTEGER:{
                IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);

                //set integer to buffer
                group_ptr->setUB_Data(prop_ptr->start_offset, 4, &int_conf->value);

                break;
            }
            case MATSHPROP_TYPE_COLOR:{
                ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);

                color_conf->color.updateGL();

                //Write color to buffer
                group_ptr->setUB_Data(prop_ptr->start_offset, 4, &color_conf->color.gl_r);
                group_ptr->setUB_Data(prop_ptr->start_offset + 4, 4, &color_conf->color.gl_g);
                group_ptr->setUB_Data(prop_ptr->start_offset + 8, 4, &color_conf->color.gl_b);


                break;
            }
            case MATSHPROP_TYPE_FVEC3:{
                Float3MtShPropConf* fvec3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);

                //Write vec3 to buffer
                group_ptr->setUB_Data(prop_ptr->start_offset, 4, &fvec3_conf->value.X);
                group_ptr->setUB_Data(prop_ptr->start_offset + 4, 4, &fvec3_conf->value.Y);
                group_ptr->setUB_Data(prop_ptr->start_offset + 8, 4, &fvec3_conf->value.Z);

                break;
            }
            case MATSHPROP_TYPE_FVEC2:{
                Float2MtShPropConf* fvec2_conf = static_cast<Float2MtShPropConf*>(conf_ptr);

                //Write vec2 to buffer
                group_ptr->setUB_Data(prop_ptr->start_offset, 4, &fvec2_conf->value.X);
                group_ptr->setUB_Data(prop_ptr->start_offset + 4, 4, &fvec2_conf->value.Y);
                break;
            }
            case MATSHPROP_TYPE_IVEC2:{
                Int2MtShPropConf* ivec2_conf = static_cast<Int2MtShPropConf*>(conf_ptr);

                //Write vec2 to buffer
                group_ptr->setUB_Data(prop_ptr->start_offset, 4, &ivec2_conf->value[0]);
                group_ptr->setUB_Data(prop_ptr->start_offset + 4, 4, &ivec2_conf->value[1]);
                break;
            }
            case MATSHPROP_TYPE_TEXTURE3:{
                //Cast pointer
                Texture3MaterialShaderProperty* texture_p = static_cast<Texture3MaterialShaderProperty*>(prop_ptr);
                Texture3MtShPropConf* texture_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);
                //Check, is texture already created
                if(!texture_conf->texture3D->mCreated){
                    //If not created, create OpenGL instance
                    texture_conf->texture3D->Init();

                    for(int i = 0; i < 6; i ++){
                        texture_conf->texture3D->pushTexture(i, texture_conf->texture_str[i]);
                    }
                    //Change Flag state
                    texture_conf->texture3D->mCreated = true;
                    texture_conf->texture3D->Use(texture_p->slotToBind);
                }else{
                    //3D texture already created, Use it
                    texture_conf->texture3D->Use(texture_p->slotToBind);
                }
                break;
            }
        }
    }
}
