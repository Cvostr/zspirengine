#include "../../headers/render/zs-shader.h"
//#include "../World/headers/World.h"
#include "../../headers/world/go_properties.h"
#include <GL/glew.h>

#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


Engine::Shader::Shader(){
    isCreated = false; //Not created by default
}

void Engine::Shader::Init() {
    this->SHADER_ID = glCreateProgram(); //Call OGL function to create new shader
}

void Engine::Shader::GLcheckCompileErrors(unsigned int shader, const char* type, const char* filepath)
{

	GLint success;
	GLchar infoLog[1024];
	if (strcmp(type, "PROGRAM"))
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);

			if (filepath != nullptr)
				printf("%s :\n", filepath);

			std::cout << type << " Shader compilation error! " << infoLog;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);

			std::cout << "Shader program compilation error! " << infoLog;
		}
	}
}

bool Engine::Shader::readShaderFile(const char* path, char* result) {

	std::string res_data;
	std::ifstream stream;

	stream.exceptions(std::ifstream::badbit);

	try
	{
		stream.open(path);// Open file
		std::stringstream css;
		css << stream.rdbuf();

		stream.close();//close file stream
		res_data = css.str();
		strcpy(result, res_data.c_str());
	}
	catch (std::ifstream::failure e)
	{
		return false;
	}
	return true;
}

bool Engine::Shader::readBinaryShaderFile(std::string path, char* result, int* size){
    std::ifstream stream;

    stream.exceptions(std::ifstream::badbit);

    try
    {
        stream.open(path, std::ifstream::binary | std::ios::ate);// Open file
        size_t fileSize = (size_t) stream.tellg(); //tellg returns size
        stream.seekg(0);
        stream.read(result, fileSize);

        *size = fileSize;

        stream.close();//close file stream
    }
    catch (std::ifstream::failure e)
    {
        return false;
    }
    return true;
}

bool Engine::Shader::compileFromFile(std::string VSpath, std::string FSpath, ZSpireEngine* engine){

    //if opengl
    if(engine->engine_info->graphicsApi == OGL32){

        std::cout << "OGL: Compiling shader " << VSpath << " " << FSpath << std::endl;

        GLchar vs_data[4096];
        GLchar fs_data[4096];

        const GLchar* vs = &vs_data[0];
        const GLchar* fs = &fs_data[0];
        //read shader files
        readShaderFile(VSpath.c_str(), &vs_data[0]);
        readShaderFile(FSpath.c_str(), &fs_data[0]);
        //init opengl shaders
        Init();

        int VS = glCreateShader(GL_VERTEX_SHADER);
        int FS = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(VS, 1, &vs, NULL); //Setting shader code text on vs
        glShaderSource(FS, 1, &fs, NULL); //Setting shader code text on fs

        glCompileShader(VS); //Compile VS shader code
        GLcheckCompileErrors(VS, "VERTEX", VSpath.c_str()); //Check vertex errors
        glCompileShader(FS); //Compile FS shader code
        GLcheckCompileErrors(FS, "FRAGMENT", FSpath.c_str()); //Check fragment compile errors

        glAttachShader(this->SHADER_ID, VS);
        glAttachShader(this->SHADER_ID, FS);

        glLinkProgram(this->SHADER_ID);
        GLcheckCompileErrors(SHADER_ID, "PROGRAM");
        //Clear shaders, we don't need them anymore
        glDeleteShader(VS);
        glDeleteShader(FS);

        Use();
        setGLuniformInt("diffuse", 0);
        setGLuniformInt("normal_map", 1);
        setGLuniformInt("specular_map", 2);
        setGLuniformInt("transparent", 5);

        setGLuniformInt("tDiffuse", 10);
        setGLuniformInt("tNormal", 11);
        setGLuniformInt("tPos", 12);
        setGLuniformInt("tTransparent", 13);

    }
    if(engine->engine_info->graphicsApi == VULKAN){
        std::cout << "VULKAN: Compiling shader " << VSpath << "_vk" << " " << FSpath << "_vk" << std::endl;

        vulkan_shader = new VkShaderBracket; //allocate bracket

        ZsVulkan* vk = engine->getVulkanContext();

        char* VScontent = new char[8192];
        char* FScontent = new char[8192];

        int VS_size, FS_size = 0;

        readBinaryShaderFile(VSpath + "_vk", VScontent, &VS_size);
        readBinaryShaderFile(FSpath + "_vk", FScontent, &FS_size);

        VkShaderModuleCreateInfo createVsInfo = {};
        createVsInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createVsInfo.pNext = nullptr;
        createVsInfo.codeSize = VS_size;
        createVsInfo.pCode = reinterpret_cast<const uint32_t*>(VScontent);
        vkCreateShaderModule(vk->getVkDevice(), &createVsInfo, nullptr, &vulkan_shader->vertexShader);

        VkShaderModuleCreateInfo createFsInfo = {};
        createFsInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createFsInfo.pNext = nullptr;
        createFsInfo.codeSize = FS_size;
        createFsInfo.pCode = reinterpret_cast<const uint32_t*>(FScontent);
        vkCreateShaderModule(vk->getVkDevice(), &createFsInfo, nullptr, &vulkan_shader->fragmentShader);

    }
    this->isCreated = true; //Shader created & compiled now
	return true;

}

void Engine::Shader::Destroy() {
	glDeleteProgram(this->SHADER_ID);
    this->isCreated = false;
}

void Engine::Shader::Use() {
	glUseProgram(this->SHADER_ID);
}

void Engine::Shader::setGLuniformMat4x4(const char* uniform_str, ZSMATRIX4x4 value) {
	unsigned int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
	glUniformMatrix4fv(uniform_id, 1, GL_FALSE, &value.m[0][0]);
}

void Engine::Shader::setTransform(ZSMATRIX4x4 transform){
	setGLuniformMat4x4("object_transform", transform);
}

void Engine::Shader::setCamera(Camera* cam, bool sendPos){
    setGLuniformMat4x4("cam_projection", cam->getProjMatrix());
    setGLuniformMat4x4("cam_view", cam->getViewMatrix());
    if(sendPos)
        setGLuniformVec3("cam_position", cam->getCameraPosition());
}

void Engine::Shader::setGLuniformColor(const char* uniform_str, ZSRGBCOLOR value) {

	unsigned int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
	glUniform3f(uniform_id, value.gl_r, value.gl_g, value.gl_b);

}
void Engine::Shader::setGLuniformFloat(const char* uniform_str, float value) {

	unsigned int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
	glUniform1f(uniform_id, value);

}

void Engine::Shader::setGLuniformVec3(const char* uniform_str, ZSVECTOR3 value){

	unsigned int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
	glUniform3f(uniform_id, value.X, value.Y, value.Z);

}

void Engine::Shader::setGLuniformVec4(const char* uniform_str, ZSVECTOR4 value){

    unsigned int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
    glUniform4f(uniform_id, value.X, value.Y, value.Z, value.W);

}

void Engine::Shader::setGLuniformInt(const char* uniform_str, int value) {

	unsigned int uniform_id = glGetUniformLocation(this->SHADER_ID, uniform_str);
	glUniform1i(uniform_id, value);

}

void Engine::Shader::setHasDiffuseTextureProperty(bool hasDiffuseMap){
    this->setGLuniformInt("hasDiffuseMap", static_cast<int>(hasDiffuseMap));
}

void Engine::Shader::setHasNormalTextureProperty(bool hasNormalMap){
    this->setGLuniformInt("hasNormalMap", static_cast<int>(hasNormalMap));
}

void Engine::Shader::setTextureCountProperty(int tX, int tY) {
	this->setGLuniformInt("textures_x", tX);
	this->setGLuniformInt("textures_y", tY);
}

void Engine::Shader::sendLight(unsigned int index, void* _light){
    LightsourceProperty* light = static_cast<LightsourceProperty*>(_light);
    if (light->light_type > 0) {
        std::string id_s = std::to_string(index);

        std::string type;
        type = "lights[" + id_s + "].type";

        std::string pos;
        pos = "lights[" + id_s + "].pos";

        std::string color;
        color = "lights[" + id_s + "].color";

        std::string dir;
        dir = "lights[" + id_s + "].dir";

        std::string range;
        range = "lights[" + id_s + "].range";

        std::string intensity;
        intensity = "lights[" + id_s + "].intensity";

        //std::string spot_angle;
        //spot_angle = "lights[" + std::to_string(index) + "].spot_angle";

        //std::string spot_oangle;
        //spot_oangle = "lights[" + std::to_string(index) + "].spot_out_angle";

        setGLuniformInt(type.c_str(), static_cast<int>(light->light_type));
        setGLuniformVec3(pos.c_str(), light->transform->abs_translation);
        setGLuniformVec3(dir.c_str(), light->direction);
        setGLuniformFloat(range.c_str(), light->range);
        setGLuniformFloat(intensity.c_str(), light->intensity);
        //setGLuniformFloat(spot_angle.c_str(), light->spot_angle_rad);
        //setGLuniformFloat(spot_oangle.c_str(), light->outrad);
        setGLuniformColor(color.c_str(), light->color);
    }
}

void Engine::Shader::unsetLight(unsigned int index){
    std::string type;
    type = "lights[" + std::to_string(index) + "].type";

    setGLuniformInt(type.c_str(), 0);
}

