#ifndef EFLPP_EVASCANVAS_H
#define EFLPP_EVASCANVAS_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_countedptr.h>
#include "eflpp_evasobject.h"

/* EFL */
#include <Evas.h>

using std::string;

namespace efl {

/**
 * An Evas Canvas Wrapper
 */
class EvasCanvas : public Trackable
{
  public:
    EvasCanvas( int width, int height );
    EvasCanvas( Evas* evas );
    EvasCanvas();
    ~EvasCanvas();

    Evas* obj() const { return o; };

    /* Output Methods */
    int lookupRenderMethod( const char* method );

    /** @brief Set the Evas output method.
     *  This does include a call to lookupRenderMethod().
     *  @param method Name of the output method. (See C API docs).
     */
    bool setOutputMethod( const char* method );

    void resize( int width, int height );
    Size size() const;
    Rect geometry() const;
    void setViewport( int x, int y, int width, int height );
    Rect viewport() const;

    /* Font Path */
    void appendFontPath( const char* path );
    void prependFontPath( const char* path );
    void clearFontPath();
    const Evas_List* fontPath();

    /* Font Cache */
    void setFontCache( unsigned int size );
    void flushFontCache();
    int fontCache();

    /* Image Cache */
    void setImageCache( unsigned int size );
    void flushImageCache();
    void reloadImageCache();
    int imageCache();

    /* Object queries */
    EvasObject* focusedObject() const;
    EvasObject* objectAtTop() const;
    EvasObject* objectAtBottom() const;

  protected:
    Evas* o;
};

} // end namespace efl

#endif // EFLPP_EVASCANVAS_H
