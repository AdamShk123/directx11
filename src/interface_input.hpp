#ifndef INTERFACE_INPUT_HPP
#define INTERFACE_INPUT_HPP

namespace Game 
{

struct MousePosition
{
	int x;
	int y;
};

enum Key : unsigned int
{
	KEY_ESC,
	KEY_W,
	KEY_S,
	KEY_A,
	KEY_D
};

class IInput 
{
public:
	IInput() {};
	~IInput() {};
	virtual void update() = 0;
	bool getKeyPressed(Key key) { return m_keyStates[key]; };
	MousePosition getMousePosition() const { return m_mousePosition; };
protected:
	void setKeyPressed(Key key, bool pressed) { m_keyStates[key] = pressed; }
	void setMousePosition(MousePosition pos) { m_mousePosition = pos; }
private:
	std::unordered_map<Key, bool> m_keyStates{};
	MousePosition m_mousePosition{};
};

}

#endif // INTERFACE_INPUT_HPP