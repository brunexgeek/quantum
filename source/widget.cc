#include <quantum/widget.hh>
#include <quantum/fonts.hh>
#include <cstring>

namespace quantum {

Widget::Widget( int x, int y, int w, int h ) : rect_(x, y, w, h), changed_(true), parent_(nullptr)
{
}

Widget::Widget( Container *parent, int x, int y, int w, int h ) : rect_(x, y, w, h), changed_(true), parent_(parent)
{
}

bool Widget::is_changed() const
{
    return changed_;
}

Point Widget::size() const
{
    return Point(rect_.w, rect_.h);
}

void Widget::size( const Point &size )
{
    rect_.w = size.x;
    rect_.h = size.y;
}

Point Widget::position() const
{
    return Point(rect_.x, rect_.y);
}

void Widget::position( const Point &point )
{
    rect_.x = point.x;
    rect_.y = point.y;
}

Point Widget::absolute_position() const
{
    Point result = Point(rect_.x, rect_.y);
    const Container *object = parent();
    if (object) result += object->absolute_position();
    return result;
}

const Container *Widget::parent() const
{
    return parent_;
}

Container *Widget::parent()
{
    return parent_;
}

void Widget::parent( Container *object )
{
    parent_ = object;
}

Container::Container( int x, int y, int w, int h ) : Widget(x, y, w, h)
{
}

Container::Container( Container *parent, int x, int y, int w, int h  ) : Widget(parent, x, y, w, h)
{
}

Point Container::content_size() const
{
    return csize_;
}

void Container::content_size( const Point &size )
{
    csize_ = size;
}

void Container::append( Widget *object )
{
    object->parent(this);
    children_.push_back(object);
}

void Container::append( Widget &object )
{
    append(&object);
}

Window::Window( int x, int y, int cwidth, int cheight ) : Container(x, y, cwidth, cheight), moving_(false)
{
    csize_ = Point(cwidth, cheight);
    rect_.w = BORDER_THICKNESS * 2 + cwidth;
    rect_.h = TITLE_HEIGHT + BORDER_THICKNESS * 2 + cheight;
}

void Window::draw( Renderer &rend, bool force )
{
    if (changed_ || force)
    {
        int sx = rect_.x;
        int sy = rect_.y;

        Rect rect;
        // window area
        rend.rectangle(sx, sy, rect_.w, rect_.h, 0x000000FF);
        rect.x = sx + BORDER_THICKNESS,
        rect.y = sy + BORDER_THICKNESS,
        rect.w = csize_.x;
        rect.h = csize_.y + TITLE_HEIGHT;
        rend.fill(rect, 0xFFFFFFFF);

        // title bar
        Line line;
        line.x1 = sx + BORDER_THICKNESS + 1;
        line.y1 = sy + BORDER_THICKNESS + 3;
        line.x2 = sx + csize_.x - 1;
        line.y2 = sy + BORDER_THICKNESS + 3;
        for (int i = 0; i < 6; ++i)
        {
            rend.line(line, 0xFF);
            line.y1 += 2;
            line.y2 += 2;
        }
        rend.line(sx, sy + TITLE_HEIGHT, sx + rect_.w - 1, sy + TITLE_HEIGHT, 0x000000FF);
        // close button
        rect.x = sx + BORDER_THICKNESS + 7;
        rect.y = sy + BORDER_THICKNESS + 2;
        rect.w = 13;
        rect.h = 13;
        rend.fill(rect, 0xFFFFFFFF);
        rect.x += 1;
        rect.y += 1;
        rect.w -= 2;
        rect.h -= 2;
        rend.rectangle(rect, 0x000000FF);
    }

    for (auto it = children_.begin(); it != children_.end(); ++it)
        (*it)->draw(rend, force || changed_);

    // draw moving frame
    if ((changed_ || force) && moving_)
    {
        rend.rectangle(new_pos_.x, new_pos_.y, rect_.w, rect_.h, 0x7F007F7F);
    }
    changed_ = false;
}

bool Window::update( Event &event )
{
    for (auto it = children_.begin(); it != children_.end(); ++it)
    {
        (*it)->update(event);
    }

    if (event.type < ET_MOUSE_DOWN && event.type > ET_MOUSE_MOVE) return false;

    if (event.type == ET_MOUSE_DOWN && !moving_)
    {
        Rect title(rect_.x, rect_.y, rect_.w, TITLE_HEIGHT);
        if (title.intersect(event.mouse_button.cursor))
        {
            moving_ = true;
            drag_off_ = event.mouse_button.cursor;
            drag_off_.x -= rect_.x;
            drag_off_.y -= rect_.y;
            new_pos_ = event.mouse_button.cursor - drag_off_;
        }
    }
    else
    if (event.type == ET_MOUSE_MOVE && moving_)
    {
        new_pos_ = event.mouse_move.cursor - drag_off_;
    }
    else
    if (event.type == ET_MOUSE_UP && moving_)
    {
        Point tmp = event.mouse_button.cursor - drag_off_;
        rect_.x = tmp.x;
        rect_.y = tmp.y;
        moving_ = false;
    }
    else
        return false;

    return changed_ = true;
}

Point Window::absolute_position() const
{
    return Point(rect_.x + BORDER_THICKNESS, rect_.y + BORDER_THICKNESS + TITLE_HEIGHT);
}

Label::Label( int x, int y, const std::string &text, fonts::Font &font ) : Widget(x, y, 0, 0),
    text_(text), font_(font)
{
}

void Label::draw( Renderer &rend, bool force )
{
    if (!changed_ && !force) return;

    Point pos;
    pos.x = rect_.x;
    pos.y = rect_.y;
    if (parent_)
    {
        pos += parent_->absolute_position();
        Point csize = parent_->content_size();
        font_.draw(rend, text_, pos.x, pos.y, csize.x - rect_.x, csize.y - rect_.y);
    }
    else
        font_.draw(rend, text_, pos.x, pos.y);
    changed_ = false;
}

bool Label::update( Event &event )
{
    (void) event;
    return false;
}

} // namespace quantum