#include <SDL2/SDL.h>
#include <quantum/api.hh>
#include <quantum/renderer.hh>
#include <quantum/widget.hh>
#include <quantum/input.hh>
#include <unistd.h>
#include <iostream>

using namespace quantum;

static constexpr int SCALE = 2;
static constexpr int SCREEN_WIDTH = 1024 / 2;
static constexpr int SCREEN_HEIGHT = 768 / 2;

int main( int argc, char **argv )
{
    ScaledRenderer renderer(true, SCREEN_WIDTH, SCREEN_HEIGHT, 2, 0x7F7F7FFF);
    Window window(50, 50, 250, 150);
    SDLInputManager input(true, renderer);

    while (true)
    {
        renderer.clear();
        window.draw(renderer, true);
        renderer.draw();
        Event event;
        if (input.poll_event(event))
        {
            if (event.type == ET_QUIT) break;
            if (event.type == ET_NONE) continue;
            //if (event.type == ET_MOUSE_MOVE) std::cerr << event.mouse_move.x << " - " << event.mouse_move.y << std::endl;
            window.update(event);
        }
        else
        {
            timespec value;
            value.tv_nsec = 10000000; // 10ms
            value.tv_sec = 0;
            nanosleep(&value, &value);
        }
    }
    return 0;
}