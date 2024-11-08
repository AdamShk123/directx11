#include "./directx11.hpp"

int main(int argc, char* argv[])
{
    //auto range = std::views::iota(0, 10);

    //auto print_function = [](const int& num) { std::println("Number: {}", num); };

    //std::ranges::for_each(range.begin(), range.end(), print_function);

    std::println("Hello World!");

    auto game = Game::Game();

    game.run();

    return 0;
}