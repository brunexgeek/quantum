#ifndef WIDGETS_HH
#define WIDGETS_HH

#include <quantum/api.hh>
#include <quantum/renderer.hh>
#include <list>

namespace quantum {

struct Event
{
};

struct Point
{
    int x, y;

    Point();
    Point( int x, int y );
    Point( const Point &that ) = default;
    Point( Point &&that ) = default;
    Point &operator =( const Point &that ) = default;
    Point &operator+=( const Point &that );
};

class Container;

class Widget
{
    public:
        Widget() = default;
        Widget( int x, int y );
        virtual void draw( Renderer &rend ) = 0;
        virtual bool update( Event &event ) = 0;
        virtual Point position() const;
        virtual void position( const Point &point );
        virtual const Container *parent() const = 0;
        virtual Container *parent() = 0;

    protected:
        Point pos_;

        virtual Point absolute_position() const;
};

class Container : public Widget
{
    public:
        Container() = default;
        Container( int x, int y );
        Container( Container *parent, int x, int y );
        virtual void append( Widget *object );
        const Container *parent() const override;
        Container *parent() override;

    protected:
        Container *parent_;
        std::list<Widget*> children_;
};

class Window : public Container
{
    public:
        static constexpr int TITLE_HEIGHT = 18; // px
        static constexpr int BORDER_THICKNESS = 1; // px

        Window( int x, int y, int width, int height );
        ~Window() = default;
        void draw( Renderer &rend ) override;
        bool update( Event &event ) override;

    protected:
        int width_;  // TODO: move to Widget
        int height_; // TODO: move to Widget
        int cwidth_;
        int cheight_;
};

} // namespace quantum

#endif // WIDGETS_HH