#include "app.h"

#include <eflpp_ecore.h>
using namespace efl;

#include <cstdlib>
#include <iostream>
using namespace std;

BootManagerApp::BootManagerApp( int argc, const char** argv, const char* title )
               :EcoreApplication( argc, argv, title )
{
}

BootManagerApp::~BootManagerApp()
{
}

void BootManagerApp::suspend( const char*, const char* )
{
    cout << "APP: suspending..." << endl;
    //FIXME Add confirmation dialog
    //::system( "apm --suspend" );
}

void BootManagerApp::reboot()
{
    cout << "APP: rebooting..." << endl;
    //FIXME Add confirmation dialog
    //::system( "reboot" );
}
