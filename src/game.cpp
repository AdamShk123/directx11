#include "./game.hpp"

namespace Game {

Game::Game()
{

    createWindow();
    createFactory();
    auto bestAdapter = findBestAdapter();
    createDevice(bestAdapter);

    #if defined(_DEBUG)
    DX::ThrowIfFailed(m_device->QueryInterface(IID_PPV_ARGS(&m_debugController)));
    #endif // _DEBUG

    createSwapChain();
    createRenderViewTarget();

    createBuffer(
        m_vertexBuffer,
        D3D11_BIND_VERTEX_BUFFER,
        (void*)vertexBufferData.data(),
        static_cast<unsigned int>(sizeof(Vertex) * vertexBufferData.size())
    );

    createBuffer(
        m_indexBuffer,
        D3D11_BIND_INDEX_BUFFER,
        (void*)indexBufferData.data(),
        static_cast<unsigned int>(sizeof(unsigned int) * indexBufferData.size())
    );

    #if defined(_DEBUG)
    unsigned int compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
    #else
    unsigned int compileFlags = 0;
    #endif

    std::wstring vertexShaderPath{ L"./vertex.hlsl" };

    compileShader(SHADER_TYPE::VERTEX_SHADER, vertexShaderPath, compileFlags);

    createInputLayout();

    std::wstring pixelShaderPath{ L"./pixel.hlsl" };

    compileShader(SHADER_TYPE::PIXEL_SHADER, pixelShaderPath, compileFlags);

    createRasterizerState();
}

Game::~Game() 
{
    if (m_factory) m_factory->Release();
    if (m_device) m_device->Release();
    if (m_deviceContext) m_deviceContext->Release();
    if (m_swapChain) m_swapChain->Release();
    if (m_renderTargetView) m_renderTargetView->Release();
    if (m_backBuffer) m_backBuffer->Release();
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release();
    if (m_rasterState) m_rasterState->Release();
    if (m_vertexShader) m_vertexShader->Release();
    if (m_pixelShader) m_pixelShader->Release();
    if (m_shaderBlob) m_shaderBlob->Release();
    if (m_shaderErrors) m_shaderErrors->Release();
    if (m_vertexLayout) m_vertexLayout->Release();

    #if defined(_DEBUG)
    if(m_debugController) m_debugController->Release();
    #endif
        
    SDL_DestroyWindow(m_window);
    m_window = nullptr;

    SDL_Quit();
}

void Game::run() 
{
    D3D11_VIEWPORT viewport{};
    viewport.Width = static_cast<float>(SCREEN_WIDTH);
    viewport.Height = static_cast<float>(SCREEN_HEIGHT);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    SDL_Event e;
    bool quit = false;

    while (!quit)
    {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            switch (e.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
                break;
            }
        }

        m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

        m_deviceContext->RSSetViewports(1, &viewport);

        m_deviceContext->ClearRenderTargetView(m_renderTargetView, DirectX::Colors::AntiqueWhite);

        m_deviceContext->IASetInputLayout(m_vertexLayout);

        m_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
        m_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

        unsigned int stride = sizeof(Vertex);
        unsigned int offset = 0;
        m_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

        m_deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_deviceContext->DrawIndexed(3, 0, 0);

        m_swapChain->Present(0, 0);
    }
}

void Game::createWindow()
{
    const auto initResult = SDL_Init(SDL_INIT_VIDEO);

    if (initResult < 0)
    {
        throw std::runtime_error(SDL_GetError());
    }

    constexpr auto windowFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    m_window = SDL_CreateWindow(
        "DirectX 11",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        windowFlags
    );

    if (m_window == nullptr)
    {
        throw std::runtime_error(SDL_GetError());
    }
}

void Game::createFactory()
{
    auto createFactoryResult = CreateDXGIFactory(IID_PPV_ARGS(&m_factory));

    DX::ThrowIfFailed(createFactoryResult);
}

IDXGIAdapter* Game::findBestAdapter() noexcept
{
    IDXGIAdapter* adapter = nullptr;
    IDXGIAdapter* bestAdapter = nullptr;

    size_t bestAdapterMemory = 0;

    int numAdapters = 0;

    while(true) 
    {
        if (FAILED(m_factory->EnumAdapters(numAdapters, &adapter))) break;

        numAdapters += 1;

        DXGI_ADAPTER_DESC adapterDesc{};
        if (FAILED(adapter->GetDesc(&adapterDesc))) 
        {
            adapter->Release();
            continue;
        }

        if (adapterDesc.DedicatedVideoMemory > bestAdapterMemory) 
        {
            if (bestAdapter) 
            {
                bestAdapter->Release();
            }

            bestAdapterMemory = adapterDesc.DedicatedVideoMemory;
            bestAdapter = adapter;
        }
        else 
        {
            adapter->Release();
        }
    }

    return bestAdapter;
}

void Game::createDevice(IDXGIAdapter* adapter)
{
    std::array<D3D_FEATURE_LEVEL, 7> featureLevelInputs{
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    D3D_FEATURE_LEVEL featureLevelOutputs = featureLevelInputs[0];

    unsigned int flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

    #if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif // _DEBUG


    auto createDeviceResult = D3D11CreateDevice(
        adapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        0,
        flags,
        featureLevelInputs.data(),
        7,
        D3D11_SDK_VERSION,
        &m_device,
        &featureLevelOutputs,
        &m_deviceContext
    );

    DX::ThrowIfFailed(createDeviceResult);
}

void Game::createSwapChain()
{
    SDL_SysWMinfo wmInfo{};
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(m_window, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;

    DXGI_SWAP_CHAIN_DESC swapChainDesc{};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = SCREEN_WIDTH;
    swapChainDesc.BufferDesc.Height = SCREEN_HEIGHT;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;

    auto createSwapChainResult = m_factory->CreateSwapChain(
        m_device,
        &swapChainDesc,
        &m_swapChain
    );

    DX::ThrowIfFailed(createSwapChainResult);
}

void Game::createRenderViewTarget() 
{
    auto getBufferResult = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&m_backBuffer));

    DX::ThrowIfFailed(getBufferResult);

    auto createRenderTargetViewResult = m_device->CreateRenderTargetView(m_backBuffer, nullptr, &m_renderTargetView);

    DX::ThrowIfFailed(createRenderTargetViewResult);
}

void Game::createBuffer(ID3D11Buffer*& buffer, D3D11_BIND_FLAG type, void* start, unsigned int size)
{
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = size;
    bufferDesc.BindFlags = type;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA bufferData{};
    bufferData.pSysMem = start;
    bufferData.SysMemPitch = 0;
    bufferData.SysMemSlicePitch = 0;

    auto createBufferResult = m_device->CreateBuffer(
        &bufferDesc,
        &bufferData,
        &buffer
    );

    DX::ThrowIfFailed(createBufferResult);
}

void Game::compileShader(SHADER_TYPE type, const std::wstring& path, unsigned int compileFlags)
{
    auto target = type == SHADER_TYPE::VERTEX_SHADER ? "vs_5_0" : "ps_5_0";

    auto compileShaderResult = D3DCompileFromFile(
        path.c_str(),
        nullptr,
        nullptr,
        "main",
        target,
        compileFlags,
        0,
        &m_shaderBlob,
        &m_shaderErrors
    );

    try
    {
        DX::ThrowIfFailed(compileShaderResult);
    }
    catch (DX::com_exception e)
    {
        std::println("erorr: {}", m_shaderErrors->GetBufferPointer());
        throw e;
    }

    HRESULT createShaderResult{};

    if (type == SHADER_TYPE::VERTEX_SHADER) 
    {
        createShaderResult = m_device->CreateVertexShader(
            m_shaderBlob->GetBufferPointer(),
            m_shaderBlob->GetBufferSize(),
            nullptr,
            &m_vertexShader
        );
    }
    else 
    {
        createShaderResult = m_device->CreatePixelShader(
            m_shaderBlob->GetBufferPointer(),
            m_shaderBlob->GetBufferSize(),
            nullptr,
            &m_pixelShader
        );
    }

    DX::ThrowIfFailed(createShaderResult);
}

void Game::createInputLayout()
{
    // Define the input layout
    std::array<D3D11_INPUT_ELEMENT_DESC, 2> layout{};

    layout[0].SemanticName = "POSITION";
    layout[0].SemanticIndex = 0;
    layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    layout[0].InputSlot = 0;
    layout[0].AlignedByteOffset = 0;
    layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout[0].InstanceDataStepRate = 0;

    layout[1].SemanticName = "COLOR";
    layout[1].SemanticIndex = 0;
    layout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    layout[1].InputSlot = 0;
    layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout[1].InstanceDataStepRate = 0;

    // Create the input layout
    auto createInputLayout = m_device->CreateInputLayout(
        layout.data(),
        static_cast<unsigned int>(layout.size()),
        m_shaderBlob->GetBufferPointer(),
        m_shaderBlob->GetBufferSize(),
        &m_vertexLayout
    );

    DX::ThrowIfFailed(createInputLayout);
}

void Game::createRasterizerState()
{
    D3D11_RASTERIZER_DESC rasterDesc{};
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    DX::ThrowIfFailed(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState));
    m_deviceContext->RSSetState(m_rasterState);
}

}