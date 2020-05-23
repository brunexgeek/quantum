#include <quantum/widget.hh>

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

Widget::Widget( int x, int y ) : pos_(x, y)
{
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

Container::Container( int x, int y ) : Widget(x, y)
{
}

Container::Container( Container *parent, int x, int y ) : Widget(x, y), parent_(parent)
{
}

void Container::append( Widget *object )
{
    children_.push_back(object);
}

const Container *Container::parent() const
{
    return nullptr;
}

Container *Container::parent()
{
    return nullptr;
}

Window::Window( int x, int y, int cwidth, int cheight ) : Container(x, y), cwidth_(cwidth), cheight_(cheight)
{
    width_ = BORDER_THICKNESS * 2 + cwidth_;
    height_ = TITLE_HEIGHT + BORDER_THICKNESS * 2 + cheight_;
}

void Window::draw( Renderer &rend )
{
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
    rend.line(sx, sy + TITLE_HEIGHT, sx + width_, sy + TITLE_HEIGHT, 0x000000FF);
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

    for (auto it = children_.begin(); it != children_.end(); ++it)
        (*it)->draw(rend);
}

bool Window::update( Event &event )
{
    (void) event;
    return false;
}

} // namespace quantum