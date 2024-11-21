#include "./game.hpp"

namespace Game 
{

Game::Game(Renderer::Renderer& renderer, IInput& input) : m_renderer(renderer), m_input(input)
{

}

Game::~Game() 
{

}

void Game::run()
{
    bool quit = false;

    unsigned int lastTime{ 0 };
    unsigned int currentTime{ 0 };

    Renderer::Vector3 cameraPosition{ 0.0f, 0.0f, 2.0f };
    Renderer::Vector3 cameraDirection{ 0.0f, 0.0f, 0.0f };

    while (!quit)
    {
        m_input.update();

        if (m_input.getKeyPressed(KEY_ESC))
            quit = true;

        auto mousePosition = m_input.getMousePosition();
        std::println("(x:{}, y:{})", mousePosition.x, mousePosition.y);

        currentTime = static_cast<unsigned int>(SDL_GetTicks());

        m_renderer.render(static_cast<float>(currentTime - lastTime) / 1000.0f);

        lastTime = currentTime;
    }
}

}