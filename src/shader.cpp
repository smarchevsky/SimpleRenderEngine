#include "shader.h"

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <string>
#include <unordered_map>

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

static const std::string s_version = "#version 460 core\n";

static const std::string s_vsInOut
    = "VS_OUT {   \n"
      "  vec4 wp; \n" // world position
      "  vec4 lp; \n" // local position
      "  vec3 n;  \n" // normal
      "} vs;      \n";

static std::unordered_map<VertexAttribute::Type, std::string> s_attribNames = {
    { VertexAttribute::Type::Position, "vertexPosition" },
    { VertexAttribute::Type::Normal, "vertexNormal" },
};

// kinda: layout (location = 0) in vec3 vertexPosition;
static std::string s_vecName = "vec";
static std::string generateVertexAtrtributes(const VertexAttribData& vertData)
{
    std::string result;
    for (int i_attrib = 0; i_attrib < vertData.attributes.size(); ++i_attrib) {
        const auto& currentAttrib = vertData.attributes[i_attrib];

        auto it = s_attribNames.find(currentAttrib.vertAttribType);
        if (it == s_attribNames.end())
            assert(false); // unsupported name

        const std::string& currentAttribName = it->second;
        result += "layout (location = " + std::to_string(i_attrib) + ") in "
            + s_vecName + std::to_string(currentAttrib.parameters.vectorSize) + " "
            + currentAttribName + ";\n";
    }
    return result;
}

static std::string commonUniformBlock()
{
    return "uniform mat4 model;      \n"
           "uniform mat4 view;       \n"
           "uniform mat4 projection; \n";
}

static std::string getVertexCode(const VertexAttribData& vertData)
{
    return s_version
        + generateVertexAtrtributes(vertData)

        + commonUniformBlock()

        + "out " + s_vsInOut +

        "void main()"
        "{\n"
        "    vs.lp = vec4(vertexPosition.xyz, 1.0f); \n"
        "    vs.wp = model * vs.lp;                  \n"
        "    vec4 cp = view * vs.wp;                 \n"
        "    gl_Position  =  projection * cp;        \n"
        "    vs.n = mat3(model) * vertexNormal.xyz;  \n"
        "}\0";
}

static std::string getFragmentCode()
{
    return s_version

        + commonUniformBlock()

        + "uniform vec3 viewPos;   \n"
          "uniform vec3 lightDir = vec3(0, 0, 1);   \n"
          "uniform vec3 diffuseColor;   \n"

        + "in " + s_vsInOut +

        "layout(location = 0) out vec3 fragColor;   \n"

        "float fresnelSchlick(float cosTheta, float f)"
        "{"
        "    return f + (1.0 - f) * pow(1.0 - cosTheta, 5.0);   \n"
        "}"

        "void main(){"
        "    vec3 nn = normalize(vs.n);   \n"
        "    vec3 viewDir = normalize(viewPos - vs.wp.xyz);   \n"
        "    vec3 lDir = lightDir.rgb;   \n"

        "    float lightDot = clamp(dot(lDir, nn), 0, 1);   \n"
        "    float viewDot = clamp(dot(viewDir, nn), 0, 1);   \n"
        "    float spec = dot(reflect(viewDir, nn), lDir);   \n"
        "    fragColor = vec3(pow(spec, 4.));   \n"
        //"    fragColor = fragColor / (fragColor + vec3(1.0));   \n"
        //"    fragColor = vec3(1) - pow(vec3(1) - fragColor, vec3(4));   \n"
        "}\n";
}

Shader::Shader(const VertexAttribData& vertData)
{
    m_shaderProgram = glCreateProgram();
    int vs = createShader(getVertexCode(vertData).c_str(), GL_VERTEX_SHADER);
    int fs = createShader(getFragmentCode().c_str(), GL_FRAGMENT_SHADER);
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
