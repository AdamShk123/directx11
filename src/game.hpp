#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <memory>
#include <ranges>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <d3d11.h>
#include <dxgi.h>

#include <directxtk/SimpleMath.h>

#include "./dx.hpp"

namespace Game 
{

constexpr unsigned int SCREEN_WIDTH = 1920;
constexpr unsigned int SCREEN_HEIGHT = 1200;

class Game 
{
public:
	Game();
	~Game();

	void run();
private:
	std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> m_window;

	IDXGIFactory* m_factory = nullptr;
	IDXGIAdapter* m_adapter = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	ID3D11Debug* m_debugController = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
};

}

#endif // GAME_HPP