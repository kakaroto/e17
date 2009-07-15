#include "app.h"
#include "win.h"

// Ångström BootManager

int main( int argc, const char **argv )
{
    BootManagerApp application( argc, argv, "Angstrom BootManager" );
    BootManagerWin* window = new BootManagerWin( 240, 320 );
    window->show();

    application.exec();

    return 0;
}

