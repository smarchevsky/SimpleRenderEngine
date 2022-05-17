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

        assert(currentAttrib.type <= VertexAttribute::Type::Normal); // only vertices and normals supported

        const glm::vec3* p_currentVector {};
        switch (currentAttrib.type) {
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

        if (currentAttrib.parameters.format == MeshAttribFormat::Float3) {

            assert(sizeof(glm::vec3) == currentAttrib.parameters.sizeInBytes);
            for (uint32_t i_vert = 0; i_vert < vertArraySize; ++i_vert) {
                *(glm::vec3*)currentByteArrayPos = p_currentVector[i_vert];
                currentByteArrayPos += attribStrideSize;
            }

        } else if (currentAttrib.parameters.format == MeshAttribFormat::Half4) {

            for (uint32_t i_vert = 0; i_vert < vertArraySize; ++i_vert) {
                auto packedVec4 = packHalf4x16(glm::vec4(p_currentVector[i_vert], 0.f));
                *(uint64_t*)currentByteArrayPos = packedVec4;
                currentByteArrayPos += attribStrideSize;
            }

        } else
            assert(false); // unsupported

        currentAttribOffset += currentAttrib.parameters.sizeInBytes;
    }
    return byteArray;
}

static ByteArray makePlainIndexByteArray(const VertIndex* vertIndices, size_t indArraySize, IndexAttribData indexAttribute)
{
    ByteArray byteArray;

    switch (indexAttribute.parameters.format) {

    case MeshAttribFormat::Uint8: {
        byteArray.resize(indArraySize * sizeof(uint8_t));
        uint8_t* byteArrayAsUint8 = (uint8_t*)byteArray.data();
        for (int i = 0; i < indArraySize; ++i) {
            byteArrayAsUint8[i] = (uint8_t)vertIndices[i];
        }
    } break;

    case MeshAttribFormat::Uint16: {
        byteArray.resize(indArraySize * sizeof(uint16_t));
        uint16_t* byteArrayAsUint16 = (uint16_t*)byteArray.data();
        for (int i = 0; i < indArraySize; ++i) {
            byteArrayAsUint16[i] = (uint16_t)vertIndices[i];
        }
    } break;

    case MeshAttribFormat::Uint32: {
        byteArray.resize(indArraySize * sizeof(uint32_t));
        assert(sizeof(*vertIndices) == sizeof(uint32_t)); // meshdata indices must be 32bit index

        uint32_t* byteArrayAsUint32 = (uint32_t*)byteArray.data();
        for (int i = 0; i < indArraySize; ++i) {
            byteArrayAsUint32[i] = vertIndices[i];
        }
    } break;

    default:
        assert(false); // invalid vertex index format
    }

    return byteArray;
}

GL_Mesh::GL_Mesh(const MeshData& meshData, VertexAttribData vertAttribData, IndexAttribData indexAttributes)
    : m_GL_IndexFormatType(indexAttributes.parameters.openGLTypeFormat)
{
    m_meshElementArraySize = meshData.getNumIndices();
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);

    auto numVertices = meshData.getNumVertices();
    const ByteArray vertexByteArray = makePlainVertexByteArray(
        meshData.getPositionsPtr(), meshData.getNormalsPtr(), numVertices, vertAttribData);

    if ((indexAttributes.parameters.format == MeshAttribFormat::Uint8 && numVertices > 255)
        || (indexAttributes.parameters.format == MeshAttribFormat::Uint16 && numVertices > 65535)) {
        assert(false); // too many vertices for this type format
    }

    const ByteArray indexByteArray = makePlainIndexByteArray(
        meshData.getIndicesPtr(), meshData.getNumIndices(), indexAttributes);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexByteArray.size(), vertexByteArray.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexByteArray.size(), indexByteArray.data(), GL_STATIC_DRAW);

    createVertexPointerAttrbutes(vertAttribData.attributes);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GL_Mesh::~GL_Mesh()
{
    if (m_VAO) {
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

void GL_Mesh::draw()
{
    if (m_VAO != s_currentlyBindedVAO)
        glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_meshElementArraySize, m_GL_IndexFormatType, 0);
}

GL_InstancedMesh::GL_InstancedMesh(const MeshData& data, VertexAttribData vertexAttributes, IndexAttribData indexAttributes, InstanceAttribData instanceAttributes)
    : GL_Mesh(data, vertexAttributes, indexAttributes)
    , m_instanceAttribData(instanceAttributes)
{
    glGenBuffers(1, &m_IBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_IBO);

    glBindVertexArray(m_VAO);
    size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
}

GL_InstancedMesh::~GL_InstancedMesh()
{
    if (m_IBO)
        glDeleteBuffers(1, &m_IBO);
}

void GL_InstancedMesh::setInstanceTransforms(const std::vector<glm::mat4>& matrices)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_IBO);
    if (m_instanceArraySize != matrices.size()) {
        m_instanceArraySize = matrices.size();
        glBufferData(GL_ARRAY_BUFFER, m_instanceArraySize * sizeof(glm::mat4), matrices.data(), GL_STATIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_instanceArraySize * sizeof(glm::mat4), matrices.data());
    }

    glBindVertexArray(0);
}

void GL_InstancedMesh::draw()
{
    if (m_VAO != s_currentlyBindedVAO)
        glBindVertexArray(m_VAO);
    glDrawElementsInstanced(GL_TRIANGLES, m_meshElementArraySize, m_GL_IndexFormatType, 0, m_instanceArraySize);
}

static_assert(std::is_same<uint32_t, VertIndex>(), "");
static_assert(std::is_same<uint32_t, GLuint>(), "");
static_assert(std::is_same<uint16_t, GLushort>(), "");
