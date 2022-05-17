#include "camera.h"
#include "mesh.h"
#include "meshdata.h"
#include "shader.h"
#include "window.h"

#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/random.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <math.h>

glm::vec3 rainbow(float x)
{
    glm::vec3 result(sin(x), sin(x + M_PI / 3), sin(x + 2 * M_PI / 3));
    return result * result;
}

std::vector<glm::mat4> getMatrices()
{
    std::vector<glm::mat4> matrices;
    for (int i = 0; i < 100.f; ++i) {
        glm::mat4 mat(1);
        mat = glm::translate(mat, glm::ballRand(3.f));
        mat = glm::rotate(mat, glm::linearRand<float>(0, 2 * M_PI), glm::sphericalRand(1.f));
        mat = glm::scale(mat, { 0.05f, 0.05f, 5.0f });
        matrices.push_back(mat);
    }
    return matrices;
}

int main()
{
    Window window(1000, 1000);

    window.getKeyMap().bindAction(SDLK_ESCAPE, KMOD_NONE, true, [&]() {
        window.closeWindow();
    });

    MeshData mData(MeshData::ParametricType::CylindricalNormalCube);
    VertexAttribData attrib(
        { { VertexAttribute::Type::Position, MeshAttribFormat::Float3 },
            { VertexAttribute::Type::Normal, MeshAttribFormat::Half4 } });

    GL_InstancedMesh mesh(mData, attrib, MeshAttribFormat::Uint16, MeshAttribFormat::Mat4x4);
    mesh.setInstanceTransforms(getMatrices());

    window.getKeyMap().bindAction(SDLK_g, KMOD_NONE, true, [&]() {
        mesh.setInstanceTransforms(getMatrices());
    });

    Shader shader(attrib);

    auto shaderModel = shader.getVariable("model");
    auto shaderView = shader.getVariable("view");
    auto shaderProjection = shader.getVariable("projection");
    auto shaderViewPos = shader.getVariable("viewPos");

    Camera camera;
    glm::mat4 modelMatrix(1); // unit matrix

    float currentTime {};
    while (window.update()) {
        currentTime += window.getDeltaTime();

        auto rotatedVector = glm::rotateZ(glm::vec3(0.f, 15.f, 6.f), currentTime);
        camera.setPos(rotatedVector);

        shader.bind();
        shaderModel.set(modelMatrix);
        shaderView.set(camera.getView());
        shaderProjection.set(camera.getProjection());
        shaderViewPos.set(camera.getPos());

        mesh.draw();
    }

    return 0;
}
