#ifndef MESH_ATTRIBUTES_H
#define MESH_ATTRIBUTES_H

#include <cstdint> // uintXX_t
#include <vector>

// clang-format off
enum class MeshAttribFormat : uint8_t {
   Invalid,
    Float1, Float2, Float3, Float4, // don't change order!
     Half1,  Half2,  Half3,  Half4,
    Mat4x4,
    Uint8, Uint16, Uint32,
}; // clang-format on

struct MeshAttribParameters {
    // MeshAttribParameters(const MeshAttribParameters& other) = default;
    MeshAttribParameters(MeshAttribFormat format);
    const MeshAttribFormat format {};
    const char* shaderName {};
    int openGLTypeFormat {};
    uint32_t sizeInBytes {};
    uint8_t vectorSize {};
    bool normalized {};
    bool isFloatVector();
    bool isHalfVector();
};

struct VertexAttribute {
    // clang-format off
    enum class Type : uint8_t { Position, Normal, Tan, BiTan, Color };
    // clang-format on

    VertexAttribute(Type type, MeshAttribFormat format);
    const MeshAttribParameters parameters;
    const Type type;
};

struct VertexAttribData {
    VertexAttribData(const std::vector<VertexAttribute>& attribs);
    VertexAttribData(std::initializer_list<VertexAttribute> attribList);
    const std::vector<VertexAttribute> attributes;
    const uint32_t strideSize;
};

struct IndexAttribData {
    IndexAttribData(MeshAttribFormat format);
    const MeshAttribParameters parameters;
};

struct InstanceAttribData {
    InstanceAttribData(MeshAttribFormat format);
    const MeshAttribParameters parameters;
};

#endif // MESH_ATTRIBUTES_H
