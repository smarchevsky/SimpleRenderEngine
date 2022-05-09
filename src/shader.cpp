#include "shader.h"

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#include <iostream>

static const char* vertexShaderSource = "#version 330 core\n"
                                        "layout (location = 0) in vec3 aPos;\n"
                                        "uniform vec2 offset;\n"
                                        "void main()\n"
                                        "{\n"
                                        "   gl_Position = vec4(aPos.x + offset.x, aPos.y + offset.y, aPos.z, 1.0);\n"
                                        "}\0";

static const char* fragmentShaderSource = "#version 330 core\n"
                                          "out vec4 FragColor;\n"
                                          "uniform vec3 color = vec3(1.0f, 0.5f, 0.2f);\n"
                                          "void main()\n"
                                          "{\n"
                                          "   FragColor = vec4(color.x, color.y, color.z, 1.0f);\n"
                                          "}\n\0";

Shader::ShaderVariable::ShaderVariable(const Shader& parent, const char* name)
    : parentShader(parent)
    , location(glGetUniformLocation(parentShader.getProgram(), name))
{
}

void Shader::ShaderVariable::set(float var) { glUniform1f(location, var); }
void Shader::ShaderVariable::set(glm::vec2 var) { glUniform2fv(location, 1, &var[0]); }
void Shader::ShaderVariable::set(glm::vec3 var) { glUniform3fv(location, 1, &var[0]); }
void Shader::ShaderVariable::set(glm::vec4 var) { glUniform4fv(location, 1, &var[0]); }

Shader::Shader()
{
    m_shaderProgram = glCreateProgram();
    int vs = createShader(vertexShaderSource, GL_VERTEX_SHADER);
    int fs = createShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    glAttachShader(m_shaderProgram, vs);
    glAttachShader(m_shaderProgram, fs);
    glLinkProgram(m_shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader()
{
    glDeleteProgram(m_shaderProgram);
}

void Shader::bind()
{
    glUseProgram(m_shaderProgram);
}

int Shader::createShader(const char* shaderSource, int shaderType)
{
    int sh = glCreateShader(shaderType);
    glShaderSource(sh, 1, &shaderSource, NULL);
    glCompileShader(sh);
    int success;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        std::string shaderTypeName = shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";
        glGetShaderInfoLog(sh, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << shaderTypeName << "::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    return sh;
}
