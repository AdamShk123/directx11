#include "./input.hpp"

namespace Game 
{

Input::Input() : m_keyStates()
{

}

Input::~Input()
{
}

void Input::update()
{
    for (const auto& state : m_keyStates) 
    {
        m_keyStates[state.first] = false;
    }

    while (SDL_PollEvent(&m_event)) {
        switch (m_event.type)
        {
        case SDL_QUIT:
            break;
        case SDL_KEYDOWN:
            switch (m_event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                m_keyStates[KEY_ESC] = true;
                break;
            }
            break;
        }
    }
}

bool Input::getKeyPressed(Key key)
{
    return m_keyStates[key];
}

}