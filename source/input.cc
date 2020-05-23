#include <quantum/api.hh>
#include <quantum/input.hh>
#include <quantum/renderer.hh>
#include <cstring>
#include <SDL2/SDL.h>

namespace quantum {

Event::Event()
{
    type = ET_NONE;
    std::memset(&mouse_button, sizeof(mouse_button), 0);
}

Event::Event( const Event &that )
{
    type = that.type;
    mouse_button.cursor = that.mouse_button.cursor;
    mouse_button.button = that.mouse_button.button;
}

InputManager::InputManager( Renderer &rend ) : rend_(rend)
{
}

Renderer &InputManager::get_renderer()
{
    return rend_;
}

SDLInputManager::SDLInputManager( bool init, Renderer &rend ) : InputManager(rend), init_(init)
{
    if (init_) SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_TIMER);
}

SDLInputManager::~SDLInputManager()
{
    if (init_) SDL_QuitSubSystem(SDL_INIT_EVENTS | SDL_INIT_TIMER);
}

static bool convertEvent( SDL_Event &event, Event &result, Renderer &rend )
{
    result.type = ET_NONE;

    switch (event.type)
    {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            result.mouse_button.cursor.x = event.button.x / rend.get_scale();
            result.mouse_button.cursor.y = event.button.y / rend.get_scale();
            result.mouse_button.button = event.button.button;
            result.type = (event.type == SDL_MOUSEBUTTONUP) ? ET_MOUSE_UP : ET_MOUSE_DOWN;
            break;
        case SDL_MOUSEMOTION:
            result.mouse_move.cursor.x = event.motion.x / rend.get_scale();
            result.mouse_move.cursor.y = event.motion.y / rend.get_scale();
            result.type = ET_MOUSE_MOVE;
            break;
        case SDL_QUIT:
            result.type = ET_QUIT;
            break;
    }

    return result.type != ET_NONE;
}

bool SDLInputManager::poll_event( Event &event )
{
    SDL_Event se;
    event.type = ET_NONE;
    if (!SDL_PollEvent(&se)) return false;
    return convertEvent(se, event, rend_);
}

} // namespace quantum