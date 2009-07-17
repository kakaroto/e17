#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmWindow.h"

using namespace std;

namespace efl {

ElmWindow* ElmWindow::factory (const std::string &name, Elm_Win_Type type)
{
  return new ElmWindow (name, type);
}

ElmWindow* ElmWindow::factory (EvasObject &parent, const std::string &name, Elm_Win_Type type)
{
  return new ElmWindow (parent, name, type);
}

ElmWindow::ElmWindow (const std::string &name, Elm_Win_Type type)
{
  o = elm_win_add (NULL, name.c_str (), type);
  
  elmInit ();
}

ElmWindow::ElmWindow (EvasObject &parent, const std::string &name, Elm_Win_Type type)
{
  o = elm_win_add (parent.obj (), name.c_str (), type);
  
  elmInit ();
}

ElmWindow::~ElmWindow () {}

void ElmWindow::setTitle (const std::string &title)
{
  elm_win_title_set(o, title.c_str ());
}

void ElmWindow::setAutoDel (bool autodel)
{
  elm_win_autodel_set(o, autodel);
}

void ElmWindow::activate ()
{
  elm_win_activate(o);
}

void ElmWindow::lower ()
{
  elm_win_lower(o);
}

void ElmWindow::raise ()
{
  elm_win_raise(o);
}

void ElmWindow::setBorderless (bool borderless)
{
  elm_win_borderless_set (o, borderless);
}

void ElmWindow::setShaped (bool shaped)
{
  elm_win_shaped_set (o, shaped);
}

void ElmWindow::setAlpha (bool alpha)
{
  elm_win_alpha_set (o, alpha);
}

void ElmWindow::setOverride (bool override)
{
  elm_win_override_set (o, override);
}

void ElmWindow::setFullscreen (bool fullscreen)
{
  elm_win_fullscreen_set (o, fullscreen);
}

void ElmWindow::setMaximized (bool maximized)
{
  elm_win_maximized_set (o, maximized);
}

void ElmWindow::setIconified (bool iconified)
{
  elm_win_iconified_set(o, iconified);
}

void ElmWindow::setLayer (int layer)
{
  elm_win_layer_set(o, layer);
}

void ElmWindow::setRotation (int rotation)
{
  elm_win_rotation_set(o, rotation);
}

void ElmWindow::setSticky (bool sticky)
{
  elm_win_sticky_set(o, sticky);
}

void ElmWindow::addObjectResize (const EvasObject &subobj)
{
  elm_win_resize_object_add (o, subobj.obj ());
}

void ElmWindow::delObjectResize (const EvasObject &subobj)
{
  elm_win_resize_object_del (o, subobj.obj ());
}

} // end namespace efl
