#include "window.h"

#include <SDL2/SDL.h>
#undef main
//#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>
#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

//#define LOG_GAME_CONTROLLERS

static int SDLCALL my_event_filter(void* userdata, SDL_Event* event)
{
    Window* p_window = (Window*)userdata;

    switch (event->type) {
    case SDL_QUIT: // press exit btn
        p_window->closeWindow();
        return 1;
    }
    if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
        const auto& it = p_window->getKeyMap().find((SDL_KeyCode)event->key.keysym.sym,
            (SDL_Keymod)event->key.keysym.mod, event->type == SDL_KEYDOWN);

        if (it != p_window->getKeyMap().getKeyActions().end()) {
            it->second();
            return 1;
        }
    }

    if (event->type == SDL_JOYAXISMOTION) {
        SDL_Joystick* joystick = SDL_JoystickFromInstanceID(event->jaxis.which);
        auto it = p_window->m_joysticks.find(joystick);
        if (it != p_window->m_joysticks.end()) {

            GameControllerData& data = it->second; // TODO redo using gamepad mapping
            bool evaluated = true;
            switch (event->jaxis.axis) {
            case 0: // left
                data.slx_value = event->jaxis.value;
                break;
            case 1:
                data.sly_value = event->jaxis.value;
                break;
            case 2:
                data.tl_value = event->jaxis.value;
                break;
            case 3: // right
                data.srx_value = event->jaxis.value;
                break;
            case 4:
                data.sry_value = event->jaxis.value;
                break;
            case 5:
                data.tr_value = event->jaxis.value;
                break;
            case 6:
                // left and right triggers connected, do nothing
                break;

            default:
                evaluated = false;
            }
            evaluated = evaluated; // fuck never read warning
#ifdef LOG_GAME_CONTROLLERS
            std::cout << "Controller: " << SDL_JoystickName(joystick);
            if (evaluated)
                std::cout
                    << "  Axis: " << (int)event->jaxis.axis
                    << "  StickLeft: " << glm::to_string(glm::ivec2(data.slx_value, data.sly_value))
                    << "  StickRight: " << glm::to_string(glm::ivec2(data.srx_value, data.sry_value))
                    << "  TriggerLeft: " << data.tl_value
                    << "  TriggerRight: " << data.tr_value
                    << std::endl;
            else {
                std::cout
                    << "\t Undefined axis: " << (int)event->jaxis.axis
                    << "\t Value: " << event->jaxis.value
                    << std::endl;
            }
#endif
        } else {
            std::cout << "This joystick not listed before..." << std::endl;
        }
    }

    if (event->type != SDL_POLLSENTINEL) {
        // std::cout << "EventType: " << (int)event->type << "Axis: " << (int)event->caxis.axis << std::endl;
        // std::cout << event->type << std::endl;
    }
    return 0;
}

Window::Window(int width /*= 800*/, int height /*= 600*/)
    : m_width(width)
    , m_height(height)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "SDL2 video subsystem couldn't be initialized. Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    m_window = SDL_CreateWindow("Glad Sample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_width, m_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    gl_context = SDL_GL_CreateContext(m_window);

    m_renderer = SDL_CreateRenderer(m_window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // VSYNC HERE!

    if (m_renderer == nullptr) {
        std::cerr << "SDL2 Renderer couldn't be created. Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    initGamepad();
    SDL_GL_MakeCurrent(m_window, gl_context);
    SDL_SetEventFilter(my_event_filter, this);

    NOW = LAST = SDL_GetPerformanceCounter();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.08, 0.08, 0.1, 1);
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

bool Window::update()
{
    NOW = SDL_GetPerformanceCounter();
    m_deltaTime = (float)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());

    SDL_SetWindowTitle(m_window, std::to_string(1 / m_deltaTime).c_str());

    SDL_Event event;
    while (SDL_PollEvent(&event)) { // poll until all events are handled!
        // decide what to do with this event.
    }

    // if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
    //     SDL_Log("Mouse Button pressed.");

    LAST = NOW;

    SDL_GL_SwapWindow(m_window);
    glClear(GL_COLOR_BUFFER_BIT);
    return m_isRendering;
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
