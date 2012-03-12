#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Window.h"

using namespace std;

namespace Elmxx {

Window* Window::factory (const std::string &name, Elm_Win_Type type)
{
  return new Window (name, type);
}

Window* Window::factory (Evasxx::Object &parent, const std::string &name, Elm_Win_Type type)
{
  return new Window (parent, name, type);
}

Window::Window (const std::string &name, Elm_Win_Type type)
{
  o = elm_win_add (NULL, name.c_str (), type);
  
  elmInit ();
}

Window::Window (Evasxx::Object &parent, const std::string &name, Elm_Win_Type type)
{
  o = elm_win_add (parent.obj (), name.c_str (), type);
  
  elmInit ();
}

Window::~Window () {}

void Window::setTitle (const std::string &title)
{
  elm_win_title_set(o, title.c_str ());
}

void Window::setAutoDel (bool autodel)
{
  elm_win_autodel_set(o, autodel);
}

void Window::activate ()
{
  elm_win_activate(o);
}

void Window::lower ()
{
  elm_win_lower(o);
}

void Window::raise ()
{
  elm_win_raise(o);
}

void Window::setBorderless (bool borderless)
{
  elm_win_borderless_set (o, borderless);
}

void Window::setShaped (bool shaped)
{
  elm_win_shaped_set (o, shaped);
}

void Window::setAlpha (bool alpha)
{
  elm_win_alpha_set (o, alpha);
}

void Window::setOverride (bool override)
{
  elm_win_override_set (o, override);
}

void Window::setFullscreen (bool fullscreen)
{
  elm_win_fullscreen_set (o, fullscreen);
}

void Window::setMaximized (bool maximized)
{
  elm_win_maximized_set (o, maximized);
}

void Window::setIconified (bool iconified)
{
  elm_win_iconified_set(o, iconified);
}

void Window::setLayer (int layer)
{
  elm_win_layer_set(o, layer);
}

void Window::setRotation (int rotation)
{
  elm_win_rotation_set(o, rotation);
}

void Window::setSticky (bool sticky)
{
  elm_win_sticky_set(o, sticky);
}

void Window::addObjectResize (const Evasxx::Object &subobj)
{
  elm_win_resize_object_add (o, subobj.obj ());
}

void Window::delObjectResize (const Evasxx::Object &subobj)
{
  elm_win_resize_object_del (o, subobj.obj ());
}

} // end namespace Elmxx
