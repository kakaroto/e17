#ifndef ECOREXX_EVAS_WINDOW_H
#define ECOREXX_EVAS_WINDOW_H

/* EFLxx */
#include <eflxx/Common.h>
#include <evasxx/Evasxx.h>
#include "Application.h"

/* EFL */
#include <Ecore_Evas.h>

//===============================================================================================
// EvasWindow
//===============================================================================================

namespace Ecorexx {

/* forward declarations */

// TODO: Test for supported backends?
class EvasWindow : public Eflxx::Trackable
{
public:
  enum Event { Resize, Move, Show, Hide, DeleteRequest, Destroy,
               FocusIn, FocusOut, MouseIn, MouseOut, PreRender, PostRender
             };

  enum EngineType
  {
    SoftwareX11 = ECORE_EVAS_ENGINE_SOFTWARE_XLIB,
    SoftwareFB = ECORE_EVAS_ENGINE_SOFTWARE_FB,
    GLX11 = ECORE_EVAS_ENGINE_OPENGL_X11,
    SoftwareBuffer = ECORE_EVAS_ENGINE_SOFTWARE_BUFFER,
    XRenderX11 = ECORE_EVAS_ENGINE_XRENDER_X11,
    DirectFB = ECORE_EVAS_ENGINE_DIRECTFB,
    SoftwareX11_16 = ECORE_EVAS_ENGINE_SOFTWARE_16_X11,
    SDL = ECORE_EVAS_ENGINE_SOFTWARE_SDL
  };

public:
  virtual ~EvasWindow();

  Evasxx::Canvas &getCanvas() const;

  /* Events */
  void setEventEnabled( Event, bool );

  /*!
   * \param quit A boolean specifying whether the app should be quit upon a delete request or not.
   *             default is false
   */
  bool quitOnDelete (bool quit);

  static bool isEngineTypeSupported (EngineType et);

  /* Window Management */
  void move( const Eflxx::Point& );
  void resize( const Eflxx::Size& );
  void setGeometry( const Eflxx::Rect& );
  Eflxx::Rect geometry() const;

  void setRotation( int );
  int rotation() const;
  void setShaped( bool );
  bool isShaped() const;

  void setAlpha( bool alpha );
  bool hasAlpha () const;

  void show();
  void hide();
  void raise();
  void lower();

  void setTitle( const std::string &title );
  const std::string getTitle() const;

  // void        ecore_evas_name_class_set(Ecore_Evas *ee, const char *n, const char *c);
  // void        ecore_evas_name_class_get(Ecore_Evas *ee, const char **n, const char **c);

  void setMinimumSize( const Eflxx::Size& );
  Eflxx::Size getMinimumSize() const;
  void setMaximumSize( const Eflxx::Size& );
  Eflxx::Size getMaximumSize() const;
  void setBaseSize( const Eflxx::Size& );
  Eflxx::Size getBaseSize() const;
  void setStepSize( const Eflxx::Size& );
  Eflxx::Size getStepSize() const;

  void setCursor( const char* file, int layer, int hot_x, int hot_y );
  // void        ecore_evas_cursor_get(Ecore_Evas *ee, char **file, int *layer, int *hot_x, int *hot_y);
  
  void setLayer( int layer );
  int getLayer() const;

  void setFocus( int on );
  int hasFocus() const;

  void setIconified( int on );
  int isIconified() const;

  void setBorderless( int on );
  int isBorderless() const;

  void setOverride( int on );
  int isOverride() const;

  void setMaximized( int on );
  int isMaximized() const;

  void setFullscreen( int on );
  int isFullscreen() const;

  void setAvoidDamage( Ecore_Evas_Avoid_Damage_Type value );
  Ecore_Evas_Avoid_Damage_Type AvoidDamage() const;

  void setWithdrawn( int on );
  int isWithdrawn() const;

  void setSticky( int on );
  int isSticky() const;
  
  // event callbacks
  void resizeEvent();
  void moveEvent();
  void showEvent();
  void hideEvent();
  void deleteRequestEvent();
  void destroyEvent();
  void focusInEvent();
  void focusOutEvent();
  void mouseInEvent();
  void mouseOutEvent();
  void preRenderEvent();
  void postRenderEvent();

  sigc::signal <void, const EvasWindow&> resizeSignal;
  sigc::signal <void, const EvasWindow&> moveSignal;
  sigc::signal <void, const EvasWindow&> showSignal;
  sigc::signal <void, const EvasWindow&> hideSignal;
  sigc::signal <void, const EvasWindow&> deleteRequestSignal;
  sigc::signal <void, const EvasWindow&> destroySignal;
  sigc::signal <void, const EvasWindow&> focusInSignal;
  sigc::signal <void, const EvasWindow&> focusOutSignal;
  sigc::signal <void, const EvasWindow&> mouseInSignal;
  sigc::signal <void, const EvasWindow&> mouseOutSignal;
  sigc::signal <void, const EvasWindow&> preRenderSignal;
  sigc::signal <void, const EvasWindow&> postRenderSignal;
  
protected:
  Evasxx::Canvas *_canvas;
  Ecore_Evas *_ee;

  EvasWindow();
  static EvasWindow* objectLink( Ecore_Evas* );

private:
  bool operator=( const EvasWindow& );
  bool operator==( const EvasWindow& );
  
  bool mQuit;
};

} // end namespace Ecorexx

#endif // ECOREXX_EVAS_WINDOW_H
