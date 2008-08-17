/* GeistObject.h

   Copyright (C) 1999,2000 Tom Gilbert.

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies of the Software and its documentation and acknowledgment shall
   be given in the documentation and software packages that this Software was
   used.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#ifndef GEIST_OBJECT_H
#define GEIST_OBJECT_H

#include "geist.h"

// exceptions

// can't add or remove children to a leaf
class eNoChildren:public eGeneric { };
// Object with no parent that really needs one.
class eDisownedObject:public eGeneric { };


typedef int Coord;
struct Point
{
   Point()
   {
      _x = 0;
      _y = 0;
   };

   Point(Coord x, Coord y)
   {
      _x = x;
      _y = y;
   };

   Coord _x;
   Coord _y;
};

struct Rect
{
   Rect() : _p(0,0)
   {
      _w = 0;
      _h = 0;
   };

   Rect(Point p, Coord w, Coord h) : _p(p._x, p._y)
   {
      _w = w;
      _h = h;
   };

   Point _p;
   Coord _w;
   Coord _h;
};

class GeistObject
{
    public:

        virtual ~GeistObject();

        // defined in the base class, all objects have a name - can be
        // overridden in case a name change requires a rerender or something
        virtual void set_name(string name)
        {
           _name = name;
        };
        virtual string get_name() const
        {
           return _name;
        };

        // child access and management
        virtual void add_child(GeistObject *)
        {
           throw eNoChildren();
        };
        virtual void remove_child(GeistObject *)
        {
           throw eNoChildren();
        };
        virtual GeistObject *get_parent() const
        {
           return _parent;
        };

        // all objects will just call parent->get_image(), the topmost item
        // should be a document with an image in it, which will be returned.
        virtual Imlib_Image get_image()
        {
           if (_parent)
              return _parent->get_image();
           throw eDisownedObject();
        };
        virtual void show();
        virtual void hide();
        virtual void render(Imlib_Image im) = 0;
        virtual void render_partial(Imlib_Image im, Rect rect) = 0;
        virtual void render();
        virtual void render_partial(Rect rect);
        virtual void move(Point p);
        virtual void set_rect(Rect rect);
        // children send the dirty up to their parent, who pass it up, up, up
        // until it gets to the topmost parent (generally a GeistDocument) who
        // processes the rect.
        virtual void dirty()
        {
           if (_parent)
              _parent->dirty(_rect);
           else
              throw eDisownedObject();
        };
        virtual void dirty(Rect r)
        {
           if (_parent)
              _parent->dirty(r);
           else
              throw eDisownedObject();
        };
        virtual Rect get_rect() const
        {
           return _rect;
        };

    protected:

        // Protected constructor. You aren't allowed to make one, this is a
        // base class :-)
        GeistObject();

        string _name;
        GeistObject *_parent;
        Rect _rect;
        bool _visible;
};

#endif
