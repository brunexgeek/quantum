#include <SDL2/SDL.h>
#include <quantum/api.hh>
#include <quantum/renderer.hh>
#include <quantum/widget.hh>
#include <quantum/input.hh>
#include <quantum/fonts.hh>
#include <unistd.h>
#include <iostream>
#include "../fonts/system-bold.hh"

using namespace quantum;

static constexpr int SCALE = 2;
static constexpr int SCREEN_WIDTH = 640 / 2;
static constexpr int SCREEN_HEIGHT = 480 / 2;

int main( int argc, char **argv )
{
    ScaledRenderer renderer(true, SCREEN_WIDTH, SCREEN_HEIGHT, 2, 0x7F7F7FFF);
    //SDLRenderer renderer(true, SCREEN_WIDTH, SCREEN_HEIGHT, 0x7F7F7FFF);
    int width = 250;
    int height = 100;
    int x = SCREEN_WIDTH / 2 - width / 2;
    int y = SCREEN_HEIGHT / 2 - height / 2;
    Window window(x, y, width, height);
    //fonts::Font font(renderer, CHICAGO_BITMAP, CHICAGO_INFO[0], CHICAGO_INFO[1], CHICAGO_INFO[2], CHICAGO_INFO[3], CHICAGO_GLYPH_COUNT, CHICAGO_GLYPHS);
    fonts::Font font(renderer, SYSTEM_BITMAP, SYSTEM_INFO[0], SYSTEM_INFO[1], SYSTEM_INFO[2], SYSTEM_GLYPH_COUNT, SYSTEM_GLYPHS);
    Label text1(10, 10, "abcdefghijklmnopqrstuvwxyz", font);
    Label text2(10, 30, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", font);
    window.append(text1);
    window.append(text2);
    SDLInputManager input(true, renderer);

    while (true)
    {
        if (window.is_changed()) renderer.clear();
        window.draw(renderer);
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