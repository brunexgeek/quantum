#include <quantum/fonts.hh>
#include <quantum/renderer.hh>
#include <SDL2/SDL.h>
#include <iostream>
#include <algorithm>
#define FONT_CHICAGO_IMPL
#include "../fonts/chicago-regular.hh"
#undef FONT_CHICAGO_IMPL

namespace quantum {
namespace fonts {

Font::Font( Renderer &renderer, const uint8_t *data, int iw, int ih, int gw, int gh, int gc, const int *glyphs ) :
    iw_(iw), ih_(ih), gw_(gw), gh_(gh)
{
    texture_ = SDL_CreateTexture( (SDL_Renderer*) renderer.get_native(),
    #ifdef USE_ORIGINAL_FORMAT
        SDL_PIXELFORMAT_INDEX8,
    #else
        SDL_PIXELFORMAT_RGBA8888,
    #endif
        SDL_TEXTUREACCESS_STREAMING, iw, ih);
    int pitch;
    uint32_t *pixels;
    if (SDL_LockTexture( (SDL_Texture*) texture_, nullptr, (void**) &pixels, &pitch) == 0)
    {
        #ifdef USE_ORIGINAL_FORMAT
        std::copy(data, data + (iw * ih), pixels);
        #else
        for (int i = 0, t = iw_ * ih; i < t; ++i)
        {
            pixels[i] = (data[i]) ? 0xFFFFFFFF : 0x000000FF;
        }
        #endif
        SDL_UnlockTexture( (SDL_Texture*) texture_ );
    }

    glyphs_.resize(gc);
    for (int i = 0, t = gc * 3; i < t; i += 3)
    {
        Glyph glyph;
        glyph.code = glyphs[i];
        glyph.index = glyphs[i + 1];
        glyph.width = glyphs[i + 2];
        glyphs_.push_back(glyph);
    }
}

Font::~Font()
{
    SDL_DestroyTexture((SDL_Texture*) texture_);
}

static bool compare_glyphs( const Glyph &a, int code )
{
    return a.code < code;
}

void Font::draw( Renderer &renderer, const std::string &text, int x, int y, int w, int h )
{
    if (h > 0 && gw_ > h) return;

    int offset = 0;
    for (auto it = text.begin(); it != text.end(); ++it)
    {
        int code = (int) *it;
        if (code == 32)
        {
            offset += 3;
            continue;
        }

        auto glyph = std::lower_bound(glyphs_.begin(), glyphs_.end(), code, compare_glyphs);
        if (glyph == glyphs_.end() || code < glyph->code) continue;
        if (w > 0 && x + offset + glyph->width > x + w) return;
        SDL_Renderer *rend = (SDL_Renderer*) renderer.get_native();
        int gpr = iw_ / gw_;

        SDL_Rect from;
        from.x = (glyph->index % gpr) * gw_;
        from.y = (glyph->index / gpr) * gh_;
        from.w = glyph->width;
        from.h = gh_;

        SDL_Rect to;
        to.x = x + offset;
        to.y = y;
        to.w = glyph->width;
        to.h = gh_;

        SDL_RenderCopy(rend, (SDL_Texture*) texture_, &from, &to);
        offset += glyph->width + 1;
    }
}

int Font::count() const
{
    return glyphs_.size();
}

} // namespace fonts
} // namespace quantum
