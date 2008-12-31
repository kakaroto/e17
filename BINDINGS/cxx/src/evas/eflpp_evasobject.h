#ifndef EFLPP_EVASOBJECT_H
#define EFLPP_EVASOBJECT_H

#include "eflpp_evasevent.h"

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_countedptr.h>

/* EFL */
#include <Evas.h>

using std::string;

namespace efl {
  
/**
 * An Evas Object wrapper
 *
 * This base class handles most of the common operation on Evas objects.
 */
class EvasObject
{
  friend class EvasCanvas;
  friend class EdjePart;

  protected:
    EvasObject () {}

  public:    
    virtual ~EvasObject();
    bool operator==(const EvasObject& rhs) { return rhs.o == o; };

    /**
     * @brief Gives access to the wrapped C type.
     * Be very careful with it as the pointer may be freed if the C++ class is 
     * destroyed and your pointer then hangs around!
     * @return Wrapped EvasObject pointer.
     */
    Evas_Object* obj() const { return o; };
    
    /* event signals */
    sigc::signal <void, const EvasMouseInEvent&> signalHandleMouseIn;
    sigc::signal <void, const EvasMouseOutEvent&> signalHandleMouseOut;
    sigc::signal <void, const EvasMouseDownEvent&> signalHandleMouseDown;
    sigc::signal <void, const EvasMouseUpEvent&> signalHandleMouseUp;
    sigc::signal <void, const EvasMouseMoveEvent&> signalHandleMouseMove;
    sigc::signal <void, const EvasMouseWheelEvent&> signalHandleMouseWheel;
    sigc::signal <void> signalHandleFree;
    sigc::signal <void, const EvasKeyDownEvent&> signalHandleKeyDown;
    sigc::signal <void, const EvasKeyUpEvent&> signalHandleKeyUp;
    sigc::signal <void> signalHandleFocusIn;
    sigc::signal <void> signalHandleFocusOut;
    sigc::signal <void> signalHandleShow;
    sigc::signal <void> signalHandleHide;
    sigc::signal <void> signalHandleMove;
    sigc::signal <void> signalHandleResize;
    sigc::signal <void> signalHandleRestack;

    /* Name */
    const char* name() const;
    void setName( const char* name );
    
    const char* type () const;

    /* Geometry */
    virtual const Size size() const;
    virtual void move( const Point& );
    virtual void move( int x = 0, int y = 0 );
    virtual void resize( int width, int height, bool ff = true );    /* gets overloaded by Image */
    virtual void resize( const Size&, bool ff = true );
    virtual void rresize( double factor, bool ff = true );
    virtual Rect geometry() const;
    virtual void setGeometry( int x, int y, int width, int height );
    virtual void setGeometry( const Rect& rect );

    /* Clipping */
    virtual EvasObject* clip() const;
    virtual void setClip( EvasObject* object );

    /* Color */
    virtual Color color() const;
    virtual void setColor( int r, int g, int b, int alpha );
    virtual void setColor( const Color& color );
    virtual int colorInterpolation() const;
    virtual void setColorInterpolation( int );

    /* Antialias */
    virtual void setAntiAliased( bool );
    virtual bool isAntiAliased() const;

    /* Render Operation */
    //FIXME add render_op

    /* Z */
    virtual int layer() const;
    virtual void setLayer( int layer );
    virtual void raise();
    virtual void lower();
    virtual void stackAbove( const EvasObject* obj );
    virtual void stackBelow( const EvasObject* obj );

    /* Visibility */
    virtual void show();
    virtual void hide();
    virtual bool isVisible() const;
    virtual void setVisible( bool visibility );

    /* Focus */
    virtual void setFocus( bool focus );
    virtual bool hasFocus() const;
    
    /**
     * @brief C object wrapper factory method
     * Only for internal usage!
     */
    static EvasObject *wrap( Evas_Object* o );

  private:
    static EvasObject* objectLink( Evas_Object* evas_object = 0 ); // TODO: needed?
    static const EvasObject* objectLink( const Evas_Object* evas_object = 0 ); // TODO: needed?
    void registerCallbacks();
    static void dispatcher( void *data, Evas *evas, Evas_Object *evas_object, void *event_info);

  protected:
    Evas_Object* o;
    bool mManaged;

    void init (const char *name);

  private:
    //EvasObject(); // disable default constructor
    
    /*!
     *  Construct from existing Evas_Object
     */
    EvasObject (Evas_Object *eo);
    
    EvasObject( const EvasObject& ); // disable copy constructor
    bool operator=(const EvasObject& ); // disable assignment operator
};

} // end namespace efl

#endif // EFLPP_EVASOBJECT_H
