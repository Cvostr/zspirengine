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
    MATSHPROP_TYPE mType; //type of property
    std::string mPropCaption;
    std::string mPropId; //String identifier, that will appear in material file
    unsigned int start_offset;

    MaterialShaderProperty();
};

class MaterialShaderPropertyConf{
public:
    MATSHPROP_TYPE type;
    MaterialShaderProperty* mProperty;

    virtual void SetIntValue(int value){}
    virtual void SetFloatValue(float value){}
    virtual void SetVec3Value(const Vec3& value) {}
    virtual void SetTexture(void* texture) {}

    MaterialShaderPropertyConf();
};


class MaterialTemplate {
public:
    std::string str_path;
    std::string Label;
    bool mAcceptShadows;
    Engine::Shader* mShader; //Pointer to shader, that binds on object render
    std::vector<MaterialShaderProperty*> properties;

    MaterialShaderProperty* addProperty(int type);
    void loadFromFile(const char* fpath);

    explicit MaterialTemplate(Engine::Shader* shader, unsigned int UB_SIZE);
};

class Material{
protected:
    std::string mTemplateStr;
public:
    //path to material file
    std::string file_path;
    //Pointer to shader group
    MaterialTemplate* mTemplate;
    std::vector<MaterialShaderPropertyConf*> confs;

    MaterialShaderPropertyConf* AddPropertyConf(int type);
    MaterialShaderPropertyConf* GetPropertyConf(std::string Identifier);
    void loadFromFile(std::string fpath);
    void loadFromBuffer(char* buffer, unsigned int size);
    void saveToFile();
    void setTemplate(MaterialTemplate* Template);
    void clear();
    void applyMatToPipeline();
    void WriteBytes(unsigned int offset, unsigned int size, void* data);

    explicit Material(std::string shader_group_str);
    explicit Material(MaterialTemplate* Template);
    Material();

    ~Material();
};

Material* allocMaterial();
Material* allocMaterial(MaterialTemplate* Template);
MaterialTemplate* allocMaterialTemplate(Engine::Shader* shader, unsigned int UB_SIZE);

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

    void SetIntValue(int value) {
        this->value = value;
    }

    //Construct
    IntegerMtShPropConf();
};
class FloatMtShPropConf : public MaterialShaderPropertyConf{
public:
    float value;

    void SetFloatValue(float value) {
        this->value = value;
    }

    //Construct
    FloatMtShPropConf();
};
class Float3MtShPropConf : public MaterialShaderPropertyConf{
public:
    Vec3 value;

    void SetVec3Value(const Vec3& value) {
        this->value = value;
    }

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
    Vec2i value;
    //Construct
    Int2MtShPropConf();
};
class ColorMtShPropConf : public MaterialShaderPropertyConf{
public:
    RGBAColor color;

    ColorMtShPropConf();
};
class Texture3MtShPropConf : public MaterialShaderPropertyConf{
public:
    std::string texture_str[6];
    Engine::Texture3D* texture3D;

     Texture3MtShPropConf();
};