#ifndef QUANTUM_INPUT_HH
#define QUANTUM_INPUT_HH

#include <quantum/renderer.hh>

namespace quantum {

static constexpr int ET_NONE = 0;
static constexpr int ET_MOUSE_DOWN = 1;
static constexpr int ET_MOUSE_UP = 2;
static constexpr int ET_MOUSE_MOVE = 3;
static constexpr int ET_KEY_DOWN = 4;
static constexpr int ET_KEY_UP = 5;
static constexpr int ET_QUIT = 6;

struct MouseData
{
    Point cursor;
    int button;
};

struct KeyData
{
    int key;
};

struct Event
{
    int type;
    union {
        MouseData mouse_button;
        MouseData mouse_move;
        KeyData key_down;
        KeyData key_up;
    };

    Event();
    Event( const Event &that );
};

class Renderer;

class InputManager
{
    public:
        InputManager( Renderer &rend );
        virtual ~InputManager() = default;
        virtual bool poll_event( Event &event ) = 0;
        virtual Renderer &get_renderer();

    public:
        Renderer &rend_;
};

class SDLInputManager : public InputManager
{
    public:
        SDLInputManager( bool init, Renderer &rend );
        ~SDLInputManager();
        bool poll_event( Event &event ) override;

    protected:
        bool init_;
};

} // namespace quantum

#endif //  QUANTUM_INPUT_HH