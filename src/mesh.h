#ifndef MESH_H
#define MESH_H
#include "mesh_attributes.h"

#include <cstdint> // uintXX_t
#include <vector>

class MeshData;



class GL_Mesh { // max 21845 vert (65536/3) vertices;
public:
    GL_Mesh(const MeshData& data,
        VertexAttribData attributes,
        IndexAttribData indexAttribute);
    void draw();

private:
    static uint32_t s_currentlyBindedVAO;
    uint32_t m_VBO {}, m_EBO {}, m_VAO {}, m_meshElementArraySize {};
    const int m_GL_IndexFormatType;
};

#endif // MESH_H
