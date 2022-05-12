#ifndef MESH_H
#define MESH_H

#include <cstdint> // uintXX_t
#include <vector>

class MeshData;

struct VertexAttribute {
    // clang-format off
    enum class Format : uint8_t {
        // dont change order of vector dimensions
        f1 = 0, f2, f3, f4, // float, ONLY F3 SUPPORTED NOW
        h1 = 4, h2, h3, h4, // half
        /* packed and normalized formats here */
    };

    enum class Type : uint8_t { Vertex, Normal, Tan, BiTan, Color };
    struct Parameters { uint16_t sizeInBytes; uint8_t vectorSize; bool normalized {}; };
    // clang-format on
    VertexAttribute(Type type, Format format);

    const Parameters parameters;
    const Format vertAttribFormat;
    const Type vertAttribType;
};

struct VertexAttribData {
    VertexAttribData(const std::vector<VertexAttribute>& attribs);
    VertexAttribData(std::initializer_list<VertexAttribute> attribList);
    const std::vector<VertexAttribute> attributes;
    const uint32_t strideSize;
};

struct IndexAttribData {
    // clang-format off
    enum class Format : uint8_t {
        u8 = 1, u16, u24, u32 };
    // clang-format on
    IndexAttribData(Format format)
        : format(format)
    {
    }
    const Format format;
};

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
