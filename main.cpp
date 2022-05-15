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

int main()
{
    Window window(1000, 1000);

    window.getKeyMap().bindAction(SDLK_ESCAPE, KMOD_NONE, true, [&]() {
        window.closeWindow();
    });

    MeshData mData(MeshData::ParametricType::CylindricalNormalCube);
    VertexAttribData attrib(
        { { VertexAttribute::Type::Position, VertexAttribute::Format::f3 },
            { VertexAttribute::Type::Normal, VertexAttribute::Format::h4 } });

    GL_Mesh mesh(mData, attrib, IndexAttribData::Format::u16);

    Shader shader(attrib);

    auto shaderModel = shader.getVariable("model");
    auto shaderView = shader.getVariable("view");
    auto shaderProjection = shader.getVariable("projection");

    Camera camera;
    glm::mat4 modelMatrix(1); // unit matrix

    float currentTime {};
    while (window.update()) {
        currentTime += window.getDeltaTime();

        auto rotatedVector = glm::rotateZ(glm::vec3(0.f, 5.f, 2.f), currentTime);
        camera.setPos(rotatedVector);

        shader.bind();
        shaderModel.set(modelMatrix);
        shaderView.set(camera.getView());
        shaderProjection.set(camera.getProjection());

        mesh.draw();
    }

    return 0;
}
