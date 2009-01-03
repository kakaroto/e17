#include "eflpp_edje.h"
#include <eflpp_debug_internal.h>

#include <iostream>
#include <cassert>
#include <string>
#include <cstring>

using namespace std;

namespace efl {

int Edje::init()
{
    return edje_init ();
}
  
int Edje::shutdown()
{
    return edje_shutdown ();
}

double Edje::frametime()
{
    return edje_frametime_get ();
}

void Edje::setFrametime( double t )
{
    edje_frametime_set (t);
}

void Edje::freeze()
{
    edje_freeze ();
}

void Edje::thaw()
{
    edje_thaw ();
}

const string Edje::fontset()
{
  return edje_fontset_append_get ();
}

void Edje::setFontSet( const string &fonts )
{
    edje_fontset_append_set (fonts.c_str ());
}

double scale ()
{
    return edje_scale_get (); 
}

void setScale (double scale)
{
    edje_scale_set (scale);
}

Eina_List *Edje::collection( const string &file )
{
    return edje_file_collection_list (file.c_str ());
}

void Edje::freeCollection( Eina_List *lst )
{
    edje_file_collection_list_free (lst);
}

bool Edje::exitsFileGroup (const string &file, const string &glob)
{
    return edje_file_group_exists (file.c_str (), glob.c_str ());
}

const string Edje::data( const string &file, const string &key )
{
    return edje_file_data_get (file.c_str (), key.c_str ());  
}

void Edje::setFileCache( int count )
{
    edje_file_cache_set (count);
}

int Edje::fileCache()
{
    return edje_file_cache_get ();
}

void Edje::flushFileCache()
{
    edje_file_cache_flush ();
}

void Edje::setCollectionCache( int count )
{
   edje_collection_cache_set (count);
}

int Edje::collectionCache()
{
  return edje_file_cache_get ();
}

void Edje::flushCollectionCache()
{
  edje_file_cache_flush ();
}

void Edje::setColorClass( const string &colorclass, const Color& object, const Color& outline, const Color& shadow )
{
    edje_color_class_set (colorclass.c_str (), 
                          object.red (), object.green (), object.blue (), object.alpha (),
                          outline.red (), outline.green (), outline.blue (), outline.alpha (),
                          shadow.red (), shadow.green (), shadow.blue (), shadow.alpha ()
                         );
}

void Edje::delColorClass( const string &colorclass )
{
    edje_color_class_del (colorclass.c_str ());
}

Eina_List *Edje::listColorclass( )
{
    return edje_color_class_list(); 
}

void Edje::setTextClass( const string &textclass, const string &font, Evas_Font_Size size )
{
  edje_text_class_set (textclass.c_str (), font.c_str (), size);
}

void Edje::delTextClass (const string &textclass)
{
    edje_text_class_del (textclass.c_str ());
}

Eina_List *Edje::listTextClass ()
{
    return edje_text_class_list ();
}

void Edje::processSignalMessage ()
{
    edje_message_signal_process ();
}

} // end namespace efl

