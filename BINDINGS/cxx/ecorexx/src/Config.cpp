#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* EFL++ */
#include <eflxx/System.h>
#include <eflxx/DebugInternal.h>
#include <eflxx/Common.h>
#include "../include/ecorexx/Config.h"

/* EFL */
#include <Ecore_Config.h>

#include <iostream>
using namespace std;

namespace Ecorexx {

//===============================================================================================
// Config
//===============================================================================================

Config::Config( const string& name )
    :_group( "default/" )
{
  ecore_config_init( const_cast<char*>( name.c_str() ) );
  ecore_config_load();
}

void Config::setGroup( const string &group )
{
  _group = group;
}

string Config::getGroup() const
{
  return _group;
}

void Config::setValue( const string &key, const bool value )
{
  ecore_config_boolean_set( key.c_str (), value );
}
bool Config::getBool( const string &key, const bool defaultValue )
{
  ecore_config_boolean_default( key.c_str (), defaultValue );
  return ecore_config_boolean_get( key.c_str () );
}

void Config::setValue( const string &key, const int value )
{
  ecore_config_int_set( key.c_str (), value );
}
int Config::getInt( const string &key, const int defaultValue )
{
  ecore_config_int_default( key.c_str (), defaultValue );
  return ecore_config_int_get( key.c_str () );
}

void Config::setValue( const string &key, const string& value )
{
  ecore_config_string_set( key.c_str (), const_cast<char*>( value.c_str () ) );
}
string Config::getString( const string &key, const string& defaultValue )
{
  ecore_config_string_default( key.c_str (), const_cast<char*>( defaultValue.c_str() ) );
  return ecore_config_string_get( key.c_str () );
}

void Config::setValue( const string &key, const float value )
{
  ecore_config_float_set( key.c_str (), value );
}
float Config::getFloat( const string &key, const float defaultValue )
{
  ecore_config_float_default( key.c_str (), defaultValue );
  return ecore_config_float_get( key.c_str () );
}

Config::~Config()
{
  ecore_config_save();
  ecore_config_shutdown();
}

}
