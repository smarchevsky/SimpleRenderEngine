#ifndef MESHDATA_H
#define MESHDATA_H

#include <glm/glm.hpp>
#include <vector>

typedef uint32_t VertIndex;
typedef glm::uvec3 TriIndex;
typedef glm::vec3 Vec3;

typedef std::vector<Vec3> VertArray;
typedef std::vector<TriIndex> TriArray;
typedef std::vector<VertIndex> IndexArray;

struct MeshData {
    enum class ParametricType {
        PlaneZ,
        CylindricalNormalCube
    };

    MeshData(ParametricType type);

    uint32_t getNumVertices() const
    {
        assert(m_vertices.size() == m_normals.size());
        return m_vertices.size();
    }
    const Vec3* getVerticesPtr() const { return m_vertices.data(); }
    const Vec3* getNormalsPtr() const { return m_normals.data(); }

    VertIndex getNumIndices() const { return m_indices.size(); }
    const VertIndex* getIndicesPtr() const { return m_indices.data(); }

private:
    VertArray m_vertices;
    VertArray m_normals;
    IndexArray m_indices;
};

#endif // MESHDATA_H
