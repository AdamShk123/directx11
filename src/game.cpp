#include "./game.hpp"

namespace Game {

Game::Game()
{
    IDXGIFactory* factory = nullptr;

    createWindow();
    createFactory(factory);
    auto bestAdapter = findBestAdapter(factory);
    createDevice(bestAdapter);

    #if defined(_DEBUG)
    DX::ThrowIfFailed(m_device->QueryInterface(IID_PPV_ARGS(&m_debugController)));
    #endif // _DEBUG

    createSwapChain(factory);
   
    factory->Release();
}

Game::~Game() 
{
    m_device->Release();
    m_deviceContext->Release();
    m_swapChain->Release();
    m_renderTargetView->Release();

    #if defined(_DEBUG)
    m_debugController->Release();
    #endif
        
    SDL_DestroyWindow(m_window);
    m_window = nullptr;

    SDL_Quit();
}

void Game::run() 
{
    ID3D11Buffer* vertexBuffer = nullptr;

    D3D11_BUFFER_DESC vertexBufferDesc{};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 3;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData{};
    vertexData.pSysMem = vertexBufferData.data();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    auto createVertexBufferResult = m_device->CreateBuffer(
        &vertexBufferDesc,
        &vertexData,
        &vertexBuffer
    );

    DX::ThrowIfFailed(createVertexBufferResult);

    ID3D11Buffer* indexBuffer = nullptr;

    D3D11_BUFFER_DESC indexBufferDesc{};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * 3;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexData{};
    indexData.pSysMem = indexBufferData.data();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    auto createIndexBufferResult = m_device->CreateBuffer(
        &indexBufferDesc,
        &indexData,
        &indexBuffer
    );

    DX::ThrowIfFailed(createIndexBufferResult);

    ID3D11VertexShader* vertexShader = nullptr;
    ID3DBlob* vertexShaderBlob = nullptr;
    ID3DBlob* errors = nullptr;

    #if defined(_DEBUG)
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
    #else
    UINT compileFlags = 0;
    #endif

    std::wstring vertPath{ L"./vertex.hlsl" };

    auto compileVertexShaderResult = D3DCompileFromFile(
        vertPath.c_str(), 
        nullptr, 
        nullptr, 
        "main",
        "vs_5_0", 
        compileFlags, 
        0, 
        &vertexShaderBlob,
        &errors
    );

    try 
    {
        DX::ThrowIfFailed(compileVertexShaderResult);
    }
    catch (DX::com_exception e) 
    {
        std::println("erorrrrr: {}", (const char*)errors->GetBufferPointer());
    }

    auto createVertexShaderResult = m_device->CreateVertexShader(
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        nullptr,
        &vertexShader
    );

    DX::ThrowIfFailed(createVertexShaderResult);

    ID3D11PixelShader* pixelShader = nullptr;
    ID3DBlob* pixelShaderBlob = nullptr;
    //ID3DBlob* errors = nullptr;

    std::wstring pixPath{ L"./pixel.hlsl" };

    auto compilePixelShaderResult = D3DCompileFromFile(
        pixPath.c_str(),
        nullptr,
        nullptr,
        "main",
        "ps_5_0",
        compileFlags,
        0,
        &pixelShaderBlob,
        &errors
    );

    DX::ThrowIfFailed(compilePixelShaderResult);

    auto createPixelShaderResult = m_device->CreatePixelShader(
        pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize(),
        nullptr,
        &pixelShader
    );

    DX::ThrowIfFailed(createPixelShaderResult);

    ID3D11InputLayout* vertexLayout = nullptr;

    // Define the input layout
    std::array<D3D11_INPUT_ELEMENT_DESC,2> layout 
    {{
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    }};

    // Create the input layout
    auto createInputLayout = m_device->CreateInputLayout(
        layout.data(), 
        layout.size(), 
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(), 
        &vertexLayout
    );

    DX::ThrowIfFailed(createInputLayout);

    m_deviceContext->IASetInputLayout(vertexLayout);

    // Set vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    m_deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set primitive topology
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //vertexBuffer->Release();
    //indexBuffer->Release();

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

        m_deviceContext->ClearRenderTargetView(m_renderTargetView, DirectX::Colors::MidnightBlue);

        m_deviceContext->VSSetShader(vertexShader, nullptr, 0);
        m_deviceContext->PSSetShader(pixelShader, nullptr, 0);

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

void Game::createFactory(IDXGIFactory*& factory)
{
    auto createFactoryResult = CreateDXGIFactory(IID_PPV_ARGS(&factory));

    DX::ThrowIfFailed(createFactoryResult);
}

IDXGIAdapter* Game::findBestAdapter(IDXGIFactory* factory) noexcept
{
    IDXGIAdapter* adapter = nullptr;
    IDXGIAdapter* bestAdapter = nullptr;

    size_t bestAdapterMemory = 0;

    int numAdapters = 0;

    while(true) 
    {
        if (FAILED(factory->EnumAdapters(numAdapters, &adapter))) break;

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

void Game::createSwapChain(IDXGIFactory* factory)
{
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(m_window, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
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

    auto createSwapChainResult = factory->CreateSwapChain(
        m_device,
        &swapChainDesc,
        &m_swapChain
    );

    DX::ThrowIfFailed(createSwapChainResult);

    ID3D11Texture2D* backBuffer = nullptr;
    auto getBufferResult = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

    DX::ThrowIfFailed(getBufferResult);

    auto createRenderTargetViewResult = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);
    backBuffer->Release();

    DX::ThrowIfFailed(createRenderTargetViewResult);

    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

    D3D11_VIEWPORT vp{};
    vp.Width = static_cast<float>(SCREEN_WIDTH);
    vp.Height = static_cast<float>(SCREEN_HEIGHT);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    m_deviceContext->RSSetViewports(1, &vp);
}

}