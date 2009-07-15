#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "EvasSmartGroup.h"

#include <iostream>
#include <assert.h>

using namespace std;

namespace efl {

EvasSmartGroup::EvasSmartGroup( EvasCanvas &canvas ) :
  EvasSmart (canvas)
{
  init();
  connectSignals ();  
}

EvasSmartGroup::EvasSmartGroup( EvasCanvas &canvas, const Point &pos ) :
  EvasSmart (canvas)
{
  init();
  connectSignals ();
  
  move( pos );
}

EvasSmartGroup::EvasSmartGroup( EvasCanvas &canvas, const Rect &rect ) :
  EvasSmart (canvas)
{
  init();
  connectSignals ();
  
  setGeometry (rect);
}

EvasSmartGroup::~EvasSmartGroup()
{
  clear ();
  evas_object_del( o );
}

void EvasSmartGroup::connectSignals ()
{
  signalAdd.connect (sigc::mem_fun (this, &EvasSmartGroup::addHandler));
  signalDel.connect (sigc::mem_fun (this, &EvasSmartGroup::delHandler));
  signalMove.connect (sigc::mem_fun (this, &EvasSmartGroup::moveHandler));
  signalResize.connect (sigc::mem_fun (this, &EvasSmartGroup::resizeHandler));
  
  signalShow.connect (sigc::mem_fun (this, &EvasSmartGroup::showHandler));
  signalHide.connect (sigc::mem_fun (this, &EvasSmartGroup::hideHandler));
  signalSetColor.connect (sigc::mem_fun (this, &EvasSmartGroup::colorSetHandler));
  //signalSetClip.connect (sigc::mem_fun (this, &EvasSmartGroup::clipSetHandler));
  signalUnsetClip.connect (sigc::mem_fun (this, &EvasSmartGroup::clipUnsetHandler));
  //signalAddMember.connect (sigc::mem_fun (this, &EvasSmartGroup::resizeHandler));
  //signalDelMember.connect (sigc::mem_fun (this, &EvasSmartGroup::resizeHandler));
}

void EvasSmartGroup::append (EvasObject *object)
{
  mEvasObjectList.push_back (object);
}

void EvasSmartGroup::prepend (EvasObject *object)
{
  mEvasObjectList.push_front (object);
}

void EvasSmartGroup::remove (EvasObject* object)
{
  mEvasObjectList.remove (object);
  cerr << "EvasSmartGroup::remove" << endl;
}

void EvasSmartGroup::clear()
{
  // FIXME: delete is bad, find another way (e.g. like Gtkmm::managed()
  /*for (list<EvasObject*>::iterator eo_it = mEvasObjectList.begin ();
       eo_it != mEvasObjectList.end ();
       ++eo_it)
  {
    delete *eo_it;
  }*/

  mEvasObjectList.clear ();
}

// Handler functions

void EvasSmartGroup::addHandler()
{
  cerr << "EvasSmartGroup::addHandler" << endl;
}

void EvasSmartGroup::delHandler()
{
  cerr << "EvasSmartGroup::delHandler" << endl;
}

void EvasSmartGroup::moveHandler(const Point &pos)
{
  cerr << "EvasSmartGroup::moveHandler" << endl;
}

void EvasSmartGroup::resizeHandler (const Size &size)
{
  cerr << "EvasSmartGroup::rmEvasSmartizeHandler" << endl;
}

void EvasSmartGroup::showHandler()
{
  printf ("EvasSmartGroup::showHandler (%p) -> size: %d\n", this, mEvasObjectList.size());
  for (list<EvasObject*>::iterator eol_it = mEvasObjectList.begin ();
       eol_it != mEvasObjectList.end ();
       eol_it++)
  {
    EvasObject *eo = (*eol_it);
    eo->show();
  }
}

void EvasSmartGroup::hideHandler()
{
  printf ("EvasSmartGroup::hideHandler (%p) -> size: %d\n", this, mEvasObjectList.size());
  for (list<EvasObject*>::iterator eol_it = mEvasObjectList.begin ();
       eol_it != mEvasObjectList.end ();
       eol_it++)
  {
    EvasObject *eo = (*eol_it);
    eo->hide();
  }
}

void EvasSmartGroup::colorSetHandler (const Color &color)
{
  cerr << "EvasSmartGroup::colorSetHandler" << endl;
}

void EvasSmartGroup::clipSetHandler( Evas_Object *clip )
{
  cerr << "EvasSmartGroup::clipSetHandler" << endl;
}

void EvasSmartGroup::clipUnsetHandler()
{
  cerr << "EvasSmartGroup::clipUnsetHandler" << endl;
}

} // end nammEvasSmartpace efl
