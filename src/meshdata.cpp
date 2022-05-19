#include "meshdata.h"

#include <algorithm>

static float boolToSignedF(bool b) { return b ? 1.f : -1.f; };
static void addQuad(TriArray& triangles, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3)
{
    triangles.insert(triangles.end(), { { p0, p1, p2 }, { p2, p1, p3 } });
};

static void triArraytoPlainI32Array(const TriArray& from, IndexArray& to)
{
    to.resize(from.size() * 3);
    for (int i = 0; i < from.size(); ++i)
        ((TriIndex*)to.data())[i] = from[i];
}

//////////// PRIMITIVES /////////////

static void createPlaneZ(VertArray& a_vertices, VertArray& a_normals, IndexArray& a_indices)
{
    a_vertices = { { -1, -1, 0 }, { 1, -1, 0 }, { -1, 1, 0 }, { 1, 1, 0 } };
    a_normals = { { 0, 0, 1 }, { 0, 0, 1 }, { 0, 0, 1 }, { 0, 0, 1 } };
    a_indices = { 0, 1, 2, 2, 1, 3 };
}

static void createSphere(VertArray& a_vertices, VertArray& a_normals, IndexArray& a_indices, uint resolution)
{
#define F(x) static_cast<float>(x)

    const float radius = 1.f;
    const uint numMeridian = resolution * 2; // resolution of mesh
    const uint numParallel = resolution; // resolution of mesh
    float parralelDivider = 2.0f * F(M_PI) / numMeridian;
    float meridianDivider = F(M_PI) / numParallel * .9999f;

    std::vector<glm::vec2> parallel(numMeridian + 1);
    for (uint i = 0; i <= numMeridian; i++)
        parallel[i] = glm::vec2(cos(F(i) * parralelDivider), sin(F(i) * parralelDivider)) * radius; // zero circle

    std::vector<glm::vec3> positions, normals;
    positions.reserve((numParallel + 1) * (numMeridian + 1));
    for (uint j = 0; j <= numParallel; j++)
        for (uint i = 0; i <= numMeridian; i++) {
            auto p = parallel[i] * sinf(F(j) * meridianDivider);
            positions.emplace_back(p, radius * cos(F(j) * meridianDivider));
        }

    normals = positions;

    std::vector<uint> indices;
    for (uint i = 0; i < numParallel; i++)
        for (uint j = 0; j < numMeridian; j++) {
            auto offset__ = (i + 0) * (numMeridian + 1) + j + 0;
            auto offset_j = (i + 0) * (numMeridian + 1) + j + 1;
            auto offset_i = (i + 1) * (numMeridian + 1) + j + 0;
            auto offsetij = (i + 1) * (numMeridian + 1) + j + 1;
            indices.insert(indices.end(), { offset__, offset_i, offset_j });
            indices.insert(indices.end(), { offset_j, offset_i, offsetij });
        }

    // no texcoord supported
    //    std::vector<glm::vec2> texCoords(positions.size());
    //    for (uint i = 0; i <= numMeridian; i++)
    //        for (uint j = 0; j <= numParallel; j++) {
    //            texCoords[j * (numMeridian + 1) + i] = { F(i) / F(numMeridian), F(j) / F(numParallel) };
    //        }

    a_vertices = std::move(positions);
    a_normals = std::move(normals);
    // data.m_uvs = std::move(texCoords);
    a_indices = std::move(indices);
}

static void createCylindricalNormalCube(VertArray& a_vertices, VertArray& a_normals, IndexArray& a_indices)
{
    std::vector<glm::vec3> vertices(8);
    for (int i = 0; i < 8; ++i)
        vertices[i] = glm::vec3(boolToSignedF(i & 4), boolToSignedF(i & 2), boolToSignedF(i & 1));

    std::vector<glm::vec3> normals = vertices;
    for (int i = 0; i < vertices.size(); ++i) {
        auto& v = vertices[i];
        normals[i] = glm::normalize(glm::vec3(v.x, v.y, v.z * 0.01f));
    }

    std::vector<glm::uvec3> triangles;
    addQuad(triangles, 0, 1, 4, 5), addQuad(triangles, 1, 3, 5, 7);
    addQuad(triangles, 3, 2, 7, 6), addQuad(triangles, 2, 0, 6, 4);
    addQuad(triangles, 4, 5, 6, 7), addQuad(triangles, 0, 1, 2, 3); /* top/bottom */

    a_vertices = std::move(vertices);
    a_normals = std::move(normals);
    triArraytoPlainI32Array(triangles, a_indices);
}

/////////////////////////////////////////////

MeshData::MeshData(ParametricType type, uint8_t resolution)
{
    switch (type) {
    case MeshData::ParametricType::PlaneZ: {
        createPlaneZ(m_positons, m_normals, m_indices);
    } break;

    case MeshData::ParametricType::CylindricalNormalCube: {
        createCylindricalNormalCube(m_positons, m_normals, m_indices);
    } break;

    case MeshData::ParametricType::Sphere: {
        createSphere(m_positons, m_normals, m_indices, resolution);
    } break;
    };
}
