#include "eflpp_ecoreconfig.h"

/* EFL++ */
#include <eflpp_sys.h>
#include <eflpp_debug_internal.h>
#include <eflpp_common.h>

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

void EcoreConfig::setGroup( const string& group )
{
    _group = group;
}

string EcoreConfig::group() const
{
    return _group;
}

void EcoreConfig::setValue( const char* key, const bool value )
{
    ecore_config_boolean_set( key, value );
}
bool EcoreConfig::getBool( const char* key, const bool defaultValue )
{
    ecore_config_boolean_default( key, defaultValue );
    return ecore_config_boolean_get( key );
}

void EcoreConfig::setValue( const char* key, const int value )
{
    ecore_config_int_set( key, value );
}
int EcoreConfig::getInt( const char* key, const int defaultValue )
{
    ecore_config_int_default( key, defaultValue );
    return ecore_config_int_get( key );
}

void EcoreConfig::setValue( const char* key, const string& value )
{
    ecore_config_string_set( key, const_cast<char*>( value.c_str() ) );
}
string EcoreConfig::getString( const char* key, const string& defaultValue )
{
    ecore_config_string_default( key, const_cast<char*>( defaultValue.c_str() ) );
    return ecore_config_string_get( key );
}

void EcoreConfig::setValue( const char* key, const float value )
{
    ecore_config_float_set( key, value );
}
float EcoreConfig::getFloat( const char* key, const float defaultValue )
{
    ecore_config_float_default( key, defaultValue );
    return ecore_config_float_get( key );
}

EcoreConfig::~EcoreConfig()
{
    ecore_config_save();
    ecore_config_shutdown();
}

}
