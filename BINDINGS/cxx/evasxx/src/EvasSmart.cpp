#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/EvasSmart.h"

#include <iostream>
#include <assert.h>

using namespace std;

namespace efl {
  
EvasSmart::EvasSmart ()
{
  mFree = false;
}
  
EvasSmart::EvasSmart(EvasCanvas &canvas)
{
  printf ("EvasSmart constructor\n");
  
  mFree = true;
 
  o =  newEsmart( canvas, "undefined" ); // FIXME: is a unique name needed?
}

EvasSmart::~EvasSmart()
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

Evas_Object *EvasSmart::newEsmart(EvasCanvas &canvas, const std::string &name )
{
  Evas_Object *evasobj;

  evasobj = evas_object_smart_add(canvas.obj(), getEsmart(name));

  evas_object_smart_data_set(evasobj, this);

  return evasobj;
}

Evas_Smart *EvasSmart::getEsmart( const std::string &name )
{
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
  sc.member_add = wrap_member_add;
  sc.member_del = wrap_member_del;
  sc.data = this;
  
  Evas_Smart *smart = evas_smart_class_new( &sc );
  
  return smart;
}

void EvasSmart::addMember (const EvasObject &obj)
{
  evas_object_smart_member_add (obj.obj (), o);
}

void EvasSmart::delMember (const EvasObject &obj)
{
  evas_object_smart_member_del (obj.obj ());
}

Eina_List *EvasSmart::getMembers ()
{
  return evas_object_smart_members_get (o);
}

void *EvasSmart::getData ()
{
  return evas_object_smart_data_get (o);
}

void EvasSmart::setData (void *data)
{
  evas_object_smart_data_set (o, data);
}

// a wrapper function for the signals...
static void wrapCustomEvent (void *data, Evas_Object *obj, void *event_info)
{
  struct CustomEventWrap *cew = static_cast <struct CustomEventWrap*> (data);
  
  EvasSmart *es = cew->es;

  sigc::signal <void, void*, Evas_Object*, void*> *ptrSig = es->getEventSignal (cew->event);
  ptrSig->emit (data, obj, event_info);
}

void EvasSmart::addEventSignal (const std::string &event)
{
  struct CustomEventWrap *cew = mCustomSignalMap[event];
  
  if (!cew)
  {
    cew = new CustomEventWrap ();
    sigc::signal <void, void*, Evas_Object*, void*> *ptrSig = new sigc::signal <void, void*, Evas_Object*, void*> ();

    cew->es = this;
    cew->customSignal = ptrSig;
    cew->event = event;

    mCustomSignalMap[event] = cew;
  
    evas_object_smart_callback_add (o, event.c_str (), wrapCustomEvent, cew);
  }
}

void EvasSmart::delEventSignal (const std::string &event)
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

sigc::signal <void, void*, Evas_Object*, void*> *EvasSmart::getEventSignal (const std::string &event)
{
  // implicit add a event signal while get
  addEventSignal (event);
  
  struct CustomEventWrap *cew = mCustomSignalMap[event];

  return cew ? cew->customSignal : NULL; // NULL should never return as one is added always
}

void EvasSmart::callEventSignal (const std::string &event, void *event_info)
{
  evas_object_smart_callback_call (o, event.c_str (), event_info);
}

void EvasSmart::hasChanged ()
{
  evas_object_smart_changed (o);
}

void EvasSmart::setNeedRecalculate (bool value)
{
  evas_object_smart_need_recalculate_set (o, value);
}

bool EvasSmart::setNeedRecalculate ()
{
  return evas_object_smart_need_recalculate_get (o);
}

void EvasSmart::calculate ()
{
  evas_object_smart_calculate (o);
}

// C wrapper helpers

void EvasSmart::wrap_add( Evas_Object *o ) 
{
  // FIXME: data not bound to object at this point. Think about new wrapper design...
  Evas_Smart *es =  evas_object_smart_smart_get (o);
    
  EvasSmart *esObj = static_cast<EvasSmart*> (evas_smart_data_get(es));
  assert (esObj);
  
  esObj->signalAdd.emit ();
}

void EvasSmart::wrap_del( Evas_Object *o) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  EvasSmart *esObj = static_cast<EvasSmart*> (evas_smart_data_get(es));
  assert (esObj);
 
  esObj->signalDel.emit ();
}

void EvasSmart::wrap_move( Evas_Object *o, Evas_Coord x, Evas_Coord y) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  EvasSmart *esObj = static_cast<EvasSmart*> (evas_smart_data_get(es));
  assert (esObj);
  
  esObj->signalMove.emit (Point (x, y));
}

void EvasSmart::wrap_resize( Evas_Object *o, Evas_Coord w, Evas_Coord h) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  EvasSmart *esObj = static_cast<EvasSmart*> (evas_smart_data_get(es));
  assert (esObj);
  
  esObj->signalResize.emit (Size (w, h));
}

void EvasSmart::wrap_show( Evas_Object *o) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  EvasSmart *esObj = static_cast<EvasSmart*> (evas_smart_data_get(es));
  assert (esObj);
  
  esObj->signalShow.emit ();
}

void EvasSmart::wrap_hide( Evas_Object *o) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  EvasSmart *esObj = static_cast<EvasSmart*> (evas_smart_data_get(es));
  assert (esObj);

  esObj->signalHide.emit ();
}

void EvasSmart::wrap_color_set( Evas_Object *o, int r, int g, int b, int a) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  EvasSmart *esObj = static_cast<EvasSmart*> (evas_smart_data_get(es));
  assert (esObj);

  esObj->signalSetColor.emit (Color (r, g, b, a));
}

void EvasSmart::wrap_clip_set( Evas_Object *o, Evas_Object *clip) 
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  EvasSmart *esObj = static_cast<EvasSmart*> (evas_smart_data_get(es));
  assert (esObj);
  
  EvasObject *clipObj = EvasObject::wrap (clip);
  esObj->signalSetClip.emit (*clipObj);
}

void EvasSmart::wrap_clip_unset( Evas_Object *o)
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  EvasSmart *esObj = static_cast<EvasSmart*> (evas_smart_data_get(es));
  assert (esObj);
  
  esObj->signalUnsetClip.emit ();
}

void EvasSmart::wrap_member_add(Evas_Object *o, Evas_Object *clip)
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  EvasSmart *esObj = static_cast<EvasSmart*> (evas_smart_data_get(es));
  assert (esObj);
  
  EvasObject *clipObj = EvasObject::wrap (clip);
  esObj->signalAddMember.emit (*clipObj);
}

void EvasSmart::wrap_member_del(Evas_Object *o, Evas_Object *clip)
{
  Evas_Smart *es = evas_object_smart_smart_get (o);
    
  EvasSmart *esObj = static_cast<EvasSmart*> (evas_smart_data_get(es));
  assert (esObj);
  
  EvasObject *clipObj = EvasObject::wrap (clip);
  esObj->signalDelMember.emit (*clipObj);
}

} // end namespace efl
