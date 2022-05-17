#include "mesh_attributes.h"

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <cassert>

bool MeshAttribParameters::isFloatVector()
{
    return (format >= MeshAttribFormat::Float1 && format <= MeshAttribFormat::Float4);
}
bool MeshAttribParameters::isHalfVector() { return format >= MeshAttribFormat::Half1 && format <= MeshAttribFormat::Half4; }

static MeshAttribParameters calcMeshAttribParameters(MeshAttribFormat format)
{
    MeshAttribParameters params(format);

    int dataSize = 0;
    params.vectorSize = 0;
    if (params.isFloatVector()) {
        params.openGLTypeFormat = GL_FLOAT;
        dataSize = sizeof(float);
        params.vectorSize = (int)format - (int)MeshAttribFormat::Float1 + 1;

    } else if (params.isHalfVector()) {
        params.openGLTypeFormat = GL_HALF_FLOAT;
        dataSize = sizeof(float) / 2;
        params.vectorSize = (int)format - (int)MeshAttribFormat::Half1 + 1;

    } else if (format == MeshAttribFormat::Uint8) {
        params.openGLTypeFormat = GL_UNSIGNED_BYTE;
        dataSize = sizeof(uint8_t);
        params.vectorSize = 1;

    } else if (format == MeshAttribFormat::Uint16) {
        params.openGLTypeFormat = GL_UNSIGNED_SHORT;
        dataSize = sizeof(uint16_t);
        params.vectorSize = 1;

    } else if (format == MeshAttribFormat::Uint32) {
        params.openGLTypeFormat = GL_UNSIGNED_INT;
        dataSize = sizeof(uint32_t);
        params.vectorSize = 1;

    } else if (format == MeshAttribFormat::Mat4x4) {
        params.openGLTypeFormat = GL_FLOAT;
        dataSize = sizeof(float);
        params.vectorSize = 16;
    }

    params.sizeInBytes = dataSize * params.vectorSize;

    assert(params.format != MeshAttribFormat::Invalid);
    assert(params.openGLTypeFormat != 0);
    assert(params.vectorSize != 0);
    assert(params.sizeInBytes != 0); // invalid mesh data, shame on you!

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

InstanceAttribData::InstanceAttribData(MeshAttribFormat format)
    : parameters(calcMeshAttribParameters(format))
{
}
