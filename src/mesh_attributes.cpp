#include "mesh_attributes.h"

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <cassert>

bool MeshAttribParameters::isFloat() { return format >= MeshAttribFormat::Float1 && format <= MeshAttribFormat::Float4; }
bool MeshAttribParameters::isHalf() { return format >= MeshAttribFormat::Half1 && format <= MeshAttribFormat::Half4; }

static MeshAttribParameters calcMeshAttribParameters(MeshAttribFormat format)
{
    MeshAttribParameters params(format);

    int dataSize = 0;
    params.vectorSize = 1;
    if (params.isFloat()) {
        dataSize = sizeof(float);
        params.openGLTypeFormat = GL_FLOAT;
        params.vectorSize = (int)format - (int)MeshAttribFormat::Float1 + 1;

    } else if (params.isHalf()) {
        dataSize = sizeof(float) / 2;
        params.openGLTypeFormat = GL_HALF_FLOAT;
        params.vectorSize = (int)format - (int)MeshAttribFormat::Half1 + 1;

    } else if (format == MeshAttribFormat::Uint8) {
        dataSize = sizeof(uint8_t);
        params.openGLTypeFormat = GL_UNSIGNED_BYTE;

    } else if (format == MeshAttribFormat::Uint16) {
        dataSize = sizeof(uint16_t);
        params.openGLTypeFormat = GL_UNSIGNED_SHORT;

    } else if (format == MeshAttribFormat::Uint32) {
        dataSize = sizeof(uint32_t);
        params.openGLTypeFormat = GL_UNSIGNED_INT;
    }
    params.sizeInBytes = dataSize * params.vectorSize;

    assert(params.format != MeshAttribFormat::Invalid
        && params.openGLTypeFormat != 0
        && params.vectorSize != 0
        && params.sizeInBytes != 0); // invalid mesh data, shame on you!

    return params;
}

MeshAttribParameters::MeshAttribParameters(MeshAttribFormat format)
    : format(format)
{
}

VertexAttribute::VertexAttribute(Type type, MeshAttribFormat format)
    : type(type)
    , parameters(calcMeshAttribParameters(format))
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

IndexAttribData::IndexAttribData(MeshAttribFormat format)
    : parameters(calcMeshAttribParameters(format))
{
}
