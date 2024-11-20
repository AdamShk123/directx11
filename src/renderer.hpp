#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <iostream>
#include <memory>
#include <ranges>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <d3d11.h>
#include <dxgi.h>
#include <D3DCompiler.h>

#include <DirectXColors.h>

#include <wrl/client.h>

#include <directxtk/SimpleMath.h>

#include "./dx.hpp"
#include "./constants.hpp"
#include "./window.hpp"

namespace Renderer 
{

//constexpr std::array<Vertex, 6> VERTEX_BUFFER_DATA{ {
//	{{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {}},
//	{{1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {}},
//	{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {}},
//	{{-1.0f, 0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}, {}},
//	{{-0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}, {}},
//	{{-0.75f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {}}
//} };
//
//constexpr std::array<unsigned int, 6> INDEX_BUFFER_DATA{
//	0, 1, 2,
//	3, 4, 5
//};

constexpr std::array<Vertex, 16> VERTEX_BUFFER_DATA{ {
	//{{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	//{{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
	//{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	//{{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	//{{-2.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	//{{-2.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
	//{{0.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	//{{0.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
	{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{-1.0f, -1.0f, -2.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{-1.0f, 1.0f, -2.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
	{{1.0f, 1.0f, -2.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{1.0f, -1.0f, -2.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{-1.0f, -1.0f, -2.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
	{{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-1.0f, 1.0f, -2.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{1.0f, -1.0f, -2.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
	{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },
	{{1.0f, 1.0f, -2.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
} };

constexpr std::array<unsigned int, 24> INDEX_BUFFER_DATA{
	0, 1, 2,
	2, 3, 0,
	4, 5, 6,
	6, 7, 4,
	8, 9, 10,
	10, 11, 8,
	12, 13, 14,
	14, 15, 12
};

typedef DirectX::SimpleMath::Matrix Matrix;
typedef DirectX::SimpleMath::Vector4 Vector4;
typedef DirectX::SimpleMath::Vector3 Vector3;
typedef DirectX::SimpleMath::Vector2 Vector2;

struct ConstantBuffer
{
	Matrix projection;
	Matrix model;
	Matrix view;
};

class Renderer
{
public:
	Renderer(Window& window);
	~Renderer();

	void render(float delta);

	void setCameraPosition(const Vector3& newPosition);
	void setCameraDirection(const Vector3& newPosition);
private:
	void createFactory();
	IDXGIAdapter* findBestAdapter() noexcept;
	void createDevice(IDXGIAdapter* adapter);
	void createSwapChain();
	void createRenderViewTarget();
	void createVertexBuffer();
	void createIndexBuffer();
	void createConstantBuffer();
	void compileVertexShader(const std::wstring& path, unsigned int compileFlags);
	void compilePixelShader(const std::wstring& path, unsigned int compileFlags);
	void createInputLayout();
	void createRasterizerState();
	void createDepthStencilBuffer();
	void createDepthStencilView();
	void createDepthStencilState();
	void populateConstantBufferDataStruct();
	void createSamplerState();
	void createTexture(const std::string& path);
	void createBlendState();

	Window& m_window;

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
	ID3D11SamplerState* m_samplerState = nullptr;
	ID3D11Texture2D* m_texture = nullptr;
	ID3D11ShaderResourceView* m_textureView = nullptr;
	ID3D11BlendState* m_blendState = nullptr;

	ConstantBuffer m_constantBufferData{};

	Vector3 m_cameraPosition{ 0.0f, 0.0f, 2.0f };
	Vector3 m_cameraDirection{ 0.0f, 0.0f, 0.0f };
};

}

#endif // RENDERER_HPP