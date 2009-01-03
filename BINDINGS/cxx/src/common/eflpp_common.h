#ifndef EFL_COMMON_H
#define EFL_COMMON_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_sys.h"
#include "eflpp_debug_internal.h"
#include <sigc++/sigc++.h>
#include <iostream>

using std::ostream;
using std::istream;

namespace efl {

typedef sigc::signal<void> Signal;
typedef sigc::slot0<void> Slot;

class Trackable : public sigc::trackable
{
    public:
        Trackable( const char* classname ) : sigc::trackable(), _classname( classname ) { };
        virtual ~Trackable() { };
        const char* className() const { return _classname; };

    private:
        const char* _classname;
};

class Size
{
    public:
        Size( int dx = 0, int dy = 0 ) : _dx(dx), _dy(dy) {};
        ~Size() {};
        void set( int dx, int dy ) { _dx = dx; _dy = dy; };
        int width() const { return _dx; };
        int height() const { return _dy; };
    private:
        int _dx;
        int _dy;
};

class Point
{
    public:
        Point( int x = 0, int y = 0 ) : _x(x), _y(y) {};
        ~Point() {};
        void set( int x, int y ) { _x = x; _y = y; };
        void setX( int x ) { _x = x; };
        void setY( int y ) { _y = y; };
        int x() const { return _x; };
        int y() const { return _y; };
    private:
        int _x;
        int _y;
};

class Rect
{
    public:
        Rect( int x = 0, int y = 0, int width = 0, int height = 0) : _x(x), _y(y), _dx(width), _dy(height) {};
        ~Rect() {};
        void set( int x = 0, int y = 0, int width = 0, int height = 0 ) { _x=x; _y=y; _dx=width; _dy=height; };
        void setX( int x ) { _x = x; };
        void setY( int y ) { _y = y; };
        void setWidth( int dx ) { _dx = dx; };
        void setHeight( int dy ) { _dy = dy; };
        int x() const { return _x; };
        int y() const { return _y; };
        int width() const { return _dx; };
        int height() const { return _dy; };
        bool contains( const Point& p ) { return p.x() > x() && p.x() < x()+width() && p.y() > y() && p.y() < y()+width(); };
    private:
        int _x;
        int _y;
        int _dx;
        int _dy;
};


class Color
{
    public:
        Color( int r = 255, int g = 255, int b = 255, int a = 255 ) : _r(r), _g(g), _b(b), _a(a) {};
        ~Color() {};
    
        void set( int r, int g, int b, int a = 255 ) { _r = r; _g = g; _b = b; _a = a; };
    
        void red ( int r ) { _r = r; }
        void green ( int g ) { _g = g; }
        void blue ( int b ) { _b = b; }
        void alpha ( int a ) { _a = a; }

        int red() const { return _r; };
        int green() const { return _g; };
        int blue() const { return _b; };
        int alpha() const { return _a; };
    
    private:
        int _r;
        int _g;
        int _b;
        int _a;
};

inline ostream& operator<<( ostream& s, const Color& color )
{
    return s << "(RGBA " << color.red() << "," << color.green() << "," << color.blue() << "," << color.alpha() << ")";
}
inline ostream& operator<<( ostream& s, const Point& point )
{
    return s << "(" << point.x() << "," << point.y() << ")";
}
inline ostream& operator<<( ostream& s, const Size& size )
{
    return s << "(" << size.width() << "*" << size.height() << ")";
}
inline ostream& operator<<( ostream& s, const Rect& rect )
{
    return s << "(" << rect.x() << "," << rect.y() << "*" << rect.width() << "," << rect.height() << ")";
}
}

#endif
