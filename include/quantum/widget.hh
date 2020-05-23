#ifndef WIDGETS_HH
#define WIDGETS_HH

#include <quantum/api.hh>
#include <quantum/renderer.hh>
#include <quantum/input.hh>
#include <list>

namespace quantum {

class Container;

class Widget
{
    public:
        Widget() = default;
        Widget( int x, int y );
        virtual void draw( Renderer &rend, bool force = false ) = 0;
        virtual bool update( Event &event ) = 0;
        virtual Point position() const;
        virtual void position( const Point &point );
        virtual const Container *parent() const = 0;
        virtual Container *parent() = 0;
        virtual bool is_changed() const;

    protected:
        Point pos_;
        bool changed_;

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
        void draw( Renderer &rend, bool force = false  ) override;
        bool update( Event &event ) override;

    protected:
        int width_;  // TODO: move to Widget
        int height_; // TODO: move to Widget
        int cwidth_;
        int cheight_;
        bool moving_;
        Point new_pos_;
        Point drag_off_;
};

} // namespace quantum

#endif // WIDGETS_HH