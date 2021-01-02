#pragma once

#include "Texture.h"
#include "Shader.hpp"
#include "UniformBuffer.hpp"
#include <vector>
#include "../vulkan/ZSVulkanPipeline.hpp"

enum MATSHPROP_TYPE{
    MATSHPROP_TYPE_NONE,
    MATSHPROP_TYPE_TEXTURE,
    MATSHPROP_TYPE_INTEGER,
    MATSHPROP_TYPE_FLOAT,
    MATSHPROP_TYPE_FVEC3,
    MATSHPROP_TYPE_COLOR,
    MATSHPROP_TYPE_TEXTURE3,
    MATSHPROP_TYPE_IVEC2,
    MATSHPROP_TYPE_FVEC2
};

//This class describes the property
class MaterialShaderProperty{
public:
    MATSHPROP_TYPE type; //type of property
    std::string prop_caption;
    std::string prop_identifier; //String identifier, that will appear in material file
    unsigned int start_offset;

    MaterialShaderProperty();
};

class MaterialShaderPropertyConf{
public:
    MATSHPROP_TYPE type;

    MaterialShaderPropertyConf();
};


class MaterialTemplate {
public:
    std::string str_path;
    std::string Label;
    bool mAcceptShadows;
    Engine::Shader* mShader; //Pointer to shader, that binds on object render
    std::vector<MaterialShaderProperty*> properties;
    //ID of uniform buffer
    Engine::UniformBuffer* mUniformBuffer;

    Engine::ZSVulkanPipeline* Pipeline;

    void setUB_Data(unsigned int offset, unsigned int size, void* data);

    MaterialShaderProperty* addProperty(int type);
    void loadFromFile(const char* fpath);

    void CreateVulkanPipeline();
    void MakeDescrSetUniform(Engine::ZSVulkanDescriptorSet* DescrSet);
    void MakeDescrSetTextures(Engine::ZSVulkanDescriptorSet* DescrSet);

    MaterialTemplate(Engine::Shader* shader, unsigned int UB_ConnectID, unsigned int UB_SIZE);
};

class Material{
private:
    std::string mTemplateStr;
public:
    Engine::ZSVulkanDescriptorSet* DescrSetUBO;
    Engine::ZSVulkanDescriptorSet* DescrSetTextures;
    //path to material file
    std::string file_path;
    //Pointer to shader group
    MaterialTemplate* mTemplate;
    std::vector<MaterialShaderPropertyConf*> confs;

    MaterialShaderPropertyConf* addPropertyConf(int type);
    void loadFromFile(std::string fpath);
    void loadFromBuffer(char* buffer, unsigned int size);
    void saveToFile();
    void setTemplate(MaterialTemplate* Template);
    void clear();
    void applyMatToPipeline();

    Material(std::string shader_group_str);
    Material(MaterialTemplate* Template);
    Material();

    ~Material();
};

namespace MtShProps {
    MaterialShaderProperty* allocateProperty(int type);
    MaterialShaderPropertyConf* allocatePropertyConf(int type);

    MaterialTemplate* genDefaultMtShGroup(Engine::Shader* shader3d, Engine::Shader* skybox,
                                                 Engine::Shader* heightmap,
        Engine::Shader* water);
    MaterialTemplate* getDefaultMtShGroup();

    void addMtShaderPropertyGroup(MaterialTemplate* group);
    MaterialTemplate* getMtShaderPropertyGroup(std::string group_name);
    MaterialTemplate* getMtShaderPropertyGroupByLabel(std::string group_label);
    //Get amount of registered material shader properties
    unsigned int getMaterialShaderPropertyAmount();
    //Get shader properties by index
    MaterialTemplate* getMaterialTemplateByIndex(unsigned int index);

    void clearMtShaderGroups();
}

class TextureMaterialShaderProperty : public MaterialShaderProperty{
public:
    int slotToBind; //Slot to texture

    TextureMaterialShaderProperty();
};

class Texture3MaterialShaderProperty : public MaterialShaderProperty{
public:
    int slotToBind; //Slot to texture
    //std::string ToggleUniform; //Uniform to set to 1

    Texture3MaterialShaderProperty();
};

//Property configurations
class TextureMtShPropConf : public MaterialShaderPropertyConf{
public:
    void* texture;
    std::string path;

    TextureMtShPropConf();
};

class IntegerMtShPropConf : public MaterialShaderPropertyConf{
public:
    int value;
    //Construct
    IntegerMtShPropConf();
};
class FloatMtShPropConf : public MaterialShaderPropertyConf{
public:
    float value;
    //Construct
    FloatMtShPropConf();
};
class Float3MtShPropConf : public MaterialShaderPropertyConf{
public:
    Vec3 value;
    //Construct
    Float3MtShPropConf();
};
class Float2MtShPropConf : public MaterialShaderPropertyConf{
public:
    Vec2 value;
    //Construct
    Float2MtShPropConf();
};
class Int2MtShPropConf : public MaterialShaderPropertyConf{
public:
    int value[2];
    //Construct
    Int2MtShPropConf();
};
class ColorMtShPropConf : public MaterialShaderPropertyConf{
public:
    ZSRGBCOLOR color;

    ColorMtShPropConf();
};
class Texture3MtShPropConf : public MaterialShaderPropertyConf{
public:
    std::string texture_str[6];
    Engine::Texture3D* texture3D;

     Texture3MtShPropConf();
};