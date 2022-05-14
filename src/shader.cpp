#include "shader.h"

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <string>

static const char* vertexShaderSource
    = "#version 330 core\n"

      "layout (location = 0) in vec3 vertexPosition;   \n"
      "layout (location = 1) in vec3 vertexNormal;   \n"

      "uniform mat4 model;   \n"
      "uniform mat4 view;   \n"
      "uniform mat4 projection;   \n"

      "out VS_OUT {"
      "    vec4 wp;   \n" // world position
      "    vec4 lp;   \n" // local position
      "    vec3 n;   \n" // normal
      "} vs;   \n"

      "void main()"
      "{\n"
      "    vs.lp = vec4(vertexPosition, 1.0f);   \n"
      "    vs.wp = model * vs.lp;   \n"
      "    vec4 cp = view * vs.wp;   \n"
      "    gl_Position  =  projection * cp;   \n"
      "    vs.n = mat3(model) * vertexNormal;   \n"
      "}\0";

static const char* fragmentShaderSource
    = "#version 330 core\n"

      "uniform mat4 model;   \n"
      "uniform mat4 view;   \n"
      "uniform mat4 projection;   \n"
      "uniform vec3 viewPos;   \n"
      "uniform vec3 lightDir = vec3(0, 0, 1);   \n"

      "uniform vec3 diffuseColor;   \n"

      "in VS_OUT {"
      "    vec4 wp;   \n" // world position
      "    vec4 lp;   \n" // local position
      "    vec3 n;    \n" // normal
      "} vs;   \n"

      "layout(location = 0) out vec3 color;   \n"

      "float fresnelSchlick(float cosTheta, float f)"
      "{"
      "    return f + (1.0 - f) * pow(1.0 - cosTheta, 5.0);   \n"
      "}"

      "void main(){"
      "    vec3 nn = normalize(vs.n);   \n"
      "    vec3 viewDir = normalize(viewPos - vs.wp.xyz);   \n"

      "    vec3 lDir = lightDir.rgb;   \n"

      "    float lightDot = clamp(dot(lDir, nn), 0, 1);   \n"
      "    float viewDot = abs(dot(viewDir, nn));   \n"
      "    float spec = -dot(reflect(viewDir, nn), lDir);   \n"
      "    color = vec3(lightDot);   \n"
      "    color = color / (color + vec3(1.0));   \n"
      "    color = vec3(1) - pow(vec3(1) - color, vec3(4));   \n"
      "}\n";

Shader::ShaderVariable::ShaderVariable(const Shader& parent, const char* name)
    : parentShader(parent)
    , location(glGetUniformLocation(parentShader.getProgram(), name))
{
}

void Shader::ShaderVariable::set(float var) { glUniform1f(location, var); }
void Shader::ShaderVariable::set(const glm::vec2& var) { glUniform2fv(location, 1, &var[0]); }
void Shader::ShaderVariable::set(const glm::vec3& var) { glUniform3fv(location, 1, &var[0]); }
void Shader::ShaderVariable::set(const glm::vec4& var) { glUniform4fv(location, 1, &var[0]); }
void Shader::ShaderVariable::set(const glm::mat4& var) { glUniformMatrix4fv(location, 1, GL_FALSE, &var[0][0]); }

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
