#include <quantum/renderer.hh>
#include <SDL2/SDL.h>

#define SET_COLOR(rgba) \
    do { SDL_SetRenderDrawColor( (SDL_Renderer*) renderer_, \
        (Uint8) ((rgba) >> 24), \
        (Uint8) ((rgba) >> 16), \
        (Uint8) ((rgba) >> 8), \
        (Uint8) (rgba)); \
        SDL_SetRenderDrawBlendMode((SDL_Renderer*) renderer_, (((rgba) & 0xFF) == 255) \
            ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND); } while (false)

namespace quantum {


Point::Point() : x(0), y(0)
{
}

Point::Point( int x, int y ) : x(x), y(y)
{
}

Point &Point::operator+=( const Point &that )
{
    x += that.x;
    y += that.y;
    return *this;
}

Point &Point::operator-=( const Point &that )
{
    x -= that.x;
    y -= that.y;
    return *this;
}

Point Point::operator+( const Point &that )
{
    Point result;
    result.x = x + that.x;
    result.y = y + that.y;
    return result;
}

Point Point::operator-( const Point &that )
{
    Point result;
    result.x = x - that.x;
    result.y = y - that.y;
    return result;
}

Line::Line() : x1(0), y1(0), x2(0), y2(0)
{
}

Line::Line( int x1, int y1, int x2, int y2 ) : x1(x1), y1(y1), x2(x2), y2(y2)
{
}

Rect::Rect() : x(0), y(0), w(0), h(0)
{
}

Rect::Rect( int x, int y, int w, int h ) : x(x), y(y), w(w), h(h)
{
}

bool Rect::intersect( const Point &point ) const
{
    return point.x >= x && point.x <= x + w && point.y >= y && point.y <= y + h;
}

SDLRenderer::SDLRenderer( bool init, int width, int height, uint32_t color ) : init_(init),
    width_(width), height_(height), color_(color)
{
    if (init_) SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	SDL_Rect srect;
	SDL_GetDisplayBounds(0, &srect);
	if (width > srect.w || height > srect.h)
    {
		if (init_) SDL_Quit();
        throw Exception("Renderer dimensions must fit the screen");
	}

	window_ = SDL_CreateWindow("Quantum", srect.w / 2 - width / 2, srect.h / 2 - height / 2, width_, height_, SDL_WINDOW_SHOWN);
	renderer_ = SDL_CreateRenderer((SDL_Window *)window_, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC*/);
    fill(color_);
    draw();
}

SDLRenderer::~SDLRenderer()
{
    SDL_DestroyRenderer((SDL_Renderer *)renderer_);
    SDL_DestroyWindow((SDL_Window *)window_);
    if (init_) SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_TIMER);
}

void SDLRenderer::line( int x1, int y1, int x2, int y2, uint32_t color )
{
    SET_COLOR(color);
    SDL_RenderDrawLine( (SDL_Renderer*) renderer_, x1, y1, x2, y2);
}

void SDLRenderer::line( const Line &line, uint32_t color )
{
    this->line(line.x1, line.y1, line.x2, line.y2, color);
}

void SDLRenderer::rectangle( int x, int y, int w, int h, uint32_t color )
{
    Rect rect(x, y, w, h);
    rectangle(rect, color);
}

void SDLRenderer::rectangle( const Rect &rect, uint32_t color )
{
    SET_COLOR(color);
    SDL_Rect temp;
    temp.x = rect.x;
    temp.y = rect.y;
    temp.w = rect.w;
    temp.h = rect.h;
    SDL_RenderDrawRect( (SDL_Renderer*) renderer_, &temp);
}

void SDLRenderer::fill( int x, int y, int w, int h, uint32_t color )
{
    Rect rect(x, y, w, h);
    fill(rect, color);
}

void SDLRenderer::fill( const Rect &rect, uint32_t color )
{
    SET_COLOR(color);
    SDL_Rect temp;
    temp.x = rect.x;
    temp.y = rect.y;
    temp.w = rect.w;
    temp.h = rect.h;
    SDL_RenderFillRect( (SDL_Renderer*) renderer_, &temp);
}

void SDLRenderer::fill( uint32_t color )
{
    SET_COLOR(color);
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = width_;
    rect.h = height_;
    SDL_RenderFillRect( (SDL_Renderer*) renderer_, &rect);
}

void SDLRenderer::clear()
{
    fill(color_);
}

void SDLRenderer::draw()
{
    SDL_RenderPresent( (SDL_Renderer*) renderer_ );
}

int SDLRenderer::get_scale() const
{
    return 1;
}

ScaledRenderer::ScaledRenderer( bool init, int w1, int h1, int scale, uint32_t color ) :
    SDLRenderer(init, w1 * scale, h1 * scale, color), texw_(w1), texh_(h1), scale_(scale)
{
    texture_ = SDL_CreateTexture( (SDL_Renderer*) renderer_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, texw_, texh_);
    SDL_SetRenderTarget( (SDL_Renderer*) renderer_, (SDL_Texture*) texture_ );
    clear();
}

void ScaledRenderer::draw()
{
    SDL_SetRenderTarget( (SDL_Renderer*) renderer_, nullptr );
    SDL_Rect src = {0, 0, texw_, texh_};
    SDL_Rect dst = {0, 0, width_, height_};
    SDL_RenderCopy( (SDL_Renderer*) renderer_, (SDL_Texture*) texture_, &src, &dst );
    SDL_RenderPresent( (SDL_Renderer*) renderer_ );
    SDL_SetRenderTarget( (SDL_Renderer*) renderer_, (SDL_Texture*) texture_ );
}

void ScaledRenderer::fill( uint32_t color )
{
    SET_COLOR(color);
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = texw_;
    rect.h = texh_;
    SDL_RenderFillRect( (SDL_Renderer*) renderer_, &rect);
}

int ScaledRenderer::get_scale() const
{
    return scale_;
}

} // namespace quantum