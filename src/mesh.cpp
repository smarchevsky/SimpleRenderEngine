#include "mesh.h"
#include "meshdata.h"

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
//#include <SDL2/SDL_opengl_glext.h>
#include <cassert>
#include <glm/glm.hpp>
#include <iostream>
#define LOG(x) std::cout << __FUNCTION__ << ", " << x << std::endl

#include <cstring>

uint32_t GL_Mesh::s_currentlyBindedVAO {};

#define RANGE(x) x.begin(), x.end()
typedef std::vector<uint8_t> ByteArray;

static VertexAttribute::Parameters calcParameters(VertexAttribute::Format format)
{
    VertexAttribute::Parameters params;
    int typeSize = (format >= VertexAttribute::Format::f1 || format <= VertexAttribute::Format::f4) ? sizeof(float)
        : (format >= VertexAttribute::Format::h1 || format <= VertexAttribute::Format::h4)          ? 2
                                                                                                    : 1;
    params.vectorSize = ((int)format & 0b11) + 1;
    params.sizeInBytes = typeSize * params.vectorSize;
    assert(format <= VertexAttribute::Format::h4);
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

static void createVertexPointerAttrbutes(const VertexAttribData& attributes)
{
    size_t offset = 0;
    for (int i = 0; i < attributes.attributes.size(); ++i) {
        const auto& attrib = attributes.attributes[i];
        if (attrib.parameters.sizeInBytes) {
            glVertexAttribPointer(i, attrib.parameters.vectorSize, GL_FLOAT,
                attrib.parameters.normalized ? GL_TRUE : GL_FALSE, attributes.strideSize, (GLvoid*)offset);
            glEnableVertexAttribArray(i);

            offset += attrib.parameters.sizeInBytes;
        }
    }
}

static ByteArray makePlainVertexByteArray(const MeshData& meshData,
    VertexAttribData& attribData)
{
    size_t vertArrSize = meshData.getVertices().size();
    assert(vertArrSize == meshData.getNormals().size());

    const uint32_t attribStrideSize = attribData.strideSize;
    ByteArray byteArray(vertArrSize * attribStrideSize, 0);

    uint32_t currentAttribOffset = 0;
    for (uint32_t i_attr = 0; i_attr < attribData.attributes.size(); ++i_attr) {

        const auto& currentAttrib = attribData.attributes[i_attr];
        assert(sizeof(glm::vec3) == currentAttrib.parameters.sizeInBytes); // only float vec3 supported

        assert(currentAttrib.vertAttribType <= VertexAttribute::Type::Normal); // only vertices and normals supported
        const std::vector<glm::vec3>& currentVector
            = (currentAttrib.vertAttribType == VertexAttribute::Type::Vertex) ? meshData.getVertices()
                                                                              : meshData.getNormals();

        uint8_t* currentByteArrayPosition = byteArray.data() + currentAttribOffset;
        for (uint32_t i_vert = 0; i_vert < vertArrSize; ++i_vert) {
            const glm::vec3& currentVertex = currentVector[i_vert];
            memcpy(currentByteArrayPosition, &currentVertex, sizeof(glm::vec3));
            currentByteArrayPosition += attribStrideSize;
        }

        currentAttribOffset += currentAttrib.parameters.sizeInBytes;
    }

    //    float* floatArray = (float*)byteArray.data();
    //    int floatArrSize = byteArray.size() / 4;

    //    LOG("floatArrSize: " << floatArrSize);
    //    std::string line;
    //    for (int i = 0; i < floatArrSize; ++i) {
    //        line += std::to_string(floatArray[i]) + " ";
    //    }
    //    LOG(line);
    return byteArray;
}

static ByteArray makePlainIndexByteArray(const MeshData& meshData, IndexAttribData indexAttribute)
{
    ByteArray byteArray;
    switch (indexAttribute.format) {
    case IndexAttribData::Format::u8:
        assert(false); // not supported
        break;
    case IndexAttribData::Format::u16:
        assert(false); // not supported
        break;
    case IndexAttribData::Format::u24:
        assert(false); // not supported
        break;
    case IndexAttribData::Format::u32:
        size_t indArraySize = meshData.getIndices().size() * sizeof(uint32_t);
        byteArray.resize(indArraySize);
        memcpy(byteArray.data(), meshData.getIndices().data(), indArraySize);
        break;
    }

    //    auto* arr = (uint32_t*)byteArray.data();
    //    int arrSize = byteArray.size() / 4;

    //    LOG("indexArrSize: " << arrSize);
    //    std::string line;
    //    for (int i = 0; i < arrSize; ++i) {
    //        line += std::to_string(arr[i]) + " ";
    //    }

    return byteArray;
}
static int getGLIndexFormatType(IndexAttribData indexAttribute)
{
    assert(indexAttribute.format <= IndexAttribData::Format::u32);
    return (indexAttribute.format == IndexAttribData::Format::u8)
        ? GL_UNSIGNED_BYTE
        : (indexAttribute.format == IndexAttribData::Format::u16) ? GL_UNSIGNED_SHORT
        : (indexAttribute.format == IndexAttribData::Format::u24) ? GL_3_BYTES
                                                                  : GL_UNSIGNED_INT;
}

GL_Mesh::GL_Mesh(const MeshData& meshData, VertexAttribData vertAttribData, IndexAttribData indexAttribData)
    : m_GL_IndexFormatType(getGLIndexFormatType(indexAttribData))
{
    m_meshElementArraySize = meshData.getIndices().size();
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);

    const ByteArray vertexByteArray = makePlainVertexByteArray(meshData, vertAttribData);
    const ByteArray indexByteArray = makePlainIndexByteArray(meshData, indexAttribData);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexByteArray.size(), vertexByteArray.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexByteArray.size(), indexByteArray.data(), GL_STATIC_DRAW);

    createVertexPointerAttrbutes(vertAttribData.attributes);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GL_Mesh::draw()
{
    if (m_VAO != s_currentlyBindedVAO)
        glBindVertexArray(m_VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, m_meshElementArraySize, m_GL_IndexFormatType, 0);
}

static_assert(std::is_same<uint32_t, GLuint>(), "");
static_assert(std::is_same<uint16_t, GLushort>(), "");
