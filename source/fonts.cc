#include <quantum/fonts.hh>
#include <quantum/renderer.hh>
#include <SDL2/SDL.h>
#include <iostream>
#include <algorithm>
#define FONT_SYSTEM_IMPL
#include "../fonts/system-regular.hh"
#undef FONT_SYSTEM_IMPL

namespace quantum {
namespace fonts {

bool Glyph::operator<( const Glyph &that ) const
{
    return code < that.code;
}

Font::Font( Renderer &renderer, const uint8_t *data, int iw, int ih, int gh, int gc, const int *glyphs ) :
    iw_(iw), ih_(ih), gh_(gh), texture_(nullptr), data_(data)
{
    texture_ = renderer.create_font_texture(data, iw_, ih_);

    for (int i = 0, t = gc * 3, j = 0; i < t; i += 3, ++j)
    {
        Glyph glyph;
        glyph.code = glyphs[i];
        glyph.start = glyphs[i + 1];
        glyph.width = glyphs[i + 2];
        glyphs_.insert(glyph);
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
    static const int XSPACE = 1;
    static const int YSPACE = 3;

    int xoff = 0;
    int yoff = 0;
    for (auto it = text.begin(); it != text.end(); ++it)
    {
        int code = (int) *it;
        if (code == 32)
        {
            xoff += 3;
            continue;
        }
        else
        if (code == 10)
        {
            yoff += gh_ + YSPACE;
            xoff = 0;
            continue;
        }

        auto glyph = std::lower_bound(glyphs_.begin(), glyphs_.end(), code, compare_glyphs);
        if (glyph == glyphs_.end() || code < glyph->code)
            glyph = glyphs_.begin();
        if (xoff + glyph->width > w)
        {
            xoff = 0;
            yoff += gh_ + YSPACE;
        }
        if (h < gh_ + yoff) return;

        Rect from(glyph->start, 0, glyph->width, gh_);
        Rect to(x + xoff, y + yoff, glyph->width, gh_);
        renderer.draw(texture_, from, to);
        xoff += glyph->width + XSPACE;
    }
}

int Font::count() const
{
    return (int) glyphs_.size();
}

int Font::text_width( const std::string &text )
{
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
        if (glyph == glyphs_.end() || code < glyph->code)
            glyph = glyphs_.begin();
        offset += glyph->width;
    }
    return offset;
}


} // namespace fonts
} // namespace quantum
