#include "window.h"

#include <SDL2/SDL.h>
#undef main
//#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>
#include <string>

Window::Window(int width /*= 800*/, int height /*= 600*/)
    : m_width(width)
    , m_height(height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL2 video subsystem couldn't be initialized. Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    m_window = SDL_CreateWindow("Glad Sample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_width, m_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    m_renderer = SDL_CreateRenderer(m_window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // VSYNC HERE!

    if (m_renderer == nullptr) {
        std::cerr << "SDL2 Renderer couldn't be created. Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    gl_context = SDL_GL_CreateContext(m_window);

    //    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    //        std::cerr << "Failed to initialize the OpenGL context." << std::endl;
    //        exit(1);
    //    }

    //    std::cout << "OpenGL version loaded: " << GLVersion.major << "."
    //              << GLVersion.minor << std::endl;

    NOW = LAST = SDL_GetPerformanceCounter();
    LAST = 0;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.08, 0.08, 0.1, 1);
}

bool Window::update()
{
    NOW = SDL_GetPerformanceCounter();
    m_deltaTime = (float)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());

    SDL_SetWindowTitle(m_window, std::to_string(1 / m_deltaTime).c_str());
    SDL_GL_SwapWindow(m_window);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_Event event;
    SDL_PollEvent(&event);

    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        SDL_Log("Mouse Button 1 (left) is pressed.");
    }
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        const auto& it = getKeyMap().find((SDL_KeyCode)event.key.keysym.sym,
            (SDL_Keymod)event.key.keysym.mod, event.type == SDL_KEYDOWN);

        if (it != getKeyMap().getKeyActions().end()) {
            it->second();
        }
    }

    switch (event.type) {
    case SDL_QUIT: // press exit btn
        m_isRendering = false;
        break;
    }

    LAST = NOW;
    return m_isRendering;
}

Window::~Window()
{
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);

    SDL_Quit();
}
