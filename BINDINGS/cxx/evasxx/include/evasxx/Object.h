#ifndef EVAS_OBJECT_H
#define EVAS_OBJECT_H

#include "Event.h"

/* STL */
#include <string>

/* EFLxx */
#include <eflxx/Eflxx.h>
//#include "Canvas.h"

/* EFL */
#include <Evas.h>

using std::string;

namespace Evasxx {

/* forward declarations */
class Smart;
class Canvas;
  
/*!
 * An Evas Object wrapper
 *
 * This base class handles most of the common operation on Evas objects.
 */
class Object
{
  //friend class Canvas; // needed?
  friend class EdjePart;   // needed?

protected:
  /*!
   * @brief The explicit protected default constructor.
   *
   * Not public, because a Object should no be created by the user.
   */
  Object ();

public:
  virtual ~Object();
  bool operator==(const Object& rhs) {
    return rhs.o == o;
  };

  /*!
   * @brief Gives access to the wrapped C type.
   * Be very careful with it as the pointer may be freed if the C++ class is
   * destroyed and your pointer then hangs around!
   * @return Wrapped Evas_Object pointer.
   */
  Evas_Object* obj() const {
    return o;
  };

  /* event signals */
  sigc::signal <void, const MouseInEvent&> signalHandleMouseIn; /**< Mouse In Event */
  sigc::signal <void, const MouseOutEvent&> signalHandleMouseOut; /**< Mouse Out Event */
  sigc::signal <void, const MouseDownEvent&> signalHandleMouseDown; /**< Mouse Button Down Event */
  sigc::signal <void, const MouseUpEvent&> signalHandleMouseUp; /**< Mouse Button Up Event */
  sigc::signal <void, const MouseMoveEvent&> signalHandleMouseMove; /**< Mouse Move Event */
  sigc::signal <void, const MouseWheelEvent&> signalHandleMouseWheel; /**< Mouse Wheel Event */
  sigc::signal <void> signalHandleFree; /**< Object Being Freed (Called after Del) */
  sigc::signal <void, const KeyDownEvent&> signalHandleKeyDown; /**< Key Press Event */
  sigc::signal <void, const KeyUpEvent&> signalHandleKeyUp; /**< Key Release Event */
  sigc::signal <void> signalHandleFocusIn; /**< Focus In Event */
  sigc::signal <void> signalHandleFocusOut; /**< Focus Out Event */
  sigc::signal <void> signalHandleShow; /**< Show Event */
  sigc::signal <void> signalHandleHide; /**< Hide Event */
  sigc::signal <void> signalHandleMove; /**< Move Event */
  sigc::signal <void> signalHandleResize; /**< Resize Event */
  sigc::signal <void> signalHandleRestack; /**< Restack Event */
  sigc::signal <void> signalHandleDel; /**< Object Being Deleted (called before Free) */
  sigc::signal <void> signalHandleHold; /**< Events go on/off hold */
  sigc::signal <void> signalHandleChangedSizeHints; /**< Size hints changed event */
  sigc::signal <void> signalHandleImagePreload; /**< Image as been preloaded */

  /* Name */
  const std::string getName() const;
  void setName( const std::string& name );

  const std::string getType () const;

  /* Geometry */
  virtual const Eflxx::Size getSize() const;
  virtual void move( const Eflxx::Point& );

  /*!
   * @brief Resize geometry of the EvasImage.
   *
   * @param fill Not used here; only used in child classes.
   */
  virtual void resize( const Eflxx::Size &size, bool fill = true  );

  /*!
   * @brief Relative resize of the current geometry
   *
   * @param factor The resize factor that is multiplied with width and height.
   */
  virtual void rresize( double factor );

  virtual Eflxx::Rect getGeometry() const;
  virtual void setGeometry( const Eflxx::Rect& rect );

  const Eflxx::Size getMinHintSize () const;
  void setMinHintSize (const Eflxx::Size &size);
  
  const Eflxx::Size getMaxHintSize () const;
  void setMaxHintSize (const Eflxx::Size &size);
  
  const Eflxx::Size getRequestHintSize () const;
  void setRequestHintSize (const Eflxx::Size &size);
  
  const Eflxx::Size getAspectHintSize (Evas_Aspect_Control &outAspect) const;
  void setAspectHintSize (Evas_Aspect_Control aspect, const Eflxx::Size &size);
  
  void getAlignHintSize (double &outX, double &outY) const;
  void setAlignHintSize (double x, double y);
  
  void getWeightHintSize (double &outX, double &outY) const;
  void setWeightHintSize (double x, double y);
  
  const Eflxx::Padding getPaddingHintSize () const;
  void setPaddingHintSize (const Eflxx::Padding &border);
  
  /* Clipping */
  virtual Object* getClip() const;
  virtual void setClip( Object* object );

  /* Color */
  virtual Eflxx::Color getColor() const;
  virtual void setColor( const Eflxx::Color& color );

  /* Antialias */
  virtual void setAntiAliased( bool );
  virtual bool isAntiAliased() const;

  /* Render Operation */
  //FIXME add render_op

  /* Z */
  virtual int getLayer() const;
  virtual void setLayer( int layer );
  virtual void raise();
  virtual void lower();
  virtual void stackAbove( const Object* obj );
  virtual void stackBelow( const Object* obj );

  /* Visibility */
  virtual void show();
  virtual void hide();
  virtual bool isVisible() const;
  virtual void setVisible( bool visibility );

  /* Focus */
  virtual void setFocus( bool focus );
  virtual bool hasFocus() const;

  void setEventsPass (bool pass);
  bool getEventsPass () const;
  void setEventsRepeat (bool repeat);
  bool getEventsRepeat () const;
  void setEventsPropagate (bool prop);
  bool getEventsPropagate () const;
  
  /*!
   * @brief Gets the smart parent.
   *
   * You should delete the returned pointer if you don't longer need it. This
   * will only delete the wrapper object. The C object is still available.
   *
   * @param obj the Object you want to get the parent
   * @return Returns the smart parent of @a obj, or NULL if @a obj is not a smart member of another Evas_Object
   *
   * @todo Support this with CountedPtr?
   */
  Object *getParent (const Object &obj);  

  // TODO: implement
  Smart *getSmart ();
  
  /*!
   * @brief C object wrapper factory method.
   *
   * For internal usage only! This return a new allocated Object that holds
   * the wrapped Evas_Object variable. With a delete on this object the wrapped
   * C type won't be freed.
   *
   * @param o The C to to be wrapped.
   * @return The wrapped C++ type.
   */
  static Object *wrap( Evas_Object* o );

  /*!
   * @brief Get the linked C++ object from a C object.
   *
   * For internal usage only! This helps to wrap the C API. In the constructor
   * the string "obj_c++" is connects with the C++ object back to the wrapped
   * C type. This allows to get the C++ object from all C types that are created
   * with the C++ API. If the C type is created by some other way (e.g. loading
   * with Edje) that this won't work as the backlink isn't created. For this case
   * use the wrap() factory function.
   *
   * @param evas_object The linked C object
   * @return The linked C++ object
   */
  static Object* objectLink( Evas_Object* evas_object = 0 );
  
  /*!
   * @@see objectLink but with const variables
   */
  static const Object* objectLink( const Evas_Object* evas_object = 0 );
  
  /*!
   * Retrieves the Canvas that the current object is on.
   *
   * @return  The Canvas that the object is on.
   */
  Eflxx::CountedPtr <Evasxx::Canvas> getCanvas ();

private:
  
  void registerCallbacks ();
  void unregisterCallbacks ();
  static void dispatcher( void *data, Evas *evas, Evas_Object *evas_object, void *event_info);

protected:
  // TODO: think about makeing init private and call it from Object constructor
  // in the current design there's no need to call from child...
  void init ();

  Evas_Object* o;

private:

  /*!
   * @brief Construct for existing Evas_Object
   *
   * 	This constructor is private and is only called by the wrap function.
   */
  Object (Evas_Object *eo);

  Object( const Object& ); // disable copy constructor
  bool operator=(const Object& ); // disable assignment operator
};

inline ostream& operator<<( ostream& s, const Object& obj )
{
  return s << "<" << " Object '" << obj.getName() << "' @ " << obj.getGeometry() << ">";
}

} // end namespace Evasxx

#endif // EVAS_OBJECT_H
