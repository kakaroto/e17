#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/edjexx/Part.h"
#include "../include/edjexx/Object.h"
#include "../include/edjexx/ExternalParam.h"
#include "../include/edjexx/Exceptions.h"

/* STD */
#include <map>

using namespace std;

namespace Edjexx {

Part::Part (Object* parent, const std::string &partname) :
  mParent (parent),
  mPartname (partname),
  mExternalObject (NULL),
  mExternalContent (NULL),
  mSwallowObject (NULL)
{
  //cout << "Part::EdjePart( '" << mPartname << "' ) constructing..." << endl;
}

Part::~Part()
{
  //cout << "~Part::EdjePart( '" << mPartname << "' ) destructing..." << endl;

  delete mExternalObject;
  delete mSwallowObject;
}

Eflxx::Rect Part::getGeometry() const
{
  int x;
  int y;
  int w;
  int h;
  edje_object_part_geometry_get (mParent->obj(), mPartname.c_str (), &x, &y, &w, &h);
  return Eflxx::Rect( x, y, w, h );
}


void Part::setText( const std::string &text )
{
  edje_object_part_text_set( mParent->obj(), mPartname.c_str (), text.c_str () );
}

const std::string Part::getText() const
{
  return edje_object_part_text_get( mParent->obj(), mPartname.c_str () );
}


void Part::swallow( Evasxx::Object* object )
{
  edje_object_part_swallow (mParent->obj(), mPartname.c_str (), object->obj());
}

void Part::unswallow( Evasxx::Object* object )
{
  edje_object_part_unswallow( mParent->obj(), object->obj() );
}

Evasxx::Object &Part::getSwallow()
{
  Evas_Object *eo = edje_object_part_swallow_get (mParent->obj(), mPartname.c_str ());
  if (!eo)
    throw SwallowNotExistingException (mPartname);
  
  if (!mSwallowObject)
  {
    mSwallowObject = Evasxx::Object::wrap (eo);
  }

  return *mSwallowObject;
}

Evasxx::Object &Part::getExternalObject ()
{
  Evas_Object *eo = edje_object_part_external_object_get (mParent->obj(), mPartname.c_str ());
  if (!eo)
    throw ExternalNotExistingException (mPartname);
  
  if (!mExternalObject)
  {
    mExternalObject = Evasxx::Object::wrap (eo);
  }
  
  return *mExternalObject;
}

Evasxx::Object &Part::getExternalContent (const std::string &content)
{
  Evas_Object *eo = edje_object_part_external_content_get (mParent->obj(), mPartname.c_str (), content.c_str ());
  if (!eo)
    throw ExternalNotExistingException (mPartname);
  
  if (!mExternalContent)
  {
    mExternalContent = Evasxx::Object::wrap (eo);
  }
  
  return *mExternalContent;
}

bool Part::setParam (Edjexx::ExternalParam *param)
{
  return edje_object_part_external_param_set (mParent->obj (), mPartname.c_str (), param->getRaw ());
}

/*const Evasxx::Object* Part::getObject ( const char* name )
{
  return Evasxx::Object::objectLink( edje_object_part_object_get( static_cast <const Evas_Object*> (mParent->obj()), name ) );
}*/

} // end namespace Edjexx
