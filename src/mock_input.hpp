#ifndef INPUT_HPP
#define INPUT_HPP

#include "./interface_input.hpp"

namespace Game
{

	class MockInput : public IInput
	{
	public:
		MockInput() {};
		~MockInput() {};

		void update() override 
		{
			setKeyPressed(KEY_ESC, true);
			setKeyPressed(KEY_D, false);
		};
	private:
	};

}

#endif // INPUT_HPP