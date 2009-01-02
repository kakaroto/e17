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

/* EFL */
#include <Edje.h>

/**
 * C++ Wrapper for the Enlightenment Edje Library (EDJE)
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace efl {

//typedef std::map<const char*, EdjePart*> EdjePartMap;
  
//===============================================================================================
// Edje
//===============================================================================================
  
// TODO: why is the implementation empty?

class Edje
{
  public:
    static int init();
    static int shutdown();

    static double frametime();
    static void setFrametime( double );

    static void freeze();
    static void thaw();

    static const char* fontset();
    static void setFontSet( const char* fonts );

    static EvasList<const char>* collection( const char* filename );
    static void freeCollection( EvasList<const char>* );

    static const char* data( const char* filename, const char* key );

    static void setFileCache( int size );
    static int fileCache();
    static void flushFileCache();

    static void setCollectionCache( int size );
    static int collectionCache();
    static void flushCollectionCache();

    static void setColorClass( const char* classname, const Color& object, const Color& outline, const Color& shadow );
    static void setTextClass( const char* classname, const char* fontname, int size );

    /*
    EAPI void         edje_extern_object_min_size_set (Evas_Object *obj, Evas_Coord minw, Evas_Coord minh);
    EAPI void         edje_extern_object_max_size_set (Evas_Object *obj, Evas_Coord maxw, Evas_Coord maxh);
    */
  private:
    Edje();
    Edje( const Edje& );
    ~Edje();
};

}

#endif // EFLPP_EDJE_H
