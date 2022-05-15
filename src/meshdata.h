#ifndef MESHDATA_H
#define MESHDATA_H

#include <glm/glm.hpp>
#include <vector>

typedef uint32_t VertexIndexUintFormat;
typedef std::vector<glm::vec3> VertArray;
typedef std::vector<glm::uvec3> TriArray;
typedef std::vector<VertexIndexUintFormat> IndexArray;

struct MeshData {
    enum class ParametricType { PlaneZ, CylindricalNormalCube };

    MeshData(ParametricType type);

    const VertArray& getVertices() const { return m_vertices; }
    const VertArray& getNormals() const { return m_normals; }
    const IndexArray& getIndices() const { return m_indices; }

private:
    VertArray m_vertices;
    VertArray m_normals;
    IndexArray m_indices;
};

#endif // MESHDATA_H
