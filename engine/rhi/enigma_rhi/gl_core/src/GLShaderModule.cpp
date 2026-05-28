#include "../include/GLShaderModule.h"
#include "../include/GLUtilities.h"


void EnigmaRHI::GLShaderModule::Create(const std::string& filename, EnigmaRHI::EShaderType shaderType)
{
    std::string source = ReadShader(filename);
    CreateShaderModule(source.c_str(), shaderType);
}

std::string EnigmaRHI::GLShaderModule::ReadShader(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
        throw std::runtime_error("failed to open file!");

    std::stringstream stream;
    stream << file.rdbuf();

    file.close();

    return stream.str().c_str();
}

void EnigmaRHI::GLShaderModule::CreateShaderModule(const char* shaderCode, EShaderType shaderType)
{
    module = glCreateShader(GLUtilities::ShaderTypeToGL(shaderType));
    glShaderSource(module, 1, &shaderCode, NULL);
    glCompileShader(module);
    CheckCompileErrors(module, std::to_string(shaderType));
}

void EnigmaRHI::GLShaderModule::CheckCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf(
                "ERREUR::SHADER_COMPILATION::%s \n%s \n-- --------------------------------------------------- -- \n",
                type.c_str(),
                infoLog);
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf(
                "ERREUR::PROGRAM_LINKING::%s \n%s \n-- --------------------------------------------------- -- \n",
                type.c_str(),
                infoLog);
        }
    }
}
