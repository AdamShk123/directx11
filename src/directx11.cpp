#include "./directx11.hpp"

int main(int argc, char* argv[])
{
    std::println("Argument Count: {}", argc);

    for(int i = 0; i < argc; i++) 
    {
        std::println("Argument[{}]: {}", i, argv[i]);
    }
    std::println("Hello World!");

    auto game = Game::Game();

    game.run();

    return 0;
}