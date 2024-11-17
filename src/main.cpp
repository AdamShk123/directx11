#include "./main.hpp"

int main(int argc, char* argv[])
{
    std::println("Argument Count: {}", argc);

    for(int i = 0; i < argc; i++) 
    {
        std::println("Argument[{}]: {}", i, argv[i]);
    }

    //std::println("Hello World!");

    //Logger::err("TAG", "Hello World!");

    auto window = Renderer::Window(CONSTANTS::TITLE, CONSTANTS::SCREEN_WIDTH, CONSTANTS::SCREEN_HEIGHT);

    auto renderer = Renderer::Renderer(window);

    auto game = Game::Game(renderer);

    game.run();

    return 0;
}