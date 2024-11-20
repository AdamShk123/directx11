#ifndef INPUT_HPP
#define INPUT_HPP

#include <unordered_map>
#include <algorithm>

#include <SDL2/SDL.h>

namespace Game 
{

enum Key : unsigned int
{
	KEY_ESC,
	KEY_W,
	KEY_S,
	KEY_A,
	KEY_D
};

class Input 
{
public:
	Input();
	~Input();

	void update();
	bool getKeyPressed(Key key);
private:
	SDL_Event m_event;
	std::unordered_map<Key, bool> m_keyStates;
};

}

#endif // INPUT_HPP