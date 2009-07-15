#ifndef EVASOBJECT_H
#define EVASOBJECT_H

#include "EvasEvent.h"

/* STL */
#include <string>

/* EFLxx */
#include <eflxx/eflpp_common.h>

/* EFL */
#include <Evas.h>

using std::string;

namespace efl {

/* forward declarations */
class EvasSmart;
  
/*!
 * An Evas Object wrapper
 *
 * This base class handles most of the common operation on Evas objects.
 */
class EvasObject
{
  friend class EvasCanvas; // needed?
  friend class EdjePart;   // needed?

protected:
  /*!
   * @brief The explicit protected default constructor.
   *
   * Not public, because a EvasObject should no be created by the user.
   */
  EvasObject ();

public:
  virtual ~EvasObject();
  bool operator==(const EvasObject& rhs) {
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
  sigc::signal <void, const EvasMouseInEvent&> signalHandleMouseIn; /**< Mouse In Event */
  sigc::signal <void, const EvasMouseOutEvent&> signalHandleMouseOut; /**< Mouse Out Event */
  sigc::signal <void, const EvasMouseDownEvent&> signalHandleMouseDown; /**< Mouse Button Down Event */
  sigc::signal <void, const EvasMouseUpEvent&> signalHandleMouseUp; /**< Mouse Button Up Event */
  sigc::signal <void, const EvasMouseMoveEvent&> signalHandleMouseMove; /**< Mouse Move Event */
  sigc::signal <void, const EvasMouseWheelEvent&> signalHandleMouseWheel; /**< Mouse Wheel Event */
  sigc::signal <void> signalHandleFree; /**< Object Being Freed (Called after Del) */
  sigc::signal <void, const EvasKeyDownEvent&> signalHandleKeyDown; /**< Key Press Event */
  sigc::signal <void, const EvasKeyUpEvent&> signalHandleKeyUp; /**< Key Release Event */
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
  virtual const Size getSize() const;
  virtual void move( const Point& );

  /*!
   * @brief Resize geometry of the EvasImage.
   *
   * @param fill Not used here; only used in child classes.
   */
  virtual void resize( const Size &size, bool fill = true  );

  /*!
   * @brief Relative resize of the current geometry
   *
   * @param factor The resize factor that is multiplied with width and height.
   */
  virtual void rresize( double factor );

  virtual Rect getGeometry() const;
  virtual void setGeometry( const Rect& rect );

  const Size getMinHintSize () const;
  void setMinHintSize (const Size &size);
  
  const Size getMaxHintSize () const;
  void setMaxHintSize (const Size &size);
  
  const Size getRequestHintSize () const;
  void setRequestHintSize (const Size &size);
  
  const Size getAspectHintSize (Evas_Aspect_Control &outAspect) const;
  void setAspectHintSize (Evas_Aspect_Control aspect, const Size &size);
  
  void getAlignHintSize (double &outX, double &outY) const;
  void setAlignHintSize (double x, double y);
  
  void getWeightHintSize (double &outX, double &outY) const;
  void setWeightHintSize (double x, double y);
  
  const Padding getPaddingHintSize () const;
  void setPaddingHintSize (const Padding &border);
  
  /* Clipping */
  virtual EvasObject* getClip() const;
  virtual void setClip( EvasObject* object );

  /* Color */
  virtual Color getColor() const;
  virtual void setColor( const Color& color );
  virtual int getColorInterpolation() const;
  virtual void setColorInterpolation( int );

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
  
  /*!
   * @brief Gets the smart parent.
   *
   * You should delete the returned pointer if you don't longer need it. This
   * will only delete the wrapper object. The C object is still available.
   *
   * @param obj the EvasObject you want to get the parent
   * @return Returns the smart parent of @a obj, or NULL if @a obj is not a smart member of another Evas_Object
   *
   * @todo Support this with CountedPtr?
   */
  EvasObject *getParent (const EvasObject &obj);  

  // TODO: implement
  EvasSmart *getEvasSmart ();
  
  /*!
   * @brief C object wrapper factory method.
   *
   * For internal usage only! This return a new allocated EvasObject that holds
   * the wrapped Evas_Object variable. With a delete on this object the wrapped
   * C type won't be freed.
   *
   * @param o The C to to be wrapped.
   * @return The wrapped C++ type.
   */
  static EvasObject *wrap( Evas_Object* o );

private:
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
  static EvasObject* objectLink( Evas_Object* evas_object = 0 );

  /*!
   * @@see objectLink but with const variables
   */
  static const EvasObject* objectLink( const Evas_Object* evas_object = 0 );

  void registerCallbacks();
  static void dispatcher( void *data, Evas *evas, Evas_Object *evas_object, void *event_info);

protected:
  void init ();

  Evas_Object* o;
  bool mFree; /// @deprecated

private:

  /*!
   * @brief Construct for existing Evas_Object
   *
   * 	This constructor is private and is only called by the wrap function.
   */
  EvasObject (Evas_Object *eo);

  EvasObject( const EvasObject& ); // disable copy constructor
  bool operator=(const EvasObject& ); // disable assignment operator
};

inline ostream& operator<<( ostream& s, const EvasObject& obj )
{
  return s << "<" << " Object '" << obj.getName() << "' @ " << obj.getGeometry() << ">";
}

} // end namespace efl

#endif // EFLPP_EVASOBJECT_H
