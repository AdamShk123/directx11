#include "./game.hpp"

namespace Game 
{

Game::Game(Renderer::Renderer& renderer) : m_renderer(renderer), m_input()
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
        //while (SDL_PollEvent(&e)) {
        //    if (e.type == SDL_QUIT) {
        //        quit = true;
        //    }
        //    switch (e.type)
        //    {
        //    case SDL_QUIT:
        //        quit = true;
        //        break;
        //    case SDL_KEYDOWN:
        //        switch (e.key.keysym.sym)
        //        {
        //        case SDLK_ESCAPE:
        //            quit = true;
        //            break;
        //        case SDLK_w:
        //            cameraPosition.z -= 0.1f;
        //            m_renderer.setCameraPosition(cameraPosition);
        //            break;
        //        case SDLK_s:
        //            cameraPosition.z += 0.1f;
        //            m_renderer.setCameraPosition(cameraPosition);
        //            break;
        //        case SDLK_d:
        //            cameraPosition.x += 0.1f;
        //            m_renderer.setCameraPosition(cameraPosition);
        //            break;
        //        case SDLK_a:
        //            cameraPosition.x -= 0.1f;
        //            m_renderer.setCameraPosition(cameraPosition);
        //            break;
        //        case SDLK_UP:
        //            cameraDirection.y += 0.1f;
        //            m_renderer.setCameraDirection(cameraDirection);
        //            break;
        //        case SDLK_DOWN:
        //            cameraDirection.y -= 0.1f;
        //            m_renderer.setCameraDirection(cameraDirection);
        //            break;
        //        case SDLK_LEFT:
        //            cameraDirection.x -= 0.1f;
        //            m_renderer.setCameraDirection(cameraDirection);
        //            break;
        //        case SDLK_RIGHT:
        //            cameraDirection.x += 0.1f;
        //            m_renderer.setCameraDirection(cameraDirection);
        //            break;
        //        }
        //        break;
        //    }
        //}

        m_input.update();

        if (m_input.getKeyPressed(KEY_ESC))
            quit = true;

        currentTime = static_cast<unsigned int>(SDL_GetTicks());

        m_renderer.render(static_cast<float>(currentTime - lastTime) / 1000.0f);

        lastTime = currentTime;
    }
}

}