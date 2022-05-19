#include "window.h"

#include <SDL2/SDL.h>
#undef main
//#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>
#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

static int eventWatcher(void* userdata, SDL_Event* event)
{
    Window* p_window = (Window*)userdata;
    switch (event->type) {
    case SDL_QUIT: {
        p_window->closeWindow();
    } break;
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
        if (true || !event->key.repeat) {
            const auto& it = p_window->getKeyMap().find((SDL_KeyCode)event->key.keysym.sym,
                (SDL_Keymod)event->key.keysym.mod, event->type == SDL_KEYDOWN);

            if (it != p_window->getKeyMap().getKeyActions().end()) {
                it->second();
            }
        }
    } break;

    case SDL_MOUSEBUTTONDOWN: {
        // clang-format off
        switch (event->button.button) {
        case 1: { p_window->isLMBDown = true; } break;
        case 2: { p_window->isMMBDown = true; } break;
        case 3: { p_window->isRMBDown = true; } break;
        }
    } break;
    case SDL_MOUSEBUTTONUP: {
        switch (event->button.button) {
        case 1: { p_window->isLMBDown = false; } break;
        case 2: { p_window->isMMBDown = false; } break;
        case 3: { p_window->isRMBDown = false; } break;
        } // clang-format on
    } break;
    case SDL_MOUSEWHEEL: {
        if (p_window->MouseScrollEvent)
            p_window->MouseScrollEvent(event->wheel.preciseY);

    } break;

    case SDL_MOUSEMOTION: {
        // event->button.state
        if (p_window->isLMBDown && p_window->LMBDragEvent)
            p_window->LMBDragEvent(event->motion.xrel, event->motion.yrel);
        if (p_window->isMMBDown && p_window->MMBDragEvent)
            p_window->MMBDragEvent(event->motion.xrel, event->motion.yrel);
        if (p_window->isRMBDown && p_window->RMBDragEvent)
            p_window->RMBDragEvent(event->motion.xrel, event->motion.yrel);

    } break;
    case SDL_WINDOWEVENT: {
        if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
            SDL_Window* win = SDL_GetWindowFromID(event->window.windowID);
            int x {}, y {};
            SDL_GetWindowSize(win, &x, &y);
            glViewport(0, 0, x, y);
        }
    }
    case SDL_POLLSENTINEL: {
    }
    }
    return 1;
}

Window::Window(int width /*= 800*/, int height /*= 600*/, uint8_t multiSampleLevel)
    : m_width(width)
    , m_height(height)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "SDL2 video subsystem couldn't be initialized. Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    if (multiSampleLevel > 1) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, multiSampleLevel);
    }

    m_window = SDL_CreateWindow("Glad Sample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_width, m_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    gl_context = SDL_GL_CreateContext(m_window);

    m_renderer = SDL_CreateRenderer(m_window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // VSYNC HERE!

    if (m_renderer == nullptr) {
        std::cerr << "SDL2 Renderer couldn't be created. Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    initGamepad();
    SDL_GL_MakeCurrent(m_window, gl_context);

    SDL_AddEventWatch(eventWatcher, this);

    NOW = LAST = SDL_GetPerformanceCounter();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.08, 0.08, 0.1, 1);
    glEnable(GL_DEPTH_TEST);
}

void Window::initGamepad()
{
    int nJoysticks = SDL_NumJoysticks();
    int nGameControllers = 0;
    std::string msg;
    for (int i_joystick = 0; i_joystick < nJoysticks; i_joystick++) {
        SDL_Joystick* joystick = SDL_JoystickOpen(i_joystick);
        SDL_GameController* controller = SDL_GameControllerOpen(i_joystick);

        if (joystick)
            std::cout << SDL_JoystickName(joystick) << std::endl;

        if (controller) {
            std::cout << SDL_GameControllerName(controller) << std::endl;
            // std::cout << SDL_GameControllerMapping(controller) << std::endl;
        }

        m_joysticks.insert({ joystick, {} });
        // m_gameControllers.push_back(controller);

        // const char* name = SDL_GameControllerName(controller);
        //  const char* mapping = SDL_GameControllerMapping(controller);

        //        msg += std::string("Name: ") + name + "  \n";
        //        if (SDL_IsGameController(i_joystick)) {
        //            nGameControllers++;
    }
    //}
    // std::cout << msg << std::endl;
    //  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Controllers", msg.c_str(), nullptr);
}
void Window::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Window::update()
{
    NOW = SDL_GetPerformanceCounter();
    m_deltaTime = (float)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());

    m_secondFract += m_deltaTime;
    m_framePerSecCounter++;

    if (m_secondFract > 1.f) {
        SDL_SetWindowTitle(m_window, std::to_string(m_framePerSecCounter / m_secondFract).c_str());
        m_framePerSecCounter = 0;
        m_secondFract = fmodf(m_secondFract, 1.f);
    }

    // if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
    //     SDL_Log("Mouse Button pressed.");

    LAST = NOW;

    SDL_GL_SwapWindow(m_window);
    SDL_Event event;
    while (SDL_PollEvent(&event)) { // poll until all events are handled!
        // decide what to do with this event.
    }

    return m_isRendering;
}

void Window::setWindowFullScreen(bool fullscreen)
{
    SDL_SetWindowFullscreen(m_window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

bool Window::getWindowFullScreen()
{
    return SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
}

Window::~Window()
{
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);

    SDL_Quit();
}

void Window::ErrorMsg(const char* title, const char* msg)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, msg, m_window);
}
