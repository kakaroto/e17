#include <eflpp_sys.h>
#include <eflpp_ewl.h>

#include <iostream>
using namespace std;

using namespace efl;

int main( int argc, const char **argv )
{
    EwlApplication app( argc, argv, "Simple Ewl Demo Application" );

    EwlWindow* mainWindow = new EwlWindow();
    app.setMainWindow( mainWindow );
    mainWindow->resize( Size( 200, 100 ) );
    mainWindow->show();

    EwlVBox* vbox = new EwlVBox();
    mainWindow->appendChild( vbox );
    vbox->setFillPolicy( All );
    vbox->show();

    EwlButton* button = new EwlButton( "Hello World" );
    vbox->appendChild( button );
    button->setAlignment( Center );
    button->show();

    cout << "calling main now" << endl;

    app.exec();

    return 0;
}

