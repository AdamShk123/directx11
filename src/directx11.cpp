#include "./directx11.hpp"

using namespace DirectX::SimpleMath;

int main(int argc, char* argv[])
{
    fmt::println("Hello World!");

    const auto initResult = SDL_Init(SDL_INIT_VIDEO);

    if (initResult < 0) 
    {
        throw std::runtime_error(SDL_GetError());
    }

    const auto windowFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    SDL_Window* window = SDL_CreateWindow(
        "DirectX 11", 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        800, 
        600, 
        windowFlags
    );

    Vector3 vec { 0.0f, 0.0f, 0.0f };
    fmt::println("vec: {}", vec.x);

    ID3D11Device* device = nullptr;
        
    auto createDeviceResult = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_SINGLETHREADED,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &device,
        nullptr,
        nullptr
    );

    fmt::println("createDeviceResult: {}", createDeviceResult);
    
    //ID3D11Device::GetImmediateContext

    SDL_Event e;
    bool quit = false;

    while(!quit) 
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
                switch(e.key.keysym.sym) 
                {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
                break;
            }
        }
    }

    SDL_DestroyWindow(window);
    window = nullptr;
        
    SDL_Quit();

    return 0;
}