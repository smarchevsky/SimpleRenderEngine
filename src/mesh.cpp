#include "mesh.h"
#include "meshdata.h"

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/packing.hpp>
#include <iostream>

#include <cassert>
uint32_t GL_Mesh::s_currentlyBindedVAO {};

#define LOG(x) std::cout << __FUNCTION__ << ", " << x << std::endl
#define RANGE(x) x.begin(), x.end()
typedef std::vector<uint8_t> ByteArray;

static void createVertexPointerAttrbutes(const VertexAttribData& attributes)
{
    size_t offset = 0;
    for (int i = 0; i < attributes.attributes.size(); ++i) {
        const auto& currentAttrib = attributes.attributes[i];
        if (currentAttrib.parameters.sizeInBytes) {
            glVertexAttribPointer(i, currentAttrib.parameters.vectorSize,
                currentAttrib.parameters.openGLTypeFormat,
                currentAttrib.parameters.normalized ? GL_TRUE : GL_FALSE,
                attributes.strideSize,
                (GLvoid*)offset);
            glEnableVertexAttribArray(i);

            offset += currentAttrib.parameters.sizeInBytes;
        }
    }
}

static ByteArray makePlainVertexByteArray(
    const glm::vec3* positions, const glm::vec3* normals, size_t vertArraySize, VertexAttribData& attribData)
{

    const uint32_t attribStrideSize = attribData.strideSize;
    ByteArray byteArray(vertArraySize * attribStrideSize, 0);

    uint32_t currentAttribOffset = 0;
    for (uint32_t i_attr = 0; i_attr < attribData.attributes.size(); ++i_attr) {

        const auto& currentAttrib = attribData.attributes[i_attr];

        assert(currentAttrib.vertAttribType <= VertexAttribute::Type::Normal); // only vertices and normals supported

        const glm::vec3* p_currentVector {};
        switch (currentAttrib.vertAttribType) {
        case VertexAttribute::Type::Position:
            p_currentVector = positions;
            break;
        case VertexAttribute::Type::Normal:
            p_currentVector = normals;
            break;
        default:
            assert(false); // unsupported
        }

        uint8_t* currentByteArrayPos = byteArray.data() + currentAttribOffset;
        switch (currentAttrib.vertAttribFormat) {
        case VertexAttribute::Format::f3: {
            assert(sizeof(glm::vec3) == currentAttrib.parameters.sizeInBytes);
            for (uint32_t i_vert = 0; i_vert < vertArraySize; ++i_vert) {
                *(glm::vec3*)currentByteArrayPos = p_currentVector[i_vert];
                currentByteArrayPos += attribStrideSize;
            }
        } break;
            //        case VertexAttribute::Format::h3:
            //            break;
        case VertexAttribute::Format::h4: {
            for (uint32_t i_vert = 0; i_vert < vertArraySize; ++i_vert) {
                auto packedVec4 = packHalf4x16(glm::vec4(p_currentVector[i_vert], 0.f));
                *(uint64_t*)currentByteArrayPos = packedVec4;
                currentByteArrayPos += attribStrideSize;
            }
        } break;
        default:
            assert(false); // unsupported
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

static ByteArray makePlainIndexByteArray(const VertIndex* vertIndices, size_t indArraySize, IndexAttribData indexAttribute)
{
    ByteArray byteArray;

    switch (indexAttribute.format) {

    case IndexAttribData::Format::u8: {
        byteArray.resize(indArraySize * sizeof(uint8_t));
        uint8_t* byteArrayAsUint8 = (uint8_t*)byteArray.data();
        for (int i = 0; i < indArraySize; ++i) {
            byteArrayAsUint8[i] = (uint8_t)vertIndices[i];
        }
    } break;

    case IndexAttribData::Format::u16: {
        byteArray.resize(indArraySize * sizeof(uint16_t));
        uint16_t* byteArrayAsUint16 = (uint16_t*)byteArray.data();
        for (int i = 0; i < indArraySize; ++i) {
            byteArrayAsUint16[i] = (uint16_t)vertIndices[i];
        }
    } break;

    case IndexAttribData::Format::u32:
        byteArray.resize(indArraySize * sizeof(uint32_t));
        assert(sizeof(*vertIndices) == sizeof(uint32_t)); // meshdata indices must be 32bit index

        uint32_t* byteArrayAsUint32 = (uint32_t*)byteArray.data();
        for (int i = 0; i < indArraySize; ++i) {
            byteArrayAsUint32[i] = vertIndices[i];
        }
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
    switch (indexAttribute.format) {
    case IndexAttribData::Format::u8:
        return GL_UNSIGNED_BYTE;

    case IndexAttribData::Format::u16:
        return GL_UNSIGNED_SHORT;

    case IndexAttribData::Format::u32:
        return GL_UNSIGNED_INT;
    }
    assert(false); // undefined index format
}

GL_Mesh::GL_Mesh(const MeshData& meshData, VertexAttribData vertAttribData, IndexAttribData indexAttribData)
    : m_GL_IndexFormatType(getGLIndexFormatType(indexAttribData))
{
    m_meshElementArraySize = meshData.getNumIndices();
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);

    auto numVertices = meshData.getNumVertices();
    const ByteArray vertexByteArray = makePlainVertexByteArray(
        meshData.getPositionsPtr(), meshData.getNormalsPtr(), numVertices, vertAttribData);

    if ((indexAttribData.format == IndexAttribData::Format::u8 && numVertices > 255)
        || (indexAttribData.format == IndexAttribData::Format::u16 && numVertices > 65535)) {
        assert(false); // too many vertices for this type format
    }

    const ByteArray indexByteArray = makePlainIndexByteArray(
        meshData.getIndicesPtr(), meshData.getNumIndices(), indexAttribData);

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

static_assert(std::is_same<uint32_t, VertIndex>(), "");
static_assert(std::is_same<uint32_t, GLuint>(), "");
static_assert(std::is_same<uint16_t, GLushort>(), "");
