#ifndef EFLPP_EDJE_H
#define EFLPP_EDJE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* EFL++ */
#include <evasxx/Utils.h>
#include <eflxx/Common.h>

/* STD */
#include <string>

/* EFL */
#include <Edje.h>

/**
 * C++ Wrapper for the Enlightenment Base Library (EDJE)
 *
 */

namespace Edjexx {

class Base
{
public:
  static int init();
  static int shutdown();

  static double getFrametime();
  static void setFrametime( double t );

  static void freeze();
  static void thaw();

  static const std::string getFontset();
  static void setFontSet( const std::string &fonts );

  static double getScale ();
  static void setScale (double scale);

  static Eina_List *collection( const std::string &file ); // TODO: wrap Eina_List
  static void freeCollection( Eina_List *lst ); // TODO: wrap Eina_List

  static bool exitsFileGroup (const std::string &file, const std::string &glob);

  static const std::string data( const std::string &file, const std::string &key );

  static void setFileCache( int count );
  static int getFileCache();
  static void flushFileCache();

  static void setCollectionCache( int count );
  static int getCollectionCache();
  static void flushCollectionCache();

  static void setColorClass( const std::string &colorclass, const Eflxx::Color& object, const Eflxx::Color& outline, const Eflxx::Color& shadow );
  static void delColorClass( const std::string &colorclass );

  static Eina_List *listColorclass( ); // TODO: wrap Eina_List

  static void setTextClass( const std::string &textclass, const std::string &font, int size );
  static void delTextClass (const std::string &textclass);

  static Eina_List *listTextClass (); // TODO: wrap Eina_List

  /*
  TODO: what do do with this functions?; what are their usage?
  EAPI void         edje_extern_object_min_size_set (Evas_Object *obj, Evas_Coord minw, Evas_Coord minh);
  EAPI void         edje_extern_object_max_size_set (Evas_Object *obj, Evas_Coord maxw, Evas_Coord maxh);
  EAPI void         edje_extern_object_aspect_set(Evas_Object *obj, Edje_Aspect_Control aspect, Evas_Coord aw, Evas_Coord ah);
  EAPI void         edje_box_layout_register(const char *name, Evas_Object_Box_Layout func, void *(*layout_data_get)(void *), void (*layout_data_free)(void *), void (*free_data)(void *), void *data);
  */

  static void processSignalMessage ();

private:
  Base();
  Base( const Base& );
  ~Base();
};

}

#endif // EFLPP_EDJE_H
