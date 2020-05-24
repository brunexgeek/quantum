#include <quantum/widget.hh>
#include <cstring>

namespace quantum {

Widget::Widget( int x, int y ) : pos_(x, y), changed_(true)
{
}

Widget::Widget( Container *parent, int x, int y ) : pos_(x, y), changed_(true), parent_(parent)
{
}

bool Widget::is_changed() const
{
    return changed_;
}

Point Widget::position() const
{
    return pos_;
}

void Widget::position( const Point &point )
{
    pos_ = point;
}

Point Widget::absolute_position() const
{
    Point result = pos_;
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

Container::Container( int x, int y ) : Widget(x, y)
{
}

Container::Container( Container *parent, int x, int y ) : Widget(parent, x, y)
{
}

void Container::append( Widget *object )
{
    object->parent(this);
    children_.push_back(object);
}

Window::Window( int x, int y, int cwidth, int cheight ) : Container(x, y), cwidth_(cwidth),
    cheight_(cheight), moving_(false)
{
    width_ = BORDER_THICKNESS * 2 + cwidth_;
    height_ = TITLE_HEIGHT + BORDER_THICKNESS * 2 + cheight_;
}

void Window::draw( Renderer &rend, bool force )
{
    if (changed_ || force)
    {
        changed_ = false;
        force = true;

        int sx = pos_.x;
        int sy = pos_.y;

        Rect rect;
        // window area
        rend.rectangle(sx, sy, width_, height_, 0x000000FF);
        rect.x = sx + BORDER_THICKNESS,
        rect.y = sy + BORDER_THICKNESS,
        rect.w = cwidth_;
        rect.h = cheight_ + TITLE_HEIGHT;
        rend.fill(rect, 0xFFFFFFFF);

        // title bar
        Line line;
        line.x1 = sx + BORDER_THICKNESS + 1;
        line.y1 = sy + BORDER_THICKNESS + 3;
        line.x2 = sx + cwidth_ - 1;
        line.y2 = sy + BORDER_THICKNESS + 3;
        for (int i = 0; i < 6; ++i)
        {
            rend.line(line, 0xFF);
            line.y1 += 2;
            line.y2 += 2;
        }
        rend.line(sx, sy + TITLE_HEIGHT, sx + width_ - 1, sy + TITLE_HEIGHT, 0x000000FF);
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

        // draw moving frame
        if (moving_)
        {
            rend.rectangle(new_pos_.x, new_pos_.y, width_, height_, 0x7F007F7F);
        }
    }

    for (auto it = children_.begin(); it != children_.end(); ++it)
        (*it)->draw(rend, force);
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
        Rect title(pos_.x, pos_.y, width_, TITLE_HEIGHT);
        if (title.intersect(event.mouse_button.cursor))
        {
            moving_ = true;
            drag_off_ = event.mouse_button.cursor - pos_;
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
        pos_ = event.mouse_button.cursor - drag_off_;
        moving_ = false;
    }
    else
        return false;

    return changed_ = true;
}

} // namespace quantum