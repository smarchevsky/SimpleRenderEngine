#include "mesh.h"

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#include <cassert>

uint32_t Mesh::s_currentlyBindedVAO {};

Mesh::Mesh(const std::vector<float>& vertexArray, const std::vector<uint32_t>& indexArray)
{

    std::vector<GLushort> ushortIndexVec(indexArray.size());
    for (int i = 0; i < indexArray.size(); ++i) {
        assert(indexArray[i] < 65536);
        ushortIndexVec[i] = indexArray[i];
    }
    Mesh(vertexArray, ushortIndexVec);
}

Mesh::Mesh(std::vector<float> vertexArray, std::vector<uint16_t> indexArray)
{

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexArray.size(), vertexArray.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indexArray.size(), indexArray.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO
    // as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::draw()
{
    if (m_VAO != s_currentlyBindedVAO)
        glBindVertexArray(m_VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

static_assert(std::is_same<uint32_t, GLuint>(), "");
static_assert(std::is_same<uint16_t, GLushort>(), "");
