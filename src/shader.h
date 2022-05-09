#ifndef SHADER_H
#define SHADER_H
#include <glm/glm.hpp>

class Shader {
public:
    struct ShaderVariable {
        ShaderVariable(const Shader& parent, const char* name);
        template <class T>
        void set(const T& var) { }
        void set(glm::vec2 var);
        void set(glm::vec3 var);
        void set(glm::vec4 var);
        void set(float var);

    private:
        const Shader& parentShader;
        int location;
    };

    Shader();
    ~Shader();
    ShaderVariable getVariable(const char* varName) const { return ShaderVariable(*this, varName); }
    int getProgram() const { return m_shaderProgram; }
    void bind();

private:
    int m_shaderProgram = -1;
    int createShader(const char* shaderSource, int shaderType);
};

#endif // SHADER_H
