#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <iostream>
#include <memory>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

namespace Renderer 
{

class Window
{
public:
	Window(const char* title, unsigned int width, unsigned int height);
	~Window();

	HWND getHandle();
private:
	SDL_Window* m_window;
};

}
#endif // WINDOW_HPP