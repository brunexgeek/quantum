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
        Widget( int x, int y, int w, int h );
        Widget( Container *parent, int x, int y, int w, int h );
        virtual void draw( Renderer &rend, bool force = false ) = 0;
        virtual bool update( Event &event ) = 0;
        virtual Point size() const;
        virtual void size( const Point &point );
        virtual Point position() const;
        virtual void position( const Point &point );
        virtual void parent( Container *object );
        virtual const Container *parent() const;
        virtual Container *parent();
        virtual bool is_changed() const;
        virtual Point absolute_position() const;

    protected:
        Rect rect_;
        bool changed_;
        Container *parent_;
};

class Container : public Widget
{
    public:
        Container() = default;
        Container( int x, int y, int w, int h );
        Container( Container *parent, int x, int y, int w, int h );
        virtual Point content_size() const;
        virtual void content_size( const Point &point );
        virtual void append( Widget *object );
        virtual void append( Widget &object );

    protected:
        Point csize_;
        std::list<Widget*> children_;
};

class Window : public Container
{
    public:
        static constexpr int BORDER = 1; // px
        static constexpr int TITLE_LINES = 4; // px
        static constexpr int TITLE_VMARGIN = 3; // px
        static constexpr int TITLE_HMARGIN = 1; // px
        static constexpr int TITLE_HEIGHT = BORDER + TITLE_VMARGIN * 2 + TITLE_LINES * 2 + 1; // px
        static constexpr int SHADOW_VOFFSET = 2; // px
        static constexpr int SHADOW_HOFFSET = 2; // px

        Window( int x, int y, int width, int height );
        ~Window() = default;
        void draw( Renderer &rend, bool force = false  ) override;
        bool update( Event &event ) override;
        Point absolute_position() const override;

    protected:
        bool moving_;
        Point new_pos_;
        Point drag_off_;
};

namespace fonts { class Font; }

class Label : public Widget
{
    public:
        Label( int x, int y, const std::string &text, fonts::Font &font );
        void draw( Renderer &rend, bool force = false ) override;
        bool update( Event &event ) override;

    protected:
        std::string text_;
        fonts::Font &font_;
};

} // namespace quantum

#endif // WIDGETS_HH