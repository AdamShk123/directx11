#include "./input.hpp"

namespace Game 
{

Input::Input() : m_event()
{

}

Input::~Input()
{
}

void Input::update()
{
    for (int i = 0; i < KEY_D; i++) 
    {
        setKeyPressed(static_cast<Key>(i), false);
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
                setKeyPressed(KEY_ESC, true);
                break;
            case SDLK_w:
                setKeyPressed(KEY_W, true);
                break;
            case SDLK_s:
                setKeyPressed(KEY_S, true);
                break;
            case SDLK_a:
                setKeyPressed(KEY_A, true);
                break;
            case SDLK_d:
                setKeyPressed(KEY_D, true);
                break;
            }
            break;
        case SDL_MOUSEMOTION:
            setMousePosition(MousePosition(m_event.motion.x, m_event.motion.y));
            break;
        }
    }
}

}