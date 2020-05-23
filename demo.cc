#include <quantum/api.hh>
#include <quantum/renderer.hh>
#include <quantum/widget.hh>
#include <unistd.h>

int main( int argc, char **argv )
{
    quantum::ScaledRenderer renderer(true, 1024, 768, 1024 / 2, 768 / 2, 0x7F7F7FFF);
    quantum::Window window(50, 50, 300, 200);
    window.draw(renderer);
    renderer.update();
    sleep(10);
    return 0;
}