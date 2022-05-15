#ifndef VERTEX_ATTRIBUTES_H
#define VERTEX_ATTRIBUTES_H

#include <cstdint> // uintXX_t
#include <vector>

struct VertexAttribute {
    // clang-format off
    enum class Format : uint8_t {
        f1 = 0, f2, f3, f4, // float
        h1 = 4, h2, h3, h4, // half
    }; // Dont change order and number. f3 and h4 are supported

    enum class Type : uint8_t { Position, Normal, Tan, BiTan, Color };
    // clang-format on

    struct Parameters {
        int openGLTypeFormat;
        uint16_t sizeInBytes;
        uint8_t vectorSize;
        bool normalized {};
    };

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
    enum class Format : uint8_t { u8, u16, u32 }; // clang-format on
    IndexAttribData(Format format)
        : format(format)
    {
    }
    const Format format;
};
#endif // VERTEX_ATTRIBUTES_H
