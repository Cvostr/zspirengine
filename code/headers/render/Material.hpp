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


class MtShaderPropertiesGroup{
public:
    std::string str_path;
    std::string groupCaption;
    bool acceptShadows;
    //ID of connected shader uniform buffer
    unsigned int UB_ConnectID;
    //ID of uniform buffer
    Engine::UniformBuffer* UB_ID;
    void setUB_Data(unsigned int offset, unsigned int size, void* data);


    Engine::Shader* render_shader; //Pointer to shader, that binds on object render
    std::vector<MaterialShaderProperty*> properties;

    MaterialShaderProperty* addProperty(int type);
    void loadFromFile(const char* fpath);

    MtShaderPropertiesGroup(Engine::Shader* shader, const char* UB_CAPTION, unsigned int UB_ConnectID, unsigned int UB_SIZE);
};

class Material{
private:
    std::string group_str;
public:
    Engine::ZSVulkanPipeline* Pipeline;
    //path to material file
    std::string file_path;
    //Pointer to shader group
    MtShaderPropertiesGroup* group_ptr;
    std::vector<MaterialShaderPropertyConf*> confs;

    MaterialShaderPropertyConf* addPropertyConf(int type);
    void loadFromFile(std::string fpath);
    void loadFromBuffer(char* buffer, unsigned int size);
    void saveToFile();
    void setPropertyGroup(MtShaderPropertiesGroup* group_ptr);
    void clear();
    void applyMatToPipeline();

    Material(std::string shader_group_str);
    Material(MtShaderPropertiesGroup* _group_ptr);
    Material();

    ~Material();
};

namespace MtShProps {
    MaterialShaderProperty* allocateProperty(int type);
    MaterialShaderPropertyConf* allocatePropertyConf(int type);

    MtShaderPropertiesGroup* genDefaultMtShGroup(Engine::Shader* shader3d, Engine::Shader* skybox,
                                                 Engine::Shader* heightmap,
        Engine::Shader* water);
    MtShaderPropertiesGroup* getDefaultMtShGroup();

    void addMtShaderPropertyGroup(MtShaderPropertiesGroup* group);
    MtShaderPropertiesGroup* getMtShaderPropertyGroup(std::string group_name);
    MtShaderPropertiesGroup* getMtShaderPropertyGroupByLabel(std::string group_label);
    //Get amount of registered material shader properties
    unsigned int getMaterialShaderPropertyAmount();
    //Get shader properties by index
    MtShaderPropertiesGroup* getMtShaderPropertiesGroupByIndex(unsigned int index);

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