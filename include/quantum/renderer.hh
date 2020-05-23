#ifndef RENDERER_HH
#define RENDERER_HH

#include <quantum/api.hh>
#include <stdint.h>

namespace quantum {

struct Line
{
    int x1, y1, x2, y2;

    Line();
    Line( int x1, int y1, int x2, int y2 );
};

struct Rect
{
    int x, y, w, h;

    Rect();
    Rect( int x1, int y1, int w, int h );
};

class Renderer
{
    public:
        virtual ~Renderer() = default;
        virtual void line( int x1, int y1, int x2, int y2, uint32_t color ) = 0;
        virtual void line( const Line &line, uint32_t color ) = 0;
        virtual void rectangle( int x, int y, int w, int h, uint32_t color ) = 0;
        virtual void rectangle( const Rect &rect, uint32_t color ) = 0;
        virtual void fill( int x, int y, int w, int h, uint32_t color ) = 0;
        virtual void fill( const Rect &rect, uint32_t color ) = 0;
        virtual void fill( uint32_t color ) = 0;
        virtual void clear() = 0;
        virtual void update() = 0;
};

class SDLRenderer : public Renderer
{
    public:
        SDLRenderer( bool init, int width, int height, uint32_t color );
        ~SDLRenderer();
        void line( int x1, int y1, int x2, int y2, uint32_t color ) override;
        void line( const Line &line, uint32_t color ) override;
        void rectangle( int x, int y, int w, int h, uint32_t color ) override;
        void rectangle( const Rect &rect, uint32_t color ) override;
        void fill( int x, int y, int w, int h, uint32_t color ) override;
        void fill( const Rect &rect, uint32_t color ) override;
        void fill( uint32_t color ) override;
        void clear() override;
        void update() override;

    protected:
        bool init_;
        int width_;
        int height_;
        uint32_t color_;
        void *window_;
        void *renderer_;
};

class ScaledRenderer : public SDLRenderer
{
    public:
        ScaledRenderer( bool init, int w1, int h1, int w2, int h2, uint32_t color );
        void fill( uint32_t color ) override;
        void update() override;

    protected:
        int texw_, texh_;
        void *texture_;
};

} // namespace quantum

#endif // RENDERER_HH