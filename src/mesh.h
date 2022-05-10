#ifndef MESH_H
#define MESH_H
#include <cstdint> // uintXX_t
#include <vector>

class Mesh { // max 21845 vert (65536/3) vertices;
public:
    Mesh(const std::vector<float>& vertexArray, const std::vector<uint32_t>& indexArray);
    Mesh(std::vector<float> vertexArray, std::vector<uint16_t> indexArray);
    void draw();

private:
    static uint32_t s_currentlyBindedVAO;
    uint32_t m_VBO {}, m_EBO {}, m_VAO {}, m_meshSize {};

};

#endif // MESH_H
