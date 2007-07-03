#ifndef ECORE_CONFIG_H
#define ECORE_CONFIG_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* STD */
#include <iostream>
using namespace std;

/**
 * C++ Wrapper for the Enlightenment Core Configuration Library (Ecore_Config)
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace efl {

//===============================================================================================
// EcoreConfig
//===============================================================================================

class EcoreConfig
{
    /**
     * @class: EcoreConfig ecoreconfig.h
     * A simple persistent Key=Value configuration database.
     * @author: Michael 'Mickey' Lauer <mickey@Vanille.de>
     **/
  public:
    EcoreConfig( const string& name );
   ~EcoreConfig();

    /* group handling */
    void setGroup( const string& );
    string group() const;

    /* read / write */
    void setValue( const char*, const bool );
    bool getBool( const char*, const bool );
    void setValue( const char*, const int );
    int getInt( const char*, const int );
    void setValue( const char*, const string& );
    string getString( const char*, const string& );
    void setValue( const char*, const float );
    float getFloat( const char*, const float );

  private:
    string _group;
};

}
#endif
