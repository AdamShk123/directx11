#ifndef INPUT_HPP
#define INPUT_HPP

#include <unordered_map>
#include <algorithm>

#include <SDL2/SDL.h>

#include "./interface_input.hpp"

namespace Game 
{

class Input : public IInput
{
public:
	Input();
	~Input();

	void update() override;
private:
	SDL_Event m_event;
};

}

#endif // INPUT_HPP