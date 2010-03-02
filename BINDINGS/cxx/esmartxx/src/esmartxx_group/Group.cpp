#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Group.h"

#include <iostream>
#include <assert.h>

using namespace std;

namespace Esmartxx {

Group::Group( Evasxx::Canvas &canvas ) :
  Evasxx::Smart (canvas)
{
  init();
  connectSignals ();  
}

Group::Group( Evasxx::Canvas &canvas, const Eflxx::Point &pos ) :
  Evasxx::Smart (canvas)
{
  init();
  connectSignals ();
  
  move( pos );
}

Group::Group( Evasxx::Canvas &canvas, const Eflxx::Rect &rect ) :
  Evasxx::Smart (canvas)
{
  init();
  connectSignals ();
  
  setGeometry (rect);
}

Group::~Group()
{
  clear ();
  evas_object_del( o );
}

void Group::connectSignals ()
{
  signalAdd.connect (sigc::mem_fun (this, &Group::addHandler));
  signalDel.connect (sigc::mem_fun (this, &Group::delHandler));
  signalMove.connect (sigc::mem_fun (this, &Group::moveHandler));
  signalResize.connect (sigc::mem_fun (this, &Group::resizeHandler));
  
  signalShow.connect (sigc::mem_fun (this, &Group::showHandler));
  signalHide.connect (sigc::mem_fun (this, &Group::hideHandler));
  signalSetColor.connect (sigc::mem_fun (this, &Group::colorSetHandler));
  //signalSetClip.connect (sigc::mem_fun (this, &Group::clipSetHandler));
  signalUnsetClip.connect (sigc::mem_fun (this, &Group::clipUnsetHandler));
  //signalAddMember.connect (sigc::mem_fun (this, &Group::resizeHandler));
  //signalDelMember.connect (sigc::mem_fun (this, &Group::resizeHandler));
}

void Group::append (Evasxx::Object *object)
{
  mEvasObjectList.push_back (object);
}

void Group::prepend (Evasxx::Object *object)
{
  mEvasObjectList.push_front (object);
}

void Group::remove (Evasxx::Object* object)
{
  mEvasObjectList.remove (object);
  cerr << "Group::remove" << endl;
}

void Group::clear()
{
  // FIXME: delete is bad, find another way (e.g. like Gtkmm::managed()
  /*for (list<Evasxx::Object*>::iterator eo_it = mEvasObjectList.begin ();
       eo_it != mEvasObjectList.end ();
       ++eo_it)
  {
    delete *eo_it;
  }*/

  mEvasObjectList.clear ();
}

// Handler functions

void Group::addHandler()
{
  cerr << "Group::addHandler" << endl;
}

void Group::delHandler()
{
  cerr << "Group::delHandler" << endl;
}

void Group::moveHandler(const Eflxx::Point &pos)
{
  cerr << "Group::moveHandler" << endl;
}

void Group::resizeHandler (const Eflxx::Size &size)
{
  cerr << "Group::rmEvasxx::SmartizeHandler" << endl;
}

void Group::showHandler()
{
  //printf ("Group::showHandler (%p) -> size: %d\n", this, mEvasObjectList.size());
  for (list<Evasxx::Object*>::iterator eol_it = mEvasObjectList.begin ();
       eol_it != mEvasObjectList.end ();
       eol_it++)
  {
    Evasxx::Object *eo = (*eol_it);
    eo->show();
  }
}

void Group::hideHandler()
{
  //printf ("Group::hideHandler (%p) -> size: %d\n", this, mEvasObjectList.size());
  for (list<Evasxx::Object*>::iterator eol_it = mEvasObjectList.begin ();
       eol_it != mEvasObjectList.end ();
       eol_it++)
  {
    Evasxx::Object *eo = (*eol_it);
    eo->hide();
  }
}

void Group::colorSetHandler (const Eflxx::Color &color)
{
  cerr << "Group::colorSetHandler" << endl;
}

void Group::clipSetHandler( Evas_Object *clip )
{
  cerr << "Group::clipSetHandler" << endl;
}

void Group::clipUnsetHandler()
{
  cerr << "Group::clipUnsetHandler" << endl;
}

} // end nammEvasxx::Smartpace efl
