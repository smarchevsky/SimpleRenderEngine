#ifndef WINDOW_H
#define WINDOW_H

#include "keymap.h"

#include <glm/glm.hpp>

typedef void* SDL_GLContext;

class Window {
public:
    Window(int width = 800, int height = 600);
    bool update();
    float getDeltaTime() { return m_deltaTime; }
    ~Window();

    glm::vec2 getMousePos();

    void closeWindow() { m_isRendering = false; }
    KeyMap& getKeyMap() { return m_keyMap; }

private:
    KeyMap m_keyMap;
    struct SDL_Window* m_window;
    struct SDL_Renderer* m_renderer;
    SDL_GLContext gl_context;
    int m_width, m_height;
    bool m_isRendering = true;

    float m_deltaTime {};
    uint64_t NOW {}, LAST {};
};
#endif // WINDOW_H
