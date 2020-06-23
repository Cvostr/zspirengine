#include "../../headers/render/zs-materials.h"
#include <fstream>
#include <iostream>
#include <GL/glew.h>
#include "../../headers/game.h"
#include "../../headers/engine/resources.h"

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
    shader->setUniformBufferBinding(UB_CAPTION, UB_ConnectID);

    //Generate uniform buffer
    this->UB_ID = Engine::allocUniformBuffer();
    this->UB_ID->init(UB_ConnectID, UB_SIZE);

    acceptShadows = false;
    properties.resize(0);
}

void MtShaderPropertiesGroup::setUB_Data(unsigned int offset, unsigned int size, void* data){
    this->UB_ID->bind();
    this->UB_ID->writeData(offset, size, data);
}

MtShaderPropertiesGroup* MtShProps::genDefaultMtShGroup(Engine::Shader* shader3d, Engine::Shader* skybox,
                                                        Engine::Shader* heightmap,
                                                        unsigned int uniform_buf_id_took){

    MtShaderPropertiesGroup* default_group = new MtShaderPropertiesGroup(shader3d, "Default3d", uniform_buf_id_took + 1, 60);
    default_group->acceptShadows = true;
    default_group->str_path = "@default";
    default_group->groupCaption = "Default 3D";

    ColorMaterialShaderProperty* diff_color_prop =
            static_cast<ColorMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_COLOR));
    diff_color_prop->prop_caption = "Color"; //Set caption in Inspector
    diff_color_prop->prop_identifier = "c_diffuse"; //Identifier to save
    diff_color_prop->colorUniform = "diffuse_color";
    diff_color_prop->start_offset = 0;

    TextureMaterialShaderProperty* diff_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
    diff_texture_prop->slotToBind = 0;
    diff_texture_prop->prop_caption = "Diffuse"; //Set caption in Inspector
    diff_texture_prop->ToggleUniform = "hasDiffuseMap";
    diff_texture_prop->prop_identifier = "t_diffuse"; //Identifier to save
    diff_texture_prop->start_offset = 12;

    TextureMaterialShaderProperty* normal_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
    normal_texture_prop->slotToBind = 1;
    normal_texture_prop->prop_caption = "Normal";
    normal_texture_prop->ToggleUniform = "hasNormalMap";
    normal_texture_prop->prop_identifier = "t_normal"; //Identifier to save
    normal_texture_prop->start_offset = 16;

    TextureMaterialShaderProperty* specular_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
    specular_texture_prop->slotToBind = 2;
    specular_texture_prop->prop_caption = "Specular";
    specular_texture_prop->ToggleUniform = "hasSpecularMap";
    specular_texture_prop->prop_identifier = "t_specular"; //Identifier to save
    specular_texture_prop->start_offset = 20;


    TextureMaterialShaderProperty* height_texture_prop =
            static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
    height_texture_prop->slotToBind = 3;
    height_texture_prop->prop_caption = "Height";
    height_texture_prop->ToggleUniform = "hasHeightMap";
    height_texture_prop->prop_identifier = "t_height"; //Identifier to save
    height_texture_prop->start_offset = 24;

    TextureMaterialShaderProperty* ao_texture_prop =
        static_cast<TextureMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_TEXTURE));
    ao_texture_prop->slotToBind = 4;
    ao_texture_prop->prop_caption = "Amb Occlusion";
    ao_texture_prop->ToggleUniform = "hasAoMap";
    ao_texture_prop->prop_identifier = "t_occlusion"; //Identifier to save
    ao_texture_prop->start_offset = 28;

    FloatMaterialShaderProperty* shininess_factor_prop =
            static_cast<FloatMaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_FLOAT));
    shininess_factor_prop->floatUniform = "material_shininess";
    shininess_factor_prop->prop_caption = "Shininess";
    shininess_factor_prop->prop_identifier = "f_shininess"; //Identifier to save
    shininess_factor_prop->start_offset = 32;

    Int2MaterialShaderProperty* uv_factor_prop =
            static_cast<Int2MaterialShaderProperty*>(default_group->addProperty(MATSHPROP_TYPE_IVEC2));
    uv_factor_prop->floatUniform = "uv_repeat";
    uv_factor_prop->prop_caption = "UV repeat";
    uv_factor_prop->prop_identifier = "i_uv_repeat"; //Identifier to save
    uv_factor_prop->start_offset = 36;

    MtShProps::addMtShaderPropertyGroup(default_group);

//Default skybox material
    MtShaderPropertiesGroup* default_sky_group = new MtShaderPropertiesGroup(skybox, "", 0, 0);
    default_sky_group->str_path = "@skybox";
    default_sky_group->groupCaption = "Default Skybox";
    Texture3MaterialShaderProperty* sky_texture =
            static_cast<Texture3MaterialShaderProperty*>(default_sky_group->addProperty(MATSHPROP_TYPE_TEXTURE3));
    sky_texture->slotToBind = 0;
    sky_texture->prop_caption = "Sky";
    sky_texture->ToggleUniform = "hasSpecularMap";
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

    std::ofstream mat_stream;
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
                mat_stream << texture_conf->path + '\0';
                break;
            }
            case MATSHPROP_TYPE_FLOAT:{
                //Cast pointer
                FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.write(reinterpret_cast<char*>(&float_conf->value), sizeof (float));
                break;
            }
            case MATSHPROP_TYPE_INTEGER:{
                //Cast pointer
                IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.write(reinterpret_cast<char*>(&int_conf->value), sizeof(int));
                break;
            }
            case MATSHPROP_TYPE_COLOR:{
                //Cast pointer
                ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.write(reinterpret_cast<char*>(&color_conf->color.r), 4);
                mat_stream.write(reinterpret_cast<char*>(&color_conf->color.g), 4);
                mat_stream.write(reinterpret_cast<char*>(&color_conf->color.b), 4);
                break;
            }
            case MATSHPROP_TYPE_FVEC3:{
                //Cast pointer
                Float3MtShPropConf* fvec3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.write(reinterpret_cast<char*>(&fvec3_conf->value.X), sizeof (float));
                mat_stream.write(reinterpret_cast<char*>(&fvec3_conf->value.Y), sizeof (float));
                mat_stream.write(reinterpret_cast<char*>(&fvec3_conf->value.Z), sizeof (float));
                break;
            }
            case MATSHPROP_TYPE_FVEC2:{
                //Cast pointer
                Float2MtShPropConf* fvec2_conf = static_cast<Float2MtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.write(reinterpret_cast<char*>(&fvec2_conf->value.X), sizeof (float));
                mat_stream.write(reinterpret_cast<char*>(&fvec2_conf->value.Y), sizeof (float));
                break;
            }
            case MATSHPROP_TYPE_IVEC2:{
                //Cast pointer
                Int2MtShPropConf* ivec2_conf = static_cast<Int2MtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.write(reinterpret_cast<char*>(&ivec2_conf->value[0]), sizeof (int));
                mat_stream.write(reinterpret_cast<char*>(&ivec2_conf->value[1]), sizeof (int));
                break;
            }
            case MATSHPROP_TYPE_TEXTURE3:{
                //Cast pointer
                Texture3MtShPropConf* tex3_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);
                //Write value
                for(int i = 0; i < 6; i ++)
                    mat_stream << (tex3_conf->texture_str[i] + '\0');

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
            char group_name[100];
            //Read shader group name
            strcpy(group_name, &buffer[position]);
            position += static_cast<unsigned int>(strlen(group_name)) + 2;

            setPropertyGroup(MtShProps::getMtShaderPropertyGroup(std::string(group_name)));
        }

        if(strcmp(prefix, "_ENTRY") == 0){ //if it is game object
            position += 7;
            char _prop_identifier[100];
            strcpy(_prop_identifier, &buffer[position]);
            std::string prop_identifier = std::string(_prop_identifier);
            position += static_cast<unsigned int>(prop_identifier.size()) + 1;

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
                            char texture_relpath[64];

                            strcpy(texture_relpath, &buffer[position]);
                            position += static_cast<unsigned int>(strlen(texture_relpath));
                            texture_conf->path = std::string(texture_relpath);

                            position += 2;
                            break;
                        }

                        case MATSHPROP_TYPE_FLOAT:{
                            //Cast pointer
                            FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);

                            memcpy(&float_conf->value, &buffer[position], sizeof(float));
                            position += sizeof (float) + 1;
                            break;
                        }

                        case MATSHPROP_TYPE_INTEGER:{
                            //Cast pointer
                            IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);

                            memcpy(&int_conf->value, &buffer[position], sizeof(int));
                            position += sizeof (int);
                            break;
                        }

                        case MATSHPROP_TYPE_COLOR:{
                            //Cast pointer
                            ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);
                            //Read color values
                            memcpy(&color_conf->color.r, &buffer[position], sizeof(int));
                            position += sizeof (int);
                            memcpy(&color_conf->color.g, &buffer[position], sizeof(int));
                            position += sizeof (int);
                            memcpy(&color_conf->color.b, &buffer[position], sizeof(int));
                            position += sizeof (int);
                            position += 1;
                            break;
                        }

                        case MATSHPROP_TYPE_FVEC3:{
                            //Cast pointer
                            Float3MtShPropConf* fvec3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);
                            //Read float vector values
                            memcpy(&fvec3_conf->value.X, &buffer[position], sizeof(float));
                            position += sizeof (float);
                            memcpy(&fvec3_conf->value.Y, &buffer[position], sizeof(float));
                            position += sizeof (float);
                            memcpy(&fvec3_conf->value.Z, &buffer[position], sizeof(float));
                            position += sizeof (float);
                            position += 1;
                            break;
                        }

                        case MATSHPROP_TYPE_FVEC2:{
                            //Cast pointer
                            Float2MtShPropConf* fvec2_conf = static_cast<Float2MtShPropConf*>(conf_ptr);
                            //Read float vector values
                            memcpy(&fvec2_conf->value.X, &buffer[position], sizeof(float));
                            position += sizeof (float);
                            memcpy(&fvec2_conf->value.Y, &buffer[position], sizeof(float));
                            position += sizeof (float);

                            position += 1;
                            break;
                        }

                        case MATSHPROP_TYPE_IVEC2:{
                            //Cast pointer
                            Int2MtShPropConf* ivec2_conf = static_cast<Int2MtShPropConf*>(conf_ptr);
                            //Read float vector values
                            memcpy(&ivec2_conf->value[0], &buffer[position], sizeof(int));
                            position += sizeof (int);
                            memcpy(&ivec2_conf->value[1], &buffer[position], sizeof(int));
                            position += sizeof (int);

                            position += 1;
                            break;
                        }

                        case MATSHPROP_TYPE_TEXTURE3:{
                            //Cast pointer
                            Texture3MtShPropConf* texture3_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);

                            for(int i = 0; i < 6; i ++){
                                char texture_relpath[64];
                                strcpy(texture_relpath, &buffer[position]);
                                texture3_conf->texture_str[i] = std::string(texture_relpath);
                                position += static_cast<unsigned int>(strlen(texture_relpath)) + 1;
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
    //iterate over all properties, send them all!
    unsigned int offset = 0;
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

                offset = texture_p->start_offset;
                group_ptr->setUB_Data(offset, 4, &db);

                break;
            }
            case MATSHPROP_TYPE_FLOAT:{
                //Cast pointer
                FloatMaterialShaderProperty* float_p = static_cast<FloatMaterialShaderProperty*>(prop_ptr);
                FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);

                offset = float_p->start_offset;
                //set float to buffer
                group_ptr->setUB_Data(offset, 4, &float_conf->value);

                break;
            }
            case MATSHPROP_TYPE_INTEGER:{
                //Cast pointer
                IntegerMaterialShaderProperty* int_p = static_cast<IntegerMaterialShaderProperty*>(prop_ptr);
                IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);

                offset = int_p->start_offset;
                //set integer to buffer
                group_ptr->setUB_Data(offset, 4, &int_conf->value);

                break;
            }
            case MATSHPROP_TYPE_COLOR:{
                //Cast pointer
                ColorMaterialShaderProperty* color_p = static_cast<ColorMaterialShaderProperty*>(prop_ptr);
                ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);

                color_conf->color.updateGL();

                offset = color_p->start_offset;
                //Write color to buffer
                group_ptr->setUB_Data(offset, 4, &color_conf->color.gl_r);
                group_ptr->setUB_Data(offset + 4, 4, &color_conf->color.gl_g);
                group_ptr->setUB_Data(offset + 8, 4, &color_conf->color.gl_b);


                break;
            }
            case MATSHPROP_TYPE_FVEC3:{
                //Cast pointer
                Float3MaterialShaderProperty* fvec3_p = static_cast<Float3MaterialShaderProperty*>(prop_ptr);
                Float3MtShPropConf* fvec3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);

                offset = fvec3_p->start_offset;
                //Write color to buffer
                group_ptr->setUB_Data(offset, 4, &fvec3_conf->value.X);
                group_ptr->setUB_Data(offset + 4, 4, &fvec3_conf->value.Y);
                group_ptr->setUB_Data(offset + 8, 4, &fvec3_conf->value.Z);

                break;
            }
            case MATSHPROP_TYPE_FVEC2:{
                //Cast pointer
                Float2MaterialShaderProperty* fvec2_p = static_cast<Float2MaterialShaderProperty*>(prop_ptr);
                Float2MtShPropConf* fvec2_conf = static_cast<Float2MtShPropConf*>(conf_ptr);

                offset = fvec2_p->start_offset;
                //Write color to buffer
                group_ptr->setUB_Data(offset, 4, &fvec2_conf->value.X);
                group_ptr->setUB_Data(offset + 4, 4, &fvec2_conf->value.Y);
                break;
            }
            case MATSHPROP_TYPE_IVEC2:{
                //Cast pointer
                Int2MaterialShaderProperty* ivec2_p = static_cast<Int2MaterialShaderProperty*>(prop_ptr);
                Int2MtShPropConf* ivec2_conf = static_cast<Int2MtShPropConf*>(conf_ptr);

                offset = ivec2_p->start_offset;
                //Write color to buffer
                group_ptr->setUB_Data(offset, 4, &ivec2_conf->value[0]);
                group_ptr->setUB_Data(offset + 4, 4, &ivec2_conf->value[1]);
                break;
            }
            case MATSHPROP_TYPE_TEXTURE3:{
                //Cast pointer
                Texture3MaterialShaderProperty* texture_p = static_cast<Texture3MaterialShaderProperty*>(prop_ptr);
                Texture3MtShPropConf* texture_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);

                if(!texture_conf->texture3D->created){
                    texture_conf->texture3D->Init();

                    for(int i = 0; i < 6; i ++){
                        texture_conf->texture3D->pushTexture(i, texture_conf->texture_str[i]);
                    }
                    texture_conf->texture3D->created = true;
                    texture_conf->texture3D->Use(texture_p->slotToBind);
                }else{
                    texture_conf->texture3D->Use(texture_p->slotToBind);
                }
                break;
            }
        }
    }
}
