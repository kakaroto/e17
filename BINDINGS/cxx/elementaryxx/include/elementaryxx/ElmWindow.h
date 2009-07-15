#ifndef ELM_WINDOW_H
#define ELM_WINDOW_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmWidget.h"

namespace efl {
  
/*!
 * smart callbacks called:
 * "delete-request" - the user requested to delete the window
 * "focus-in" - window got focus
 * "focus-out" - window lost focus
 */
class ElmWindow : public ElmWidget
{
public:
  virtual ~ElmWindow ();
  
  static ElmWindow* factory (const std::string &name, Elm_Win_Type type);
  
  /*!
   * Contructor for a EwlWindow that has a parent window.
   * The function ecore_x_icccm_transient_for_set is used to specify that a window 
   * is transient for another top-level window and should be handled accordingly.
   */
  static ElmWindow* factory (EvasObject &parent, const std::string &name, Elm_Win_Type type);
  
  void setTitle (const std::string &title);
  
  void setAutoDel (bool autodel);
  
  void activate ();
  
  void lower ();
  
  void raise ();
  
  void setBorderless (bool borderless);
  
  void setShaped (bool shaped);
  
  void setAlpha (bool alpha);
  
  void setOverride (bool override);
  
  void setFullscreen (bool fullscreen);
  
  void setMaximized (bool maximized);
  
  void setIconified (bool iconified);
  
  void setLayer (int layer);
  
  void setRotation (int rotation);
  
  void setSticky (bool sticky);
  
  //void setKeyboardMode (Elm_Win_Keyboard_Mode mode);
  
  //void setKeyboardWin (bool isKeyboard); // FIXME: is* name?
  
  void addObjectResize (const EvasObject &subobj);
  
  void delObjectResize (const EvasObject &subobj);
  
private:
  ElmWindow (); // forbid standard constructor
  ElmWindow (const ElmWindow&); // forbid copy constructor
  
  // private construction -> use factory ()
  ElmWindow (const std::string &name, Elm_Win_Type type);
  ElmWindow (EvasObject &parent, const std::string &name, Elm_Win_Type type);
};

} // end namespace efl

#endif // ELM_WINDOW_H

#if 0
   // TODO

 
   ...
 
 
   EAPI void         elm_win_keyboard_mode_set(Evas_Object *obj, Elm_Win_Keyboard_Mode mode);
   EAPI void         elm_win_keyboard_win_set(Evas_Object *obj, Evas_Bool is_keyboard);

   EAPI Evas_Object *elm_win_inwin_add(Evas_Object *obj);
   EAPI void         elm_win_inwin_activate(Evas_Object *obj);
   EAPI void         elm_win_inwin_style_set(Evas_Object *obj, const char *style);
   EAPI void         elm_win_inwin_content_set(Evas_Object *obj, Evas_Object *content);
 
    /* available styles: 
    * default
    * minimal
    * minimal_vertical
    */

   EAPI Ecore_X_Window elm_win_xwindow_get(const Evas_Object *obj);  
#endif
