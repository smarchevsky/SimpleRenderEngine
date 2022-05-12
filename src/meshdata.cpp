#include "meshdata.h"

#include <algorithm>

static float boolToSignedF(bool b) { return b ? 1.f : -1.f; };
static void addQuad(TriArray& triangles, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3)
{
    triangles.insert(triangles.end(), { { p0, p1, p2 }, { p2, p1, p3 } });
};

MeshData::MeshData(ParametricType type)
{
    switch (type) {
    case MeshData::ParametricType::PlaneZ:
        m_vertices = { { -1, -1, 0 }, { 1, -1, 0 }, { -1, 1, 0 }, { 1, 1, 0 } };
        m_normals = { { 0, 0, 1 }, { 0, 0, 1 }, { 0, 0, 1 }, { 0, 0, 1 } };
        m_indices = { 0, 1, 2, 2, 1, 3 };
        break;
    case MeshData::ParametricType::CylindricalNormalCube:
        std::vector<glm::vec3> vertices(8);
        for (int i = 0; i < 8; ++i)
            vertices[i] = glm::vec3(boolToSignedF(i & 4), boolToSignedF(i & 2), boolToSignedF(i & 1));

        std::vector<glm::vec3> normals = vertices;
        std::for_each(normals.begin(), normals.end(),
            [](glm::vec3& v) { return glm::normalize(glm::vec3(v.x, v.y, 0.f)); });

        std::vector<glm::uvec3> triangles;
        addQuad(triangles, 0, 1, 4, 5), addQuad(triangles, 1, 3, 5, 7);
        addQuad(triangles, 3, 2, 7, 6), addQuad(triangles, 2, 0, 6, 4);
        addQuad(triangles, 4, 5, 6, 7), addQuad(triangles, 0, 1, 2, 3); /* top/bottom */

        m_vertices = std::move(vertices);
        m_normals = std::move(normals);
        m_indices.resize(triangles.size() * 3);
        break;
    };
}
