#ifndef EFLPP_EVASEDJE_H
#define EFLPP_EVASEDJE_H

/* STL */
#include <string>

/* EFL++ */
#include <eflxx/eflpp_common.h>
#include <eflxx/CountedPtr.h>

#include <evasxx/EvasObject.h>
#include <evasxx/EvasCanvas.h>
#include "EdjePart.h"

/* EFL */
#include <Edje.h>

using std::string;

namespace efl {

typedef sigc::signal<void, const char*, const char*> EdjeSignalSignal;
typedef sigc::slot2<void, const char*, const char*> EdjeSignalSlot;
  
class EvasEdje : public EvasObject
{
public:
  EvasEdje( EvasCanvas &canvas );
  EvasEdje( EvasCanvas &canvas, const std::string &filename, const std::string &groupname );
  EvasEdje( EvasCanvas &canvas, const Point &pos, const std::string &filename, const std::string &groupname );

  ~EvasEdje();

  /*
  EAPI void         edje_object_signal_callback_add (Evas_Object *obj, const char *emission, const char *source, void (*func) (void *data, Evas_Object *obj, const char *emission, const char *source), void *data);

  EAPI void        *edje_object_signal_callback_del (Evas_Object *obj, const char *emission, const char *source, void (*func) (void *data, Evas_Object *obj, const char *emission, const char *source));

  EAPI void         edje_object_text_change_cb_set  (Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, const char *part), void *data);

  EAPI void         edje_object_message_send           (Evas_Object *obj, Edje_Message_Type type, int id, void *msg);

  EAPI void         edje_object_message_handler_set    (Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg), void *data);

  EAPI void         edje_object_message_signal_process (Evas_Object *obj);

  EAPI void         edje_message_signal_process        (void);
  */

  void setFile( const std::string &filename, const std::string &partname );

  void setPlaying( bool b );
  bool isPlaying() const;
  void setAnimated( bool b );
  bool isAnimated() const;

  // TODO: implement
  int freeze();
  int thaw();

  // TODO: implement
  void setColorClass( const std::string &colorclass, const Color& object, const Color& outline, const Color& shadow );
  void setTextClass( const std::string &textclass, const std::string &fontname, int size );

  void setMinimumSize( const Size& size );
  void setMaximumSize( const Size& size );
  Size getMinimalSize() const;
  Size getMaximumSize() const;

  // TODO: implement
  void recalculateLayout();

  bool hasPart( const std::string &partname ) const;
  CountedPtr <EdjePart> operator[]( const std::string &partname );

  /*!
   * @param partname Access a EdjePart in the EvasEdje.
   * @throw EdjePartNotExistingException
   */
  CountedPtr <EdjePart> getPart( const std::string &partname );

  /* signals and slots */
  void connect( const std::string &emission, const std::string &source, const EdjeSignalSlot& slot );
  void emit( const std::string &emission, const std::string &source );

  static EvasEdje *wrap( Evas_Object* o );

private:
  // TODO: wrap Edje_Message_Type to avoid include of C header
  static void _edje_message_handler_callback( void* data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg );
  static void _edje_signal_handler_callback( void *data, Evas_Object *obj, const char *emission, const char *source );

  EvasEdje();
  EvasEdje( const EvasEdje& );

  EvasEdje( Evas_Object* object );

  bool operator=( const EvasEdje& );
  bool operator==( const EvasEdje& );
};


} // end namespace efl

#endif // EFLPP_EVASEDJE_H
