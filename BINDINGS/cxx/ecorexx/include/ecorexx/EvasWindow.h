#ifndef ECORE_EVAS_WINDOW_H
#define ECORE_EVAS_WINDOW_H

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
class XWindow;

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
  virtual void resizeEvent();
  virtual void moveEvent();
  virtual void showEvent();
  virtual void hideEvent();
  virtual void deleteRequestEvent();
  virtual void destroyEvent();
  virtual void focusInEvent();
  virtual void focusOutEvent();
  virtual void mouseInEvent();
  virtual void mouseOutEvent();
  virtual void preRenderEvent();
  virtual void postRenderEvent();

  /*!
   * \return A boolean specifying whether the window should be closed upon a delete request or not.
   *         The default implementation returns true.
   */
  virtual bool canClose() const;

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

protected:
  Evasxx::Canvas *_canvas;
  Ecore_Evas *_ee;

  EvasWindow();
  static EvasWindow* objectLink( Ecore_Evas* );

private:
  bool operator=( const EvasWindow& );
  bool operator==( const EvasWindow& );
};

class EvasWindowSoftwareX11 : public EvasWindow
{
public:
  EvasWindowSoftwareX11( const Eflxx::Size &size, const char* display = 0 );
  virtual ~EvasWindowSoftwareX11();

  /*!
   * \brief Gets a pointer to the X window. The returned pointer needs to be
   *        deleted if you don't need it any longer.
   *
   * \return The XWindow.
   */
  XWindow *getXWindow();

private:
  EvasWindowSoftwareX11();
  bool operator=( const EvasWindowSoftwareX11& );
  bool operator==( const EvasWindowSoftwareX11& );
};

class EvasWindowGLX11 : public EvasWindow
{
public:
  EvasWindowGLX11( const Eflxx::Size &size, const char* display = 0 );
  virtual ~EvasWindowGLX11();

  /*!
   * \brief Gets a pointer to the X window. The returned pointer needs to be
   *        deleted if you don't need it any longer.
   *
   * \return The XWindow.
   */
  XWindow *getXWindow();

private:
  EvasWindowGLX11();
  bool operator=( const EvasWindowGLX11& );
  bool operator==( const EvasWindowGLX11& );
};

class EvasWindowXRenderX11 : public EvasWindow
{
public:
  EvasWindowXRenderX11( const Eflxx::Size &size, const char* display = 0 );
  virtual ~EvasWindowXRenderX11();

  /*!
   * \brief Gets a pointer to the X window. The returned pointer needs to be
   *        deleted if you don't need it any longer.
   *
   * \return The XWindow.
   */
  XWindow *getXWindow();

private:
  EvasWindowXRenderX11();
  bool operator=( const EvasWindowXRenderX11& );
  bool operator==( const EvasWindowXRenderX11& );
};

class EvasWindowFB : public EvasWindow
{
public:
  EvasWindowFB( const Eflxx::Size &size, const char* display = 0, int rotation = 0 );
  virtual ~EvasWindowFB();

  XWindow *getXWindow();

private:
  EvasWindowFB();
  bool operator=( const EvasWindowFB& );
  bool operator==( const EvasWindowFB& );
};

} // end namespace Ecorexx

#endif // ECORE_EVAS_WINDOW_H
