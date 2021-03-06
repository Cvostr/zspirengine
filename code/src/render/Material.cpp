#include <render/Material.hpp>
#include <vulkan/VKMaterial.hpp>
#include <iostream>
#include <misc/misc.h>
#include <game.h>

extern ZSpireEngine* engine_ptr;
//Hack to support resources
extern ZSGAME_DATA* game_data;

Material* allocMaterial() {
    Material* result = nullptr;
    switch (engine_ptr->engine_info->graphicsApi) {
    case OGL: {
        result = new Material;
        break;
    }
    case VULKAN: {
        result = new VKMaterial;
        break;
    }
    }
    return result;
}

Material* allocMaterial(MaterialTemplate* Template) {
    Material* result = nullptr;
    switch (engine_ptr->engine_info->graphicsApi) {
    case OGL: {
        result = new Material(Template);
        break;
    }
    case VULKAN: {
        result = new VKMaterial(Template);
        break;
    }
    }
    return result;
}

MaterialTemplate* allocMaterialTemplate(Engine::Shader* shader, unsigned int UB_SIZE) {
    MaterialTemplate* result = nullptr;
    switch (engine_ptr->engine_info->graphicsApi) {
    case OGL: {
        result = new MaterialTemplate(shader, UB_SIZE);
        break;
    }
    case VULKAN: {
        result = new VKMaterialTemplate(shader, UB_SIZE);
        break;
    }
    }
    return result;
}

MaterialShaderProperty::MaterialShaderProperty(){
    mType = MATSHPROP_TYPE_NONE;
    start_offset = 0;
}

MaterialShaderPropertyConf::MaterialShaderPropertyConf(){
    type = MATSHPROP_TYPE_NONE;
    mProperty = nullptr;
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
}

Float2MtShPropConf::Float2MtShPropConf(){
    type = MATSHPROP_TYPE_FVEC2;
}

Int2MtShPropConf::Int2MtShPropConf(){
    type = MATSHPROP_TYPE_IVEC2;
}

ColorMtShPropConf::ColorMtShPropConf(){
    type = MATSHPROP_TYPE_COLOR;
}
//Textures3D stuff
Texture3MaterialShaderProperty::Texture3MaterialShaderProperty(){
    mType = MATSHPROP_TYPE_TEXTURE3;
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
    mType = MATSHPROP_TYPE_TEXTURE;

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
            _ptr->mType = (MATSHPROP_TYPE)type;
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

void Material::WriteBytes(unsigned int offset, unsigned int size, void* data) {
    if (engine_ptr->engine_info->graphicsApi == OGL) {
        game_data->pipeline->GetMaterialsUniformBuffer()->writeDataBuffered(offset, size, data);
    }
    if (engine_ptr->engine_info->graphicsApi == VULKAN) {
        VKMaterial* _VKMaterial = static_cast<VKMaterial*>(this);
        _VKMaterial->WriteToBuffer(offset, size, (char*)data);
    }
}

void Material::setTemplate(MaterialTemplate* Template){
    this->clear(); //clear all confs, first
    //Iterate over all properties in group
    for(unsigned int prop_i = 0; prop_i < Template->properties.size(); prop_i ++){
        //Obtain pointer to property in group
        MaterialShaderProperty* prop_ptr = Template->properties[prop_i];
        //Add PropertyConf with the same type
        MaterialShaderPropertyConf* Conf = this->AddPropertyConf(prop_ptr->mType);
        Conf->mProperty = prop_ptr;
    }
    //store pointer of picked group
    this->mTemplate = Template;
    //store string id of picked group
    this->mTemplateStr = Template->str_path;
    
    unsigned int bytesize = 0;

    if (engine_ptr->engine_info->graphicsApi == VULKAN && Template->mShader->mCreated) {
        VKMaterialTemplate* VKTemplate = static_cast<VKMaterialTemplate*>(Template);
        ((VKMaterial*)(this))->CreateBuffer(VKTemplate->BufferSize);
       ( (VKMaterial*)(this))->CreateDescriptors();
    }
    
}

Material::Material(){
    setTemplate(MtShProps::getDefaultMtShGroup());
}

Material::Material(std::string shader_group_str){
    setTemplate(MtShProps::getMtShaderPropertyGroup(shader_group_str));
}

Material::Material(MaterialTemplate* Template){
    setTemplate(Template);
}

Material::~Material(){

}

MaterialShaderPropertyConf* Material::AddPropertyConf(int type){
    //Allocate new property
    MaterialShaderPropertyConf* newprop_ptr =
            static_cast<MaterialShaderPropertyConf*>(MtShProps::allocatePropertyConf(type));
    //Push pointer to vector
    this->confs.push_back(newprop_ptr);

    return confs[confs.size() - 1];
}

MaterialShaderPropertyConf* Material::GetPropertyConf(std::string Identifier) {
    for (unsigned int conf_i = 0; conf_i < confs.size(); conf_i++) {
        MaterialShaderPropertyConf* Conf = confs[conf_i];
        if (Conf->mProperty == nullptr)
            continue;
        if (Conf->mProperty->mPropId.compare(Identifier) == 0) {
            return Conf;
        }
    }
    return nullptr;
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

            setTemplate(MtShProps::getMtShaderPropertyGroup(group_name));
        }

        if (strcmp(prefix, "_ENTRY") == 0) { //if it is game object
            position += 7;
            std::string prop_identifier;
            readString(prop_identifier, buffer, position);

            for (unsigned int prop_i = 0; prop_i < mTemplate->properties.size(); prop_i++) {
                MaterialShaderProperty* prop_ptr = mTemplate->properties[prop_i];
                MaterialShaderPropertyConf* conf_ptr = this->confs[prop_i];
                //check if compare
                if (prop_identifier.compare(prop_ptr->mPropId) == 0) {
                    switch (prop_ptr->mType) {
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
                        readBinaryValue(&ivec2_conf->value.X, buffer + position, position);
                        readBinaryValue(&ivec2_conf->value.Y, buffer + position, position);

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

void Material::saveToFile() {
    //if it is a default material, then quit function
    if (file_path[0] == '@') return;

    ZsStream mat_stream;
    mat_stream.open(file_path, std::ofstream::out);

    if (!mat_stream.is_open()) { //File isn't opened
        std::cout << "Error opening output stream " << file_path;
        return;
    }

    //Write material header
    mat_stream << "ZSP_MATERIAL\n";
    //Write group string
    mat_stream << "_GROUP "; //write group header
    mat_stream.writeString(this->mTemplateStr); //write group label

    for (unsigned int prop_i = 0; prop_i < mTemplate->properties.size(); prop_i++) {
        //Obtain pointers to prop and prop's configuration
        MaterialShaderProperty* prop_ptr = mTemplate->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = this->confs[prop_i];
        //write entry header
        mat_stream << "_ENTRY ";
        mat_stream.writeString(prop_ptr->mPropId); //Write identifier

        switch (prop_ptr->mType) {
            case MATSHPROP_TYPE_NONE: {
                break;
            }
            case MATSHPROP_TYPE_TEXTURE: {
                //Cast pointer
                TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeString(texture_conf->path);
                break;
            }
            case MATSHPROP_TYPE_FLOAT: {
                //Cast pointer
                FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeBinaryValue(&float_conf->value);
                break;
            }
            case MATSHPROP_TYPE_INTEGER: {
                //Cast pointer
                IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeBinaryValue(&int_conf->value);
                break;
            }
            case MATSHPROP_TYPE_COLOR: {
                //Cast pointer
                ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeBinaryValue(&color_conf->color.r);
                mat_stream.writeBinaryValue(&color_conf->color.g);
                mat_stream.writeBinaryValue(&color_conf->color.b);
                break;
            }
            case MATSHPROP_TYPE_FVEC3: {
                //Cast pointer
                Float3MtShPropConf* fvec3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeBinaryValue(&fvec3_conf->value.X);
                mat_stream.writeBinaryValue(&fvec3_conf->value.Y);
                mat_stream.writeBinaryValue(&fvec3_conf->value.Z);
                break;
            }
            case MATSHPROP_TYPE_FVEC2: {
                //Cast pointer
                Float2MtShPropConf* fvec2_conf = static_cast<Float2MtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeBinaryValue(&fvec2_conf->value.X);
                mat_stream.writeBinaryValue(&fvec2_conf->value.Y);
                break;
            }
            case MATSHPROP_TYPE_IVEC2: {
                //Cast pointer
                Int2MtShPropConf* ivec2_conf = static_cast<Int2MtShPropConf*>(conf_ptr);
                //Write value
                mat_stream.writeBinaryValue(&ivec2_conf->value.X);
                mat_stream.writeBinaryValue(&ivec2_conf->value.Y);
                break;
            }
            case MATSHPROP_TYPE_TEXTURE3: {
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


void Material::applyMatToPipeline() {
    if (this->mTemplate->mShader == nullptr)
        return;
    this->mTemplate->mShader->Use();

    //iterate over all properties, send them all!
    for (unsigned int prop_i = 0; prop_i < mTemplate->properties.size(); prop_i++) {
        MaterialShaderProperty* prop_ptr = mTemplate->properties[prop_i];
        MaterialShaderPropertyConf* conf_ptr = confs[prop_i];
        switch (prop_ptr->mType) {
        case MATSHPROP_TYPE_NONE: {
            break;
        }
        case MATSHPROP_TYPE_TEXTURE: {
            //Cast pointer
            TextureMaterialShaderProperty* texture_p = static_cast<TextureMaterialShaderProperty*>(prop_ptr);
            TextureMtShPropConf* texture_conf = static_cast<TextureMtShPropConf*>(conf_ptr);

            int db = 0;
            //If correct path is set to texture
            if (texture_conf->path.compare("@none") != 0) {
                //if texture isn't loaded
                if (texture_conf->texture == nullptr) {
                    //Try to find texture
                    texture_conf->texture = game_data->resources->getTextureByLabel(texture_conf->path);
                    if (texture_conf->texture == nullptr) break; //No resource with that name, exiting
                }
                //Set opengl texture
                db = 1;
                Engine::TextureResource* tex_ptr = static_cast<Engine::TextureResource*>(texture_conf->texture);
                tex_ptr->Use(texture_p->slotToBind);
                if (engine_ptr->engine_info->graphicsApi == VULKAN) {
                    ((VKMaterial*)(this))->SetTexture(texture_p->slotToBind, ((Engine::vkTexture*)tex_ptr->texture_ptr)->GetImageView(), game_data->vk_main->mDefaultTextureSampler);
                }
            }
            
            //Set texture state
            WriteBytes(texture_p->start_offset, 4, &db);

            break;
        }
        case MATSHPROP_TYPE_FLOAT: {
            FloatMtShPropConf* float_conf = static_cast<FloatMtShPropConf*>(conf_ptr);

            //set float to buffer
            WriteBytes(prop_ptr->start_offset, 4, &float_conf->value);

            break;
        }
        case MATSHPROP_TYPE_INTEGER: {
            IntegerMtShPropConf* int_conf = static_cast<IntegerMtShPropConf*>(conf_ptr);

            //set integer to buffer
            WriteBytes(prop_ptr->start_offset, 4, &int_conf->value);

            break;
        }
        case MATSHPROP_TYPE_COLOR: {
            ColorMtShPropConf* color_conf = static_cast<ColorMtShPropConf*>(conf_ptr);

            color_conf->color.updateGL();

            //Write color to buffer
            WriteBytes(prop_ptr->start_offset, 4, &color_conf->color.gl_r);
            WriteBytes(prop_ptr->start_offset + 4, 4, &color_conf->color.gl_g);
            WriteBytes(prop_ptr->start_offset + 8, 4, &color_conf->color.gl_b);


            break;
        }
        case MATSHPROP_TYPE_FVEC3: {
            Float3MtShPropConf* fvec3_conf = static_cast<Float3MtShPropConf*>(conf_ptr);

            //Write vec3 to buffer
            WriteBytes(prop_ptr->start_offset, 4, &fvec3_conf->value.X);
            WriteBytes(prop_ptr->start_offset + 4, 4, &fvec3_conf->value.Y);
            WriteBytes(prop_ptr->start_offset + 8, 4, &fvec3_conf->value.Z);

            break;
        }
        case MATSHPROP_TYPE_FVEC2: {
            Float2MtShPropConf* fvec2_conf = static_cast<Float2MtShPropConf*>(conf_ptr);

            //Write vec2 to buffer
            WriteBytes(prop_ptr->start_offset, 4, &fvec2_conf->value.X);
            WriteBytes(prop_ptr->start_offset + 4, 4, &fvec2_conf->value.Y);
            break;
        }
        case MATSHPROP_TYPE_IVEC2: {
            Int2MtShPropConf* ivec2_conf = static_cast<Int2MtShPropConf*>(conf_ptr);

            //Write vec2 to buffer
            WriteBytes(prop_ptr->start_offset, 4, &ivec2_conf->value.X);
            WriteBytes(prop_ptr->start_offset + 4, 4, &ivec2_conf->value.Y);
            break;
        }
        case MATSHPROP_TYPE_TEXTURE3: {
            //Cast pointer
            Texture3MaterialShaderProperty* texture_p = static_cast<Texture3MaterialShaderProperty*>(prop_ptr);
            Texture3MtShPropConf* texture_conf = static_cast<Texture3MtShPropConf*>(conf_ptr);
            //Check, is texture already created
            if (!texture_conf->texture3D->mCreated) {
                //If not created, create OpenGL instance
                texture_conf->texture3D->Init();

                for (int i = 0; i < 6; i++) {
                    texture_conf->texture3D->pushTexture(i, texture_conf->texture_str[i]);
                }
                //Change Flag state
                texture_conf->texture3D->mCreated = true;
                texture_conf->texture3D->Use(texture_p->slotToBind);
            }
            else {
                //3D texture already created, Use it
                texture_conf->texture3D->Use(texture_p->slotToBind);

            }
            break;
        }
        }
    }
        if (engine_ptr->engine_info->graphicsApi == OGL) {
            game_data->pipeline->GetMaterialsUniformBuffer()->bind();
            game_data->pipeline->GetMaterialsUniformBuffer()->updateBufferedData();
        }
}