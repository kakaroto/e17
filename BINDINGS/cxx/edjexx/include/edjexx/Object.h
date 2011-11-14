#ifndef EDJEXX_OBJECT_H
#define EDJEXX_OBJECT_H

/* STL */
#include <string>
#include <map>

/* EFL++ */
#include <eflxx/Eflxx.h>
#include <evasxx/Evasxx.h>

/* local */
#include "Part.h"

/* EFL */
#include <Edje.h>

namespace Edjexx {

typedef sigc::signal<void, const std::string, const std::string> SignalSignal;
typedef sigc::slot2<void, const std::string, const std::string> SignalSlot;
  
class Object : public Evasxx::Object
{
public:
  Object( Evasxx::Canvas &canvas );
  Object( Evasxx::Canvas &canvas, const std::string &filename, const std::string &groupname );
  Object( Evasxx::Canvas &canvas, const Eflxx::Point &pos, const std::string &filename, const std::string &groupname );

  virtual ~Object();

  /*
  EAPI void         edje_object_signal_callback_add (Evas_Object *obj, const char *emission, const char *source, void (*func) (void *data, Evas_Object *obj, const char *emission, const char *source), void *data);

  EAPI void        *edje_object_signal_callback_del (Evas_Object *obj, const char *emission, const char *source, void (*func) (void *data, Evas_Object *obj, const char *emission, const char *source));

  EAPI void         edje_object_text_change_cb_set  (Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, const char *part), void *data);

  EAPI void         edje_object_message_send           (Evas_Object *obj, Edje_Message_Type type, int id, void *msg);

  EAPI void         edje_object_message_handler_set    (Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg), void *data);

  EAPI void         edje_object_message_signal_process (Evas_Object *obj);

  EAPI void         edje_message_signal_process        (void);
  */

  void setFile( const std::string &filename, const std::string &groupname );

  void setPlaying( bool b );
  bool isPlaying() const;
  void setAnimated( bool b );
  bool isAnimated() const;

  // TODO: implement
  int freeze();
  int thaw();

  // TODO: implement
  void setColorClass( const std::string &colorclass, const Eflxx::Color& object, const Eflxx::Color& outline, const Eflxx::Color& shadow );
  void setTextClass( const std::string &textclass, const std::string &fontname, int size );

  void setMinimumSize( const Eflxx::Size& size );
  void setMaximumSize( const Eflxx::Size& size );
  Eflxx::Size getMinimalSize() const;
  Eflxx::Size getMaximumSize() const;

  // TODO: implement
  void recalculateLayout();

  bool hasPart( const std::string &partname ) const;
  Part &operator[]( const std::string &partname );

  /*!
   * @param partname Access a Part in the Object.
   * @throw PartNotExistingException
   */
  Part &getPart( const std::string &partname );

  /* signals and slots */
  void connect (const std::string &emission, const std::string &source, const SignalSlot& slot);
	void disconnect (const std::string &emission, const std::string &source);
	void disconnectAll ();
  void emit (const std::string &emission, const std::string &source);

  static Object *wrap( Evas_Object* o );

private:
  // TODO: wrap Edje_Message_Type to avoid include of C header
  static void _edje_message_handler_callback( void* data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg );
  static void _edje_signal_handler_callback( void *data, Evas_Object *obj, const char *emission, const char *source );

  Object();
  Object( const Object& );

  Object( Evas_Object* object );

  void deleteAllParts ();

  bool operator=( const Object& );
  bool operator==( const Object& );

	std::map <std::pair <std::string, std::string>, SignalSignal*> mSignalList;
  std::map <std::string, Part*> mPartList;

  bool mFree;
};


} // end namespace Edjexx

#endif // EDJEXX_OBJECT_H
