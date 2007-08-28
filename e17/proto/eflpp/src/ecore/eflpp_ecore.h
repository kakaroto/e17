#ifndef ECORE_BASE_H
#define ECORE_BASE_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* EFL++ */
#include <eflpp_evas.h>
#include <eflpp_ecore_window.h>

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

} // end namespace efl

#endif // ECORE_BASE_H
