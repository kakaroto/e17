#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* EFL++ */
#include <eflxx/eflpp_sys.h>
#include <eflxx/eflpp_debug_internal.h>
#include <eflxx/eflpp_common.h>
#include "../include/ecorexx/EcoreConfig.h"

/* EFL */
#include <Ecore_Config.h>

#include <iostream>
using namespace std;

namespace efl {

//===============================================================================================
// EcoreConfig
//===============================================================================================

EcoreConfig::EcoreConfig( const string& name )
    :_group( "default/" )
{
  ecore_config_init( const_cast<char*>( name.c_str() ) );
  ecore_config_load();
}

void EcoreConfig::setGroup( const string &group )
{
  _group = group;
}

string EcoreConfig::getGroup() const
{
  return _group;
}

void EcoreConfig::setValue( const string &key, const bool value )
{
  ecore_config_boolean_set( key.c_str (), value );
}
bool EcoreConfig::getBool( const string &key, const bool defaultValue )
{
  ecore_config_boolean_default( key.c_str (), defaultValue );
  return ecore_config_boolean_get( key.c_str () );
}

void EcoreConfig::setValue( const string &key, const int value )
{
  ecore_config_int_set( key.c_str (), value );
}
int EcoreConfig::getInt( const string &key, const int defaultValue )
{
  ecore_config_int_default( key.c_str (), defaultValue );
  return ecore_config_int_get( key.c_str () );
}

void EcoreConfig::setValue( const string &key, const string& value )
{
  ecore_config_string_set( key.c_str (), const_cast<char*>( value.c_str () ) );
}
string EcoreConfig::getString( const string &key, const string& defaultValue )
{
  ecore_config_string_default( key.c_str (), const_cast<char*>( defaultValue.c_str() ) );
  return ecore_config_string_get( key.c_str () );
}

void EcoreConfig::setValue( const string &key, const float value )
{
  ecore_config_float_set( key.c_str (), value );
}
float EcoreConfig::getFloat( const string &key, const float defaultValue )
{
  ecore_config_float_default( key.c_str (), defaultValue );
  return ecore_config_float_get( key.c_str () );
}

EcoreConfig::~EcoreConfig()
{
  ecore_config_save();
  ecore_config_shutdown();
}

}
