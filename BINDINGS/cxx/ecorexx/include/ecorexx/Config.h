#ifndef ECORE_CONFIG_H
#define ECORE_CONFIG_H

/* STD */
#include <iostream>

/**
 * C++ Wrapper for the Enlightenment Core Configuration Library (Ecore_Config)
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace Ecorexx {

//===============================================================================================
// Config
//===============================================================================================

class Config
{
  /**
   * @class: Config ecoreconfig.h
   * A simple persistent Key=Value configuration database.
   * @author: Michael 'Mickey' Lauer <mickey@Vanille.de>
   **/
public:
  Config( const std::string& name );
  ~Config();

  /* group handling */
  void setGroup( const std::string& );
  std::string getGroup() const;

  /* read / write */
  void setValue( const std::string &key, const bool value);
  bool getBool( const std::string &key, const bool value);
  void setValue( const std::string &key, const int value);
  int getInt( const std::string &key, const int value);
  void setValue( const std::string &key, const std::string& value);
  std::string getString( const std::string &key, const std::string& value);
  void setValue( const std::string &key, const float value);
  float getFloat( const std::string &key, const float value);

private:
  std::string _group;
};

}
#endif
