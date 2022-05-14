#ifndef WINDOW_H
#define WINDOW_H

#include "keymap.h"
#include <glm/glm.hpp>

typedef void* SDL_GLContext;
struct _SDL_Joystick;

struct GameControllerData {
    int16_t slx_value {}, sly_value {}, srx_value {}, sry_value {};
    int16_t tl_value {}, tr_value {};
};

class Window {
public:
    Window(int width = 800, int height = 600);
    void initGamepad();

    bool update();

    float getDeltaTime() { return m_deltaTime; }
    // glm::vec2 getMousePos();

    void closeWindow() { m_isRendering = false; }
    KeyMap& getKeyMap() { return m_keyMap; }
    ~Window();

    void ErrorMsg(const char* title, const char* msg);
    std::unordered_map<_SDL_Joystick*, GameControllerData> m_joysticks;

private:
    KeyMap m_keyMap;
    struct SDL_Window* m_window;
    struct SDL_Renderer* m_renderer;

    SDL_GLContext gl_context;
    int m_width, m_height;

    uint64_t NOW {}, LAST {};
    float m_deltaTime {};
    bool m_isRendering = true;
};
#endif // WINDOW_H
