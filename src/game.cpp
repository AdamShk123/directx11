#include "./game.hpp"

namespace Game 
{

Game::Game(Renderer::Renderer& renderer) : m_renderer(renderer)
{

}

Game::~Game() 
{

}

void Game::run()
{
    SDL_Event e;
    bool quit = false;

    //unsigned int lastTime = 0, 
    //unsigned int currentTime = 0;

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

        m_renderer.render();

        //currentTime = SDL_GetTicks();
        //lastTime = SDL_GetTicks();
    }
}

}