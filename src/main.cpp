#include "./main.hpp"

int main(int argc, char* argv[])
{
    std::println("Argument Count: {}", argc);

    for(int i = 0; i < argc; i++) 
    {
        std::println("Argument[{}]: {}", i, argv[i]);
    }

    std::println("Hello World!");

    auto window = Renderer::Window(CONSTANTS::TITLE.data(), CONSTANTS::SCREEN_WIDTH, CONSTANTS::SCREEN_HEIGHT);

    auto game = Renderer::Renderer(window);

    game.run();

    return 0;
}