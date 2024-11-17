#ifndef GAME_HPP
#define GAME_HPP

#include "./renderer.hpp"

namespace Game 
{

class Game 
{
public:
	Game(Renderer::Renderer& renderer);
	~Game();

	void run();
private:
	Renderer::Renderer& m_renderer;
};

}

#endif // GAME_HPP