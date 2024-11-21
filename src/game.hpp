#ifndef GAME_HPP
#define GAME_HPP

#include "./renderer.hpp"
#include "./input.hpp"

namespace Game 
{

class Game 
{
public:
	Game(Renderer::Renderer& renderer, IInput& input);
	~Game();

	void run();
private:
	Renderer::Renderer& m_renderer;
	IInput& m_input;
};

}

#endif // GAME_HPP