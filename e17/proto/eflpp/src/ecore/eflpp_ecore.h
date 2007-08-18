#ifndef ECORE_BASE_H
#define ECORE_BASE_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* EFL++ */
#include <eflpp_evas.h>

/* EFL */
#include <Evas.h>
#include <Ecore_Config.h>
#include <Ecore_Data.h>
#include <Ecore_Ipc.h>
#include <Ecore_Txt.h>
#include <Ecore_Con.h>
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Ecore_Job.h>

/* STD */
#include <string>
using std::string;
#include <list>
using std::list;

#define eApp EcoreApplication::application()

/**
 * C++ Wrapper for the Enlightenment Core Library (Ecore)
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace efl {

/* Forward declarations */
class EcoreEvasWindow;
class EcoreApplication;
class EcoreCallback;
class EcoreConfig;
class EcoreAnimator;
class EcoreTimer;

class EcoreCallback
{
  public:
    EcoreCallback( EcoreApplication* object ) : o(object) {};
    ~EcoreCallback() {};
  private:
    EcoreApplication* o;
};

//===============================================================================================
// EcoreApplication
//===============================================================================================

class EcoreApplication : public Trackable
{
  public:
    EcoreApplication( int argc, const char** argv, const string& name );
    virtual ~EcoreApplication();

    static EcoreApplication* application();
    string name() const { return _name; };

    /* Main Window */
    void setMainWindow( EcoreEvasWindow* );
    EcoreEvasWindow* mainWindow() const;

    /* Main Loop */
    void exec();
    void processOneEvent();
    void quit();

    /* Timer */
    void startTimer( double seconds );
    virtual void timerEvent( EcoreTimer* ) {};

    /* Font */
    EvasFont font();
    EvasFont fixedFont();
    void setFont( const EvasFont& );
    void setFixedFont( const EvasFont& );

    /* Configuration */
    EcoreConfig* config();

  private:
    const string _binary;
    string _name;
    static EcoreApplication* _instance;
    EcoreEvasWindow* _mainWindow;
    EvasFont* _font;
    EvasFont* _fixedFont;
    EcoreConfig* _config;

  private:
    EcoreApplication();
    bool operator=( const EcoreApplication& );
    bool operator==( const EcoreApplication& );
};

//===============================================================================================
// EcoreEvasWindow
//===============================================================================================

class EcoreEvasWindow : public Trackable
{
  public:
    enum Event { Resize, Move, Show, Hide, DeleteRequest, Destroy,
                    FocusIn, FocusOut, MouseIn, MouseOut, PreRender, PostRender };

  public:
    EcoreEvasWindow( int width, int height, const char* display = 0, int rotation = 0 );
    virtual ~EcoreEvasWindow();

    EvasCanvas* canvas() const { return _canvas; };

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

   /**
    * @returns a boolean specifying whether the window should be closed upon a delete request or not.
    * The default implementation returns true.
    **/
    virtual bool canClose() const;

    /* Window Management */
    void move( const Point& );
    void resize( const Size& );
    void setGeometry( const Rect& );
    Rect geometry() const;

    void setRotation( int );
    int rotation() const;
    void setShaped( bool );
    bool isShaped() const;

    void show();
    void hide();
    void raise();
    void lower();

    void setTitle( const char* );
    const char* title() const;

    // void        ecore_evas_name_class_set(Ecore_Evas *ee, const char *n, const char *c);
    // void        ecore_evas_name_class_get(Ecore_Evas *ee, const char **n, const char **c);

    void setMinimumSize( const Size& );
    Size minimumSize() const;
    void setMaximumSize( const Size& );
    Size maximumSize() const;
    void setBaseSize( const Size& );
    Size baseSize() const;
    void setStepSize( const Size& );
    Size stepSize() const;

    void setCursor( const char* file, int layer, int hot_x, int hot_y );
    // void        ecore_evas_cursor_get(Ecore_Evas *ee, char **file, int *layer, int *hot_x, int *hot_y);

    void setLayer( int layer );
    int layer() const;

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

    void setAvoidDamage( int on );
    int isAvoidDamage() const;

    void setWithdrawn( int on );
    int isWithdrawn() const;

    void setSticky( int on );
    int isSticky() const;

  protected:
    EvasCanvas* _canvas;
    Ecore_Evas* _ee;

    static EcoreEvasWindow* objectLink( Ecore_Evas* );

  private:
    EcoreEvasWindow();
    bool operator=( const EcoreEvasWindow& );
    bool operator==( const EcoreEvasWindow& );
};

class EcoreEvasWindowSoftwareX11 : public EcoreEvasWindow
{
  public:
    EcoreEvasWindowSoftwareX11( int width, int height, const char* display = 0 );
    virtual ~EcoreEvasWindowSoftwareX11();

    // TODO: Test for supported backends?
  
  private:
    EcoreEvasWindowSoftwareX11();
    bool operator=( const EcoreEvasWindowSoftwareX11& );
    bool operator==( const EcoreEvasWindowSoftwareX11& );
};

class EcoreEvasWindowFB : public EcoreEvasWindow
{
  public:
    EcoreEvasWindowFB( int width, int height, const char* display, int rotation );
    virtual ~EcoreEvasWindowFB();

    // TODO: Test for supported backends?
  
  private:
    EcoreEvasWindowFB();
    bool operator=( const EcoreEvasWindowFB& );
    bool operator==( const EcoreEvasWindowFB& );
};

//===============================================================================================
// EcoreAnimator
//===============================================================================================

class EcoreAnimator
{
  public:
    EcoreAnimator();
    virtual ~EcoreAnimator();

    virtual bool tick();
    static void setFrameTime( double frametime );
    static double frameTime();

  private:
    Ecore_Animator* _ea;

    static int __dispatcher( void* data );
};

//===============================================================================================
// EcoreTimer
//===============================================================================================

class EcoreTimer
{
    typedef sigc::signal<void,EcoreTimer*> Signal;
    typedef sigc::slot1<void,EcoreTimer*> Slot;

  public:
    EcoreTimer( double seconds, bool singleshot = false );
    virtual ~EcoreTimer();

    virtual void tick();
    void setInterval( double );

    static EcoreTimer* singleShot( double seconds, const EcoreTimer::Slot& );

  public: /* signals */
    EcoreTimer::Signal timeout;

  private:
    Ecore_Timer* _et;
    bool _ss;

    static int __dispatcher( void* data );
};

}
#endif
