#ifndef ECORE_X_WINDOW_H
#define ECORE_X_WINDOW_H

#include <eflpp_ecore.h>
#include <Ecore_X.h>

//===============================================================================================
// EcoreXWindow
//===============================================================================================

/*!
 * This is an initial wrapper for Ecore_X.
 */

namespace efl {

class EcoreXWindow //: public Trackable
{
  public:
    enum EcoreXWindowType
    {
      Desktop = ECORE_X_WINDOW_TYPE_DESKTOP,
      Dock = ECORE_X_WINDOW_TYPE_DOCK,
      Toolbar = ECORE_X_WINDOW_TYPE_TOOLBAR,
      Menu = ECORE_X_WINDOW_TYPE_MENU,
      Utility = ECORE_X_WINDOW_TYPE_UTILITY,
      Spash = ECORE_X_WINDOW_TYPE_SPLASH,
      Dialog = ECORE_X_WINDOW_TYPE_DIALOG,
      Normal = ECORE_X_WINDOW_TYPE_NORMAL,
      Unknown = ECORE_X_WINDOW_TYPE_UNKNOWN
    };
  
  public:
    //EcoreXWindow () {};
    EcoreXWindow( Ecore_X_Window exw );
    virtual ~EcoreXWindow();

    void setNetWMWindowType( EcoreXWindowType type );
    //void getNetWMWindowTypePrefetch();
    //void getNetWMWindowTypeFetc ();
    bool getNetWMWindowType( EcoreXWindowType &outType );
    
    // TODO: not sure where to put DPMS and all other functions
    static bool getDPMSEnabled ();
    static void setDPMSEnabled (bool enabled);
    
  protected:

  private:
    bool operator=( const EcoreXWindow& );
    bool operator==( const EcoreXWindow& );
    
    Ecore_X_Window _exwin;
};


} // end namespace efl

#endif // ECORE_X_WINDOW_H
