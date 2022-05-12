#include "mesh.h"
#include "meshdata.h"
#include "shader.h"
#include "window.h"

#include <iostream>
#define LOG(x) std::cout << x << std::endl

#include <glm/gtc/random.hpp>
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

    MeshData mData(MeshData::ParametricType::PlaneZ);
    VertexAttribData attrib(
        { { VertexAttribute::Type::Vertex, VertexAttribute::Format::f3 },
            { VertexAttribute::Type::Normal, VertexAttribute::Format::f3 } });

    GL_Mesh mesh(mData, attrib, IndexAttribData::Format::u32);

    Shader shader;
    auto shaderOffset = shader.getVariable("offset");
    auto shaderColor = shader.getVariable("color");

    float currentTime {};
    while (window.update()) {

        currentTime += window.getDeltaTime();

        shader.bind();
        // apply variables after shader bind, but before draw
        shaderOffset.set(glm::vec2(sin(currentTime), cos(currentTime)) * .6f);
        shaderColor.set(rainbow(currentTime));

        mesh.draw();
    }

    return 0;
}
