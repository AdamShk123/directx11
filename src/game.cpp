#include "./game.hpp"

namespace Game {

Game::Game()
{
    const auto initResult = SDL_Init(SDL_INIT_VIDEO);

    if (initResult < 0)
    {
        throw std::runtime_error(SDL_GetError());
    }

    const auto windowFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    m_window = std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>>(
        SDL_CreateWindow(
            "DirectX 11",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            800,
            600,
            windowFlags
        ),
        SDL_DestroyWindow
    );

    auto createFactoryResult = CreateDXGIFactory(__uuidof(m_factory), (void**)(&m_factory));

    DX::ThrowIfFailed(createFactoryResult);

    auto enumerateAdaptersResult = m_factory->EnumAdapters(0, &m_adapter);

    DX::ThrowIfFailed(enumerateAdaptersResult);

    IDXGIOutput* adapterOutput;
    unsigned int numerator = 0;
    unsigned int denominator = 1;
    DX::ThrowIfFailed(m_adapter->EnumOutputs(0, &adapterOutput));

    unsigned int numModes{};

    DX::ThrowIfFailed(adapterOutput->GetDisplayModeList(
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_ENUM_MODES_INTERLACED,
        &numModes, NULL)
    );

    std::vector<DXGI_MODE_DESC> displayModeList(numModes);

    auto getDisplayModeListResult = adapterOutput->GetDisplayModeList(
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_ENUM_MODES_INTERLACED,
        &numModes, displayModeList.data()
    );

    DX::ThrowIfFailed(getDisplayModeListResult);

    auto filterFunction = [](const DXGI_MODE_DESC& mode)
    {
        return mode.Width == 1920 && mode.Height == 1200;
    };

    auto mode = std::find_if(
        displayModeList.begin(),
        displayModeList.end(),
        filterFunction
    );

    if(mode != displayModeList.end()) 
    {
        numerator = mode->RefreshRate.Numerator;
        denominator = mode->RefreshRate.Denominator;
    }
    else 
    {
        throw std::runtime_error("Failed to find appropriate display mode!");
    }

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

    auto createDeviceResult = D3D11CreateDevice(
        m_adapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        0,
        D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG,
        featureLevelInputs.data(),
        7,
        D3D11_SDK_VERSION,
        &m_device,
        &featureLevelOutputs,
        &m_deviceContext
    );

    DX::ThrowIfFailed(createDeviceResult);

    #if defined(_DEBUG)
    DX::ThrowIfFailed(m_device->QueryInterface(IID_PPV_ARGS(&m_debugController)));
    #endif // _DEBUG

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(m_window.get(), &wmInfo);
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

    auto createSwapChainResult = D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        D3D11_CREATE_DEVICE_DEBUG,
        featureLevelInputs.data(),
        1,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &m_swapChain,
        &m_device,
        &featureLevelOutputs,
        &m_deviceContext
    );

    DX::ThrowIfFailed(createSwapChainResult);
}

Game::~Game() 
{
    m_factory->Release();
    m_adapter->Release();
    m_device->Release();
    m_deviceContext->Release();

    #if defined(_DEBUG)
    m_debugController->Release();
    #endif
        
    //SDL_DestroyWindow(m_window);
    //m_window = nullptr;

    SDL_Quit();
}

void Game::run() 
{
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
    }
}

}