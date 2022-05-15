#include "mesh_attributes.h"

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <cassert>

static VertexAttribute::Parameters calcParameters(VertexAttribute::Format format)
{
    VertexAttribute::Parameters params;
    bool isFloat = (format >= VertexAttribute::Format::f1 && format <= VertexAttribute::Format::f4);
    bool isHalf = (format >= VertexAttribute::Format::h1 && format <= VertexAttribute::Format::h4);
    int typeSize;
    if (isFloat) {
        typeSize = sizeof(float);
        params.openGLTypeFormat = GL_FLOAT;
    } else if (isHalf) {
        typeSize = 2;
        params.openGLTypeFormat = GL_HALF_FLOAT;
    } else
        assert(false); // unsupported

    params.vectorSize = ((int)format & 0b11) + 1;
    params.sizeInBytes = typeSize * params.vectorSize;
    return params;
}

VertexAttribute::VertexAttribute(Type type, Format format)
    : vertAttribType(type)
    , vertAttribFormat(format)
    , parameters(calcParameters(format))
{
}

static uint32_t calcStrideSize(const std::vector<VertexAttribute>& attribs)
{
    uint32_t strideSize = 0;
    for (const auto& a : attribs)
        strideSize += a.parameters.sizeInBytes;
    return strideSize;
}

VertexAttribData::VertexAttribData(const std::vector<VertexAttribute>& attribs)
    : attributes(attribs)
    , strideSize(calcStrideSize(attribs))
{
}

static auto createVectorFromInitializerList(std::initializer_list<VertexAttribute> attribList)
{
    return std::vector<VertexAttribute>(attribList);
}

VertexAttribData::VertexAttribData(std::initializer_list<VertexAttribute> attribList)
    : VertexAttribData(std::move(createVectorFromInitializerList(attribList)))
{
}
