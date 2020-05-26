#ifndef QUANTUM_FONTS_HH
#define QUANTUM_FONTS_HH

#include <string>
#include <set>

namespace quantum {

class Renderer;

namespace fonts {

extern const uint8_t CHICAGO_PIXELS[];

struct Glyph
{
    int code;
    int start;
    int width;

    bool operator<( const Glyph &that ) const;
};

class Font
{
    public:
        Font( Renderer &renderer, const uint8_t *data, int iw, int ih, int gh, int gc, const int *glyphs );
        ~Font();
        void draw( Renderer &renderer, const std::string &text, int x, int y, int w = 0, int h = 0 );
        int text_width( const std::string &text );
        const uint8_t *data() const;
        int count() const;

    protected:
        int iw_, ih_, gh_;
        void *texture_;
        const uint8_t *data_;
        std::set<Glyph> glyphs_;
};

class FontManager
{
    public:
        ~FontManager();
        static FontManager &get_instance();
        const Font &get_font( const std::string &name ) const;

    protected:
        FontManager();
};

} // namespace fonts
} // namespace quantum

#endif // QUANTUM_FONTS_HH