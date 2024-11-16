#include "./window.hpp"

namespace Renderer 
{

Window::Window(const std::string& title, unsigned int width, unsigned int height)
{
    const auto initResult = SDL_Init(SDL_INIT_VIDEO);

    if (initResult < 0)
    {
        throw std::runtime_error(SDL_GetError());
    }

    constexpr auto windowFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        windowFlags
    );

    if (m_window == nullptr)
    {
        throw std::runtime_error(SDL_GetError());
    }
}

Window::~Window()
{
    SDL_DestroyWindow(m_window);
    m_window = nullptr;

    SDL_Quit();
}

HWND Window::getHandle()
{
    SDL_SysWMinfo wmInfo{};
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(m_window, &wmInfo);
    return wmInfo.info.win.window;
}

}