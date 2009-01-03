#ifndef EFLPP_EDJE_H
#define EFLPP_EDJE_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* EFL++ */
#include <eflpp_evas.h>
#include <eflpp_evasutils.h>
#include <eflpp_common.h>
#include <eflpp_countedptr.h>
#include <eflpp_evascanvas.h>
#include "eflpp_edjepart.h"
#include "eflpp_evasedje.h"

/* STD */
#include <string>

/* EFL */
#include <Edje.h>

using std::string;

/**
 * C++ Wrapper for the Enlightenment Edje Library (EDJE)
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace efl {

class Edje
{
  public:
    static int init();
    static int shutdown();

    static double frametime();
    static void setFrametime( double t );

    static void freeze();
    static void thaw();

    static const string fontset();
    static void setFontSet( const string &fonts );
    
    static double scale ();
    static void setScale (double scale);
    
    static Eina_List *collection( const string &file ); // TODO: wrap Eina_List
    static void freeCollection( Eina_List *lst ); // TODO: wrap Eina_List
    
    static bool exitsFileGroup (const string &file, const string &glob);

    static const string data( const string &file, const string &key );

    static void setFileCache( int count );
    static int fileCache();
    static void flushFileCache();

    static void setCollectionCache( int count );
    static int collectionCache();
    static void flushCollectionCache();

    static void setColorClass( const string &colorclass, const Color& object, const Color& outline, const Color& shadow );
    static void delColorClass( const string &colorclass );
    
    static Eina_List *listColorclass( ); // TODO: wrap Eina_List

    static void setTextClass( const string &textclass, const string &font, int size );
    static void delTextClass (const string &textclass);

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
    Edje();
    Edje( const Edje& );
    ~Edje();
};

}

#endif // EFLPP_EDJE_H
