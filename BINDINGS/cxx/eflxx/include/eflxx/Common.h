#ifndef EFL_COMMON_H
#define EFL_COMMON_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "System.h"
#include "DebugInternal.h"
#include <sigc++/sigc++.h>
#include <iostream>

/* EFL */
#include <Evas.h>

using std::ostream;
using std::istream;

namespace Eflxx {

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

class Point;
class Size
{
    public:
        Size( Evas_Coord dx = 0, Evas_Coord dy = 0 ) : _dx(dx), _dy(dy) {};
        //Size (Point p) : _dx (p.x ()), _dy (p.y ()) {};
        ~Size() {};
        void set( Evas_Coord dx, Evas_Coord dy ) { _dx = dx; _dy = dy; };
        void width( Evas_Coord width ) { _dx = width; }
        void heigth( Evas_Coord height ) { _dy = height; }
        Evas_Coord width() const { return _dx; };
        Evas_Coord height() const { return _dy; };
    private:
        Evas_Coord _dx;
        Evas_Coord _dy;
};

class Point
{
    public:
        Point( Evas_Coord x = 0, Evas_Coord y = 0 ) : _x(x), _y(y) {};
        Point (Size s) : _x (s.width ()), _y (s.height ()) {};
        ~Point() {};
        void set( Evas_Coord x, Evas_Coord y ) { _x = x; _y = y; };
        void x( Evas_Coord x ) { _x = x; };
        void y( Evas_Coord y ) { _y = y; };
        Evas_Coord x() const { return _x; };
        Evas_Coord y() const { return _y; };
    private:
        Evas_Coord _x;
        Evas_Coord _y;
};

class Rect
{
    public:
        Rect( Evas_Coord x = 0, Evas_Coord y = 0, Evas_Coord width = 0, Evas_Coord height = 0) : _x(x), _y(y), _dx(width), _dy(height) {};
        ~Rect() {};
        void set( Evas_Coord x = 0, Evas_Coord y = 0, Evas_Coord width = 0, Evas_Coord height = 0 ) { _x=x; _y=y; _dx=width; _dy=height; };
        void x( Evas_Coord x ) { _x = x; };
        void y( Evas_Coord y ) { _y = y; };
        void width( Evas_Coord dx ) { _dx = dx; };
        void height( Evas_Coord dy ) { _dy = dy; };
        Evas_Coord x() const { return _x; };
        Evas_Coord y() const { return _y; };
        Evas_Coord width() const { return _dx; };
        Evas_Coord height() const { return _dy; };
        Point pos() const {return Point (_x, _y); }
        Size size() const {return Size (_dx, _dy); }
        bool contains( const Point& p ) { return p.x() > x() && p.x() < x()+width() && p.y() > y() && p.y() < y()+width(); };
    private:
        Evas_Coord _x;
        Evas_Coord _y;
        Evas_Coord _dx;
        Evas_Coord _dy;
};

class Padding
{
    public:
        Padding( Evas_Coord l = 0, Evas_Coord r = 0, Evas_Coord t = 0, Evas_Coord b = 0) : _l(l), _r(r), _t(t), _b(b) {};
        ~Padding() {};
        void set( Evas_Coord l = 0, Evas_Coord r = 0, Evas_Coord t = 0, Evas_Coord b = 0 ) { _l=l; _r=r; _r=r; _b=b; };
        void left( Evas_Coord l ) { _l = l; };
        void right( Evas_Coord r ) { _r = r; };
        void top( Evas_Coord t ) { _t = t; };
        void bottom( Evas_Coord b ) { _b = b; };
        Evas_Coord left() const { return _l; };
        Evas_Coord right() const { return _r; };
        Evas_Coord top() const { return _t; };
        Evas_Coord bottom() const { return _b; };

    private:
        Evas_Coord _l;
        Evas_Coord _r;
        Evas_Coord _t;
        Evas_Coord _b;
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
