#include "../include/edjexx/EdjeBase.h"
#include <eflxx/eflpp_debug_internal.h>

#include <iostream>
#include <cassert>
#include <string>
#include <cstring>

using namespace std;

namespace efl {

int EdjeBase::init()
{
  return edje_init ();
}

int EdjeBase::shutdown()
{
  return edje_shutdown ();
}

double EdjeBase::getFrametime()
{
  return edje_frametime_get ();
}

void EdjeBase::setFrametime( double t )
{
  edje_frametime_set (t);
}

void EdjeBase::freeze()
{
  edje_freeze ();
}

void EdjeBase::thaw()
{
  edje_thaw ();
}

const string EdjeBase::getFontset()
{
  return edje_fontset_append_get ();
}

void EdjeBase::setFontSet( const string &fonts )
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

Eina_List *EdjeBase::collection( const string &file )
{
  return edje_file_collection_list (file.c_str ());
}

void EdjeBase::freeCollection( Eina_List *lst )
{
  edje_file_collection_list_free (lst);
}

bool EdjeBase::exitsFileGroup (const string &file, const string &glob)
{
  return edje_file_group_exists (file.c_str (), glob.c_str ());
}

const string EdjeBase::data( const string &file, const string &key )
{
  return edje_file_data_get (file.c_str (), key.c_str ());
}

void EdjeBase::setFileCache( int count )
{
  edje_file_cache_set (count);
}

int EdjeBase::getFileCache()
{
  return edje_file_cache_get ();
}

void EdjeBase::flushFileCache()
{
  edje_file_cache_flush ();
}

void EdjeBase::setCollectionCache( int count )
{
  edje_collection_cache_set (count);
}

int EdjeBase::getCollectionCache()
{
  return edje_file_cache_get ();
}

void EdjeBase::flushCollectionCache()
{
  edje_file_cache_flush ();
}

void EdjeBase::setColorClass( const string &colorclass, const Color& object, const Color& outline, const Color& shadow )
{
  edje_color_class_set (colorclass.c_str (),
                        object.red (), object.green (), object.blue (), object.alpha (),
                        outline.red (), outline.green (), outline.blue (), outline.alpha (),
                        shadow.red (), shadow.green (), shadow.blue (), shadow.alpha ()
                       );
}

void EdjeBase::delColorClass( const string &colorclass )
{
  edje_color_class_del (colorclass.c_str ());
}

Eina_List *EdjeBase::listColorclass( )
{
  return edje_color_class_list();
}

void EdjeBase::setTextClass( const string &textclass, const string &font, Evas_Font_Size size )
{
  edje_text_class_set (textclass.c_str (), font.c_str (), size);
}

void EdjeBase::delTextClass (const string &textclass)
{
  edje_text_class_del (textclass.c_str ());
}

Eina_List *EdjeBase::listTextClass ()
{
  return edje_text_class_list ();
}

void EdjeBase::processSignalMessage ()
{
  edje_message_signal_process ();
}

} // end namespace efl

