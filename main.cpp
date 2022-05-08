#include "window.h"
#include <iostream>
#define LOG(x) std::cout << x << std::endl

int main()
{
    Window window(1000, 1000);

    window.getKeyMap().bindAction(SDLK_ESCAPE, KMOD_NONE, true, [&]() {
        window.closeWindow();
    });

    while (window.update()) {
    }

    return 0;
}
