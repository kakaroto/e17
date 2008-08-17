#include "app.h"
#include "win.h"

/* EFL */
#include <eflpp_ecore.h>
#include <eflpp_evas.h>
#include <eflpp_edje.h>

#include <assert.h>

using namespace efl;

BootManagerWin::BootManagerWin( int width, int height )
    :EcoreEvasWindow( width, height ), _app( static_cast<BootManagerApp*>( eApp ) )
{
    EvasCanvas* evas = canvas();

    //FIXME: Why does that crash if created in slotCalibrationDone() ?
    background = new EvasEdje( 0, 0, PACKAGE_DATA_DIR "/edjes/angstrom-bootmanager.edj", "background", evas );
    background->setLayer( 0 );
    background->resize( evas->size() );
    background->part( "version" )->setText( "Angstrom V2006.01" );
    background->show();

    //FIXME: Focus handling on buttons is not quite right if and only if the calibration is created... evas/edje bug?
    buttons = new EvasEdje( 0, 0, PACKAGE_DATA_DIR "/edjes/angstrom-bootmanager.edj", "buttons", evas );
    buttons->setLayer( 1 );
    buttons->resize( evas->size() );
    buttons->show();

    buttons->connect( "angstrom|system|suspend", "*", sigc::mem_fun( _app, &BootManagerApp::suspend ) );
    buttons->connect( "angstrom|system|reboot", "*", sigc::hide(sigc::hide(sigc::mem_fun( _app, &BootManagerApp::reboot ) ) ) );
}

BootManagerWin::~BootManagerWin()
{
}

void BootManagerWin::slotCalibrationDone()
{
    // scan available boot devices and show them in a list
}
