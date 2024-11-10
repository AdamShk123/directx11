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
#include <D3DCompiler.h>
#include <directxcolors.h>

#include <directxtk/SimpleMath.h>

#include "./dx.hpp"

namespace Game 
{

constexpr unsigned int SCREEN_WIDTH = 800;
constexpr unsigned int SCREEN_HEIGHT = 600;

struct Vertex
{
	std::array<float,3> position;
	std::array<float, 3> color;
};

constexpr std::array<Vertex, 3> vertexBufferData{ {
	{{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
	{{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
} };

constexpr std::array<unsigned int, 3> indexBufferData{ 0,1,2 };

class Game 
{
public:
	Game();
	~Game();

	void run();
private:

	void createWindow();
	void createFactory(IDXGIFactory*& factory);
	IDXGIAdapter* findBestAdapter(IDXGIFactory* factory) noexcept;
	void createDevice(IDXGIAdapter* adapter);
	void createSwapChain(IDXGIFactory* factory);

	SDL_Window* m_window = nullptr;

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	ID3D11Debug* m_debugController = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
};

}

#endif // GAME_HPP