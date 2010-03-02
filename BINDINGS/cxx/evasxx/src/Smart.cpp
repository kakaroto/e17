#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/Smart.h"

#include <iostream>
#include <assert.h>

using namespace std;
using namespace Eflxx;

namespace Evasxx {
  
Smart::Smart ()
{
  /* In the case that a existing C SMART is used than no C++ signals are bound!
   * Take care of this!
   */
  mFree = false;
}
  
Smart::Smart(Canvas &canvas)
{
  //printf ("Smart constructor\n");
  
  mFree = true;
 
  o =  newEsmart( canvas, "undefined" ); // FIXME: is a unique name needed?
}

Smart::~Smart()
{
  if (mFree)
  {
    evas_smart_free (evas_object_smart_smart_get (o));
  }
  
  for(CustomEventWrapMapType::iterator iter = mCustomSignalMap.begin (); iter != mCustomSignalMap.end (); ++iter)
  {
    delEventSignal (iter->first); 
  }
}

Evas_Object *Smart::newEsmart(Canvas &canvas, const std::string &name )
{
  Evas_Object *evasobj;

  evasobj = evas_object_smart_add(canvas.obj(), getEsmart(name));

  evas_object_smart_data_set(evasobj, this);

  return evasobj;
}

Evas_Smart *Smart::getEsmart( const std::string &name )
{
  cout << "getEsmart" << endl;
  sc.name = name.c_str (); // FIXME: is a unique name needed?
  sc.version =  EVAS_SMART_CLASS_VERSION;
  sc.add = wrap_add;
  sc.del = wrap_del;
  sc.move = wrap_move;
  sc.resize = wrap_resize;
  sc.show = wrap_show;
  sc.hide = wrap_hide;
  sc.color_set = wrap_color_set;
  sc.clip_set = wrap_clip_set;
  sc.clip_unset = wrap_clip_unset;
  sc.calculate = NULL; // TODO
  sc.member_add = wrap_member_add;
  sc.member_del = wrap_member_del;
  sc.data = this;
  
  Evas_Smart *smart = evas_smart_class_new( &sc );
  
  return smart;
}

void Smart::addMember (const Object &obj)
{
  evas_object_smart_member_add (obj.obj (), o);
}

void Smart::delMember (const Object &obj)
{
  evas_object_smart_member_del (obj.obj ());
}

Eina_List *Smart::getMembers ()
{
  return evas_object_smart_members_get (o);
}

void *Smart::getData ()
{
  return evas_object_smart_data_get (o);
}

void Smart::setData (void *data)
{
  evas_object_smart_data_set (o, data);
}

void Smart::addEventSignal (const std::string &event)
{
  struct CustomEventWrap *cew = mCustomSignalMap[event];
  
  if (!cew)
  {
    cew = new CustomEventWrap ();
    sigc::signal <void, Evas_Object*, void*> *ptrSig = new sigc::signal <void, Evas_Object*, void*> ();

    cew->es = this;
    cew->customSignal = ptrSig;
    cew->event = event;

    mCustomSignalMap[event] = cew;
  
    evas_object_smart_callback_add (o, event.c_str (), wrapCustomEvent, cew);
  }
}

void Smart::delEventSignal (const std::string &event)
{
  /* ignore return */ evas_object_smart_callback_del (o, event.c_str (), wrapCustomEvent);
  
  struct CustomEventWrap *cew = mCustomSignalMap[event];
  if (cew)
  {
    delete cew->customSignal;
  }
  delete cew;
  
  mCustomSignalMap.erase (event);
}

sigc::signal <void, Evas_Object*, void*> *Smart::getEventSignal (const std::string &event)
{
  // implicit add a event signal while get
  addEventSignal (event);
  
  struct CustomEventWrap *cew = mCustomSignalMap[event];

  return cew ? cew->customSignal : NULL; // NULL should never return as one is added always
}

void Smart::callEventSignal (const std::string &event, void *event_info)
{
  evas_object_smart_callback_call (o, event.c_str (), event_info);
}

void Smart::hasChanged ()
{
  evas_object_smart_changed (o);
}

void Smart::setNeedRecalculate (bool value)
{
  evas_object_smart_need_recalculate_set (o, value);
}

bool Smart::setNeedRecalculate ()
{
  return evas_object_smart_need_recalculate_get (o);
}

void Smart::calculate ()
{
  evas_object_smart_calculate (o);
}

// C wrapper helpers

// a wrapper function for the signals...
void Smart::wrapCustomEvent (void *data, Evas_Object *obj, void *event_info)
{
  struct CustomEventWrap *cew = static_cast <struct CustomEventWrap*> (data);
  
  Smart *es = cew->es;

  sigc::signal <void, Evas_Object*, void*> *ptrSig = es->getEventSignal (cew->event);
  ptrSig->emit (obj, event_info);
}

void Smart::wrap_add( Evas_Object *o ) 
{
  // FIXME: data not bound to object at this point. Think about new wrapper design...
  Evas_Smart *es =  evas_object_smart_smart_get (o);
    
  Smart *esObj = static_cast<Smart*> (evas_smart_data_get(es));
  assert (esObj);
  
  esObj->signalAdd.emit ();
}

void Smart::wrap_del( Evas_Object *o) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  Smart *esObj = static_cast<Smart*> (evas_smart_data_get(es));
  assert (esObj);
 
  esObj->signalDel.emit ();
}

void Smart::wrap_move( Evas_Object *o, Evas_Coord x, Evas_Coord y) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  Smart *esObj = static_cast<Smart*> (evas_smart_data_get(es));
  assert (esObj);
  
  esObj->signalMove.emit (Point (x, y));
}

void Smart::wrap_resize( Evas_Object *o, Evas_Coord w, Evas_Coord h) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  Smart *esObj = static_cast<Smart*> (evas_smart_data_get(es));
  assert (esObj);
  
  esObj->signalResize.emit (Size (w, h));
}

void Smart::wrap_show( Evas_Object *o) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  Smart *esObj = static_cast<Smart*> (evas_smart_data_get(es));
  assert (esObj);
  
  esObj->signalShow.emit ();
}

void Smart::wrap_hide( Evas_Object *o) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  Smart *esObj = static_cast<Smart*> (evas_smart_data_get(es));
  assert (esObj);

  esObj->signalHide.emit ();
}

void Smart::wrap_color_set( Evas_Object *o, int r, int g, int b, int a) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  Smart *esObj = static_cast<Smart*> (evas_smart_data_get(es));
  assert (esObj);

  esObj->signalSetColor.emit (Color (r, g, b, a));
}

void Smart::wrap_clip_set( Evas_Object *o, Evas_Object *clip) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  Smart *esObj = static_cast<Smart*> (evas_smart_data_get(es));
  assert (esObj);
  
  Object *clipObj = Object::wrap (clip);
  esObj->signalSetClip.emit (*clipObj);
}

void Smart::wrap_clip_unset( Evas_Object *o)
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  Smart *esObj = static_cast<Smart*> (evas_smart_data_get(es));
  assert (esObj);
  
  esObj->signalUnsetClip.emit ();
}

void Smart::wrap_member_add(Evas_Object *o, Evas_Object *clip)
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  Smart *esObj = static_cast<Smart*> (evas_smart_data_get(es));
  assert (esObj);
  
  Object *clipObj = Object::wrap (clip);
  esObj->signalAddMember.emit (*clipObj);
}

void Smart::wrap_member_del(Evas_Object *o, Evas_Object *clip)
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  Smart *esObj = static_cast<Smart*> (evas_smart_data_get(es));
  assert (esObj);
  
  Object *clipObj = Object::wrap (clip);
  esObj->signalDelMember.emit (*clipObj);
}

} // end namespace Evasxx

