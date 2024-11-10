#include "./directx11.hpp"

int main(int argc, char* argv[])
{
    std::println("Hello World!");

    auto game = Game::Game();

    game.run();

    return 0;
}