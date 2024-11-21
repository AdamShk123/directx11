#include <gtest/gtest.h>

#include "../src/mock_input.hpp"

TEST(testInputSuite, testEmpty)
{
    auto input = Game::MockInput();
    EXPECT_EQ(input.getKeyPressed(Game::Key::KEY_A), false);
}

TEST(testInputSuite, testEscapePressed)
{
    auto input = Game::MockInput();
    input.update();
    EXPECT_EQ(input.getKeyPressed(Game::Key::KEY_ESC), true);
}

TEST(testInputSuite, testDNotPressed)
{
    auto input = Game::MockInput();
    input.update();
    EXPECT_EQ(input.getKeyPressed(Game::Key::KEY_D), false);
}