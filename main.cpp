#include "mesh.h"
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

    window.getKeyMap().bindAction(SDLK_q, KMOD_LCTRL, true, [&]() {
        window.closeWindow();
    });

    const float x = 0.2f, y = x, z = 0;
    std::vector<float> vertices = { // xyz0, xyz1, xyz2, xyz3
        x, y, z, x, -y, z, -x, -y, z, -x, y, z
    };
    std::vector<unsigned short> indices = {
        /*first: */ 0, 1, 3, /*second: */ 1, 2, 3
    };
    Mesh mesh(vertices, indices);

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
