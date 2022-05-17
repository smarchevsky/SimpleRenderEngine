#ifndef MESH_H
#define MESH_H
#include "mesh_attributes.h"

#include <cstdint> // uintXX_t
#include <glm/glm.hpp>
#include <vector>

class MeshData;

class GL_Mesh { // max 21845 vert (65536/3) vertices;
public:
    GL_Mesh(const MeshData& data,
        VertexAttribData vertexAttributes,
        IndexAttribData indexAttributes);
    virtual ~GL_Mesh();

    virtual void draw();

protected:
    static uint32_t s_currentlyBindedVAO;
    const int m_GL_IndexFormatType;
    uint32_t m_VBO {}, m_EBO {}, m_VAO {};
    uint32_t m_meshElementArraySize {}; // num of indices
};

class GL_InstancedMesh : public GL_Mesh {
public:
    GL_InstancedMesh(const MeshData& data,
        VertexAttribData vertexAttributes,
        IndexAttribData indexAttributes,
        InstanceAttribData instanceAttributes);
    virtual ~GL_InstancedMesh();

    void setInstanceTransforms(const std::vector<glm::mat4>& matrices);
    virtual void draw();

    uint32_t m_IBO {}; // instance buffer object
    uint32_t m_instanceArraySize {}; // num of instances

    const InstanceAttribData m_instanceAttribData;
};
#endif // MESH_H
