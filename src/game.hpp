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

#include <wrl/client.h>

#include <directxtk/SimpleMath.h>

#include "./dx.hpp"

namespace Game 
{

constexpr unsigned int SCREEN_WIDTH = 800;
constexpr unsigned int SCREEN_HEIGHT = 600;

enum SHADER_TYPE {
	VERTEX_SHADER = 0,
	PIXEL_SHADER = 1
};

struct Vertex
{
	std::array<float,3> position;
	std::array<float, 3> color;
};

typedef DirectX::SimpleMath::Matrix Matrix;
typedef DirectX::SimpleMath::Vector4 Vector4;

struct ConstantBuffer 
{
	Matrix projection;
	Matrix model;
	Matrix view;
};

constexpr std::array<Vertex, 6> vertexBufferData{ {
	{{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
	{{1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	{{-1.0f, 0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}},
	{{-0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}},
	{{-0.75f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}
} };

constexpr std::array<unsigned int, 6> indexBufferData{ 
	0, 1, 2,
	3, 4, 5
};

struct Releaser 
{
	void operator()(IUnknown* p) 
	{
		p->Release();
	}
};

class Game
{
public:
	Game();
	~Game();

	void run();
private:

	void createWindow();
	void createFactory();
	IDXGIAdapter* findBestAdapter() noexcept;
	void createDevice(IDXGIAdapter* adapter);
	void createSwapChain();
	void createRenderViewTarget();
	void createBuffer(ID3D11Buffer*& buffer, D3D11_BIND_FLAG type, void* start, unsigned int size);
	void compileShader(SHADER_TYPE type, const std::wstring& path, unsigned int compileFlags);
	void createInputLayout();
	void createRasterizerState();

	SDL_Window* m_window = nullptr;

	IDXGIFactory* m_factory = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	ID3D11Debug* m_debugController = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	ID3D11Texture2D* m_backBuffer = nullptr;
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	ID3D11Buffer* m_constantBuffer = nullptr;
	ID3D11RasterizerState* m_rasterState = nullptr;
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3DBlob* m_shaderBlob = nullptr;
	ID3DBlob* m_shaderErrors = nullptr;
	ID3D11InputLayout* m_vertexLayout = nullptr;
};

}

#endif // GAME_HPP