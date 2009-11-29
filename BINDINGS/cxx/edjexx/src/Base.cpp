#include "../include/edjexx/Base.h"
#include <eflxx/DebugInternal.h>

#include <iostream>
#include <cassert>
#include <string>
#include <cstring>

using namespace std;

namespace Edjexx {

int Base::init()
{
  return edje_init ();
}

int Base::shutdown()
{
  return edje_shutdown ();
}

double Base::getFrametime()
{
  return edje_frametime_get ();
}

void Base::setFrametime( double t )
{
  edje_frametime_set (t);
}

void Base::freeze()
{
  edje_freeze ();
}

void Base::thaw()
{
  edje_thaw ();
}

const string Base::getFontset()
{
  return edje_fontset_append_get ();
}

void Base::setFontSet( const string &fonts )
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

Eina_List *Base::collection( const string &file )
{
  return edje_file_collection_list (file.c_str ());
}

void Base::freeCollection( Eina_List *lst )
{
  edje_file_collection_list_free (lst);
}

bool Base::exitsFileGroup (const string &file, const string &glob)
{
  return edje_file_group_exists (file.c_str (), glob.c_str ());
}

const string Base::data( const string &file, const string &key )
{
  return edje_file_data_get (file.c_str (), key.c_str ());
}

void Base::setFileCache( int count )
{
  edje_file_cache_set (count);
}

int Base::getFileCache()
{
  return edje_file_cache_get ();
}

void Base::flushFileCache()
{
  edje_file_cache_flush ();
}

void Base::setCollectionCache( int count )
{
  edje_collection_cache_set (count);
}

int Base::getCollectionCache()
{
  return edje_file_cache_get ();
}

void Base::flushCollectionCache()
{
  edje_file_cache_flush ();
}

void Base::setColorClass( const string &colorclass, const Eflxx::Color& object, const Eflxx::Color& outline, const Eflxx::Color& shadow )
{
  edje_color_class_set (colorclass.c_str (),
                        object.red (), object.green (), object.blue (), object.alpha (),
                        outline.red (), outline.green (), outline.blue (), outline.alpha (),
                        shadow.red (), shadow.green (), shadow.blue (), shadow.alpha ()
                       );
}

void Base::delColorClass( const string &colorclass )
{
  edje_color_class_del (colorclass.c_str ());
}

Eina_List *Base::listColorclass( )
{
  return edje_color_class_list();
}

void Base::setTextClass( const string &textclass, const string &font, Evas_Font_Size size )
{
  edje_text_class_set (textclass.c_str (), font.c_str (), size);
}

void Base::delTextClass (const string &textclass)
{
  edje_text_class_del (textclass.c_str ());
}

Eina_List *Base::listTextClass ()
{
  return edje_text_class_list ();
}

void Base::processSignalMessage ()
{
  edje_message_signal_process ();
}

} // end namespace Edjexx

