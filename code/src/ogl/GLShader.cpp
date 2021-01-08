#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include "../../headers/ogl/GLShader.hpp"

Engine::glShader::glShader(){

}
Engine::glShader::~glShader(){
    Destroy();
}

void Engine::glShader::GLcheckCompileErrors(unsigned int shader, const char* type, const char* filepath){
    GLint success;
    GLchar infoLog[1024];
    if (strcmp(type, "PROGRAM"))
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);

            if (filepath != nullptr)
                printf("%s :\n", filepath);

            std::cout << type << " Shader compilation error! " << infoLog << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);

            std::cout << "Shader program compilation error! " << infoLog << std::endl;
        }
    }
}

bool Engine::glShader::readShaderFile(const char* path, char** result, size_t& size){
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

        char* mem = new char[res_data.size() + 1];
        strcpy(mem, res_data.c_str());
        mem[res_data.size()] = '\0';
        *result = mem;
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}


bool Engine::glShader::compileFromFile(std::string VSpath, std::string FSpath, std::string GSpath) {
    std::cout << "OGL: Compiling shader " << VSpath << " " << FSpath << std::endl;

    GLchar* vs_data;
    GLchar* fs_data;
    GLchar* gs_data = nullptr;

    size_t size;

    readShaderFile(VSpath.c_str(), &vs_data, size);
    readShaderFile(FSpath.c_str(), &fs_data, size);

    if (!GSpath.empty()) {
        readShaderFile(GSpath.c_str(), &gs_data, size);
    }

    bool Result = compileFromStr(vs_data, fs_data, gs_data);

    delete[] vs_data;
    delete[] fs_data;
    if (mStages & HAS_GEOM_SHADER)
        delete[] gs_data;

    return Result;
}
bool Engine::glShader::compileFromStr(const char* _VS, const char* _FS, const char* _GS) {
    if (mCreated)
        return false;

    mStages = HAS_VERT_SHADER | HAS_FRAG_SHADER;

    this->mShaderID = glCreateProgram();
    unsigned int VS = glCreateShader(GL_VERTEX_SHADER);
    unsigned int FS = glCreateShader(GL_FRAGMENT_SHADER);
    unsigned int GS = 0;
    if (_GS != nullptr) {
        GS = glCreateShader(GL_GEOMETRY_SHADER);
        mStages |= HAS_GEOM_SHADER;
    }

    glShaderSource(VS, 1, &_VS, nullptr); //Setting shader code text on vs
    glCompileShader(VS); //Compile VS shader code
    GLcheckCompileErrors(VS, "VERTEX", "VSpath"); //Check vertex errors
    glAttachShader(this->mShaderID, VS);

    glShaderSource(FS, 1, &_FS, nullptr); //Setting shader code text on fs
    glCompileShader(FS); //Compile FS shader code
    GLcheckCompileErrors(FS, "FRAGMENT", "FSpath"); //Check fragment compile errors
    glAttachShader(this->mShaderID, FS);
    
    if(mStages & HAS_GEOM_SHADER) {
        glShaderSource(GS, 1, &_GS, nullptr); //Setting shader code text on gs
        glCompileShader(GS); //Compile GS shader code
        GLcheckCompileErrors(GS, "Geometry", "GSpath"); //Check fragment compile errors
        glAttachShader(this->mShaderID, GS);
    }

    glLinkProgram(this->mShaderID);
    GLcheckCompileErrors(mShaderID, "PROGRAM");
    //Clear shaders, we don't need them anymore
    glDeleteShader(VS);
    glDeleteShader(FS);
    if (mStages & HAS_GEOM_SHADER)
        glDeleteShader(GS);

    this->mCreated = true; //Shader created & compiled now
    return true;
}

bool Engine::glShader::compileComputeFromFile(std::string CSpath) {

    GLchar* cs_data = nullptr;

    size_t size;

    readShaderFile(CSpath.c_str(), &cs_data, size);

    bool Result = compileComputeFromStr(cs_data);
    delete[] cs_data;
    return Result;
}
bool Engine::glShader::compileComputeFromStr(const char* _CS) {
    if (mCreated)
        return false;

    mStages = HAS_COMP_SHADER;

    this->mShaderID = glCreateProgram();
    unsigned int CShader = glCreateShader(GL_COMPUTE_SHADER);

    glShaderSource(CShader, 1, &_CS, nullptr); //Setting shader code text on cs
    glCompileShader(CShader); //Compile CS shader code
    GLcheckCompileErrors(CShader, "Compute", "CSpath"); //Check fragment compile errors
    glAttachShader(mShaderID, CShader);
    glLinkProgram(mShaderID);

    glDeleteShader(CShader);

    mCreated = true;

    return true;
}

void Engine::glShader::Destroy(){
    if (mCreated) {
        glDeleteProgram(this->mShaderID);
        mCreated = false;
    }
}
void Engine::glShader::Use(){
    if (mCreated)
        glUseProgram(this->mShaderID);
}
