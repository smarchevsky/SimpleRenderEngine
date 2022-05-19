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

        mat = glm::scale(mat, glm::vec3(.2));
        matrices.push_back(mat);
    }
    return matrices;
}

int main()
{
    Window window(1000, 1000, 16);

    window.getKeyMap().bindAction(SDLK_ESCAPE, KMOD_NONE, true, [&]() {
        window.closeWindow();
    });

    MeshData mData(MeshData::ParametricType::CylindricalNormalCube);

    glm::mat4 mjMatrix(1);
    // multiJoint.setMatrix(glm::rotate(mjMatrix, (float)M_PI / 2, { 1, 0, 0 }));

    //    MeshData mData(MeshData::ParametricType::CylindricalNormalCube);

    VertexAttribData attrib(
        { { VertexAttribute::Type::Position, MeshAttribFormat::Float3 },
            { VertexAttribute::Type::Normal, MeshAttribFormat::Half4 } });

    GL_InstancedMesh mesh(mData, attrib, MeshAttribFormat::Uint16, MeshAttribFormat::Mat4x4);
    mesh.setInstanceTransforms(getMatrices());

    window.getKeyMap().bindAction(SDLK_g, KMOD_NONE, true, [&]() {
        mesh.setInstanceTransforms(getMatrices());
    });

    // GL_InstancedMesh mesh(mData, attrib, MeshAttribFormat::Uint16,
    //                        MeshAttribFormat::Mat4x4);

    GL_InstancedMesh sphereMesh(MeshData(MeshData::ParametricType::Sphere, 16),
        attrib, MeshAttribFormat::Uint16, MeshAttribFormat::Mat4x4);

    sphereMesh.setInstanceTransforms(getMatrices());

    window.getKeyMap().bindAction(SDLK_F11, KMOD_NONE, true, [&]() {
        window.setWindowFullScreen(!window.getWindowFullScreen());
    });

    window.getKeyMap().bindAction(SDLK_g, KMOD_NONE, true, [&]() {
        // auto offset0 = multiJoint.addOffset(1, 0, glm::vec2(0, -1));
        // multiJoint.addOffset(2, 1, offset0);
    });

    Shader shader(attrib);

    auto shaderModel = shader.getVariable("model");
    auto shaderView = shader.getVariable("view");
    auto shaderProjection = shader.getVariable("projection");
    auto shaderViewPos = shader.getVariable("viewPos");
    auto diffuseColorPos = shader.getVariable("diffuseColor");

    Camera camera;

    glm::vec2 sceneRot = { 0.2f, 0.2f };
    bool isDirty = true;

    window.RMBDragEvent = [&](int dx, int dy) {
        constexpr float offsetScale = 0.003f;
        sceneRot += glm::vec2(-dx, dy) * offsetScale;
        sceneRot.y = glm::clamp(sceneRot.y, -(float)M_PI_2 + 0.001f, (float)M_PI_2 - 0.001f);
        sceneRot.x = fmodf(sceneRot.x, M_PI * 2);

        auto origin = camera.getAim();
        auto rotatedVector1 = origin + glm::rotateZ(glm::rotateX(glm::vec3(0.f, camera.getDistance(), 0.f), sceneRot.y), sceneRot.x);
        camera.setPos(rotatedVector1);
        isDirty = true;
    };
    window.MMBDragEvent = [&](int dx, int dy) {
        constexpr float offsetScale = 0.001f;
        glm::mat4 invView = glm::inverse(camera.getView());
        glm::vec3 right = invView[0];
        glm::vec3 up = invView[1];
        auto aim = camera.getAim();
        auto pos = camera.getPos();
        float distance = glm::distance(pos, aim);
        auto offset = (-right * (float)dx + up * (float)dy) * offsetScale * distance;
        camera.setPos(pos + offset, false);
        camera.setAim(aim + offset, false);
        camera.updateViewMatrix();
        isDirty = true;
    };
    window.MouseScrollEvent = [&](int dy) {
        float distance = camera.getDistance();
        distance *= powf(0.9f, dy);
        distance = glm::clamp(distance, 0.1f, 1000.f);
        camera.setDistance(distance);
        isDirty = true;
    };
    window.RMBDragEvent(0, 0);
    glm::mat4 modelMatrix(1); // unit matrix

    float currentTime {};

    while (window.update()) {
        if (isDirty) {
            window.clear();
            currentTime += window.getDeltaTime();

            shader.bind();
            shaderModel.set(modelMatrix);
            shaderView.set(camera.getView());
            shaderProjection.set(camera.getProjection());
            shaderViewPos.set(camera.getPos());

            diffuseColorPos.set({ .3f, .8f, .1f });
            sphereMesh.draw();

            diffuseColorPos.set(glm::vec3 { .3f, .3f, .3f });
            //      mesh.draw();
            isDirty = false;
        }
    }

    return 0;
}
