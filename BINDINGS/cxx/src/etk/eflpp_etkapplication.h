#ifndef EFLPP_ETKAPPLICATION_H
#define EFLPP_ETKAPPLICATION_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>
#include <eflpp_countedptr.h>
#include "eflpp_etkwindow.h"

#define etkApp EtkApplication::application()

using std::string;

namespace efl {

class EtkApplication
{
    public:
    EtkApplication( int argc, const char** argv, const char* name );
    ~EtkApplication();

    static EtkApplication* application();
    const char* name() const { return _name; };

    /* Main Window */
    void setMainWindow( EtkWindow* );
    EtkWindow* mainWindow() const;

    /* Main Loop */
    void exec();
    void exit();

    private:
    const char* _name;
    static EtkApplication* _instance;
    EtkWindow* _mainWindow;

};


} // end namespace efl

#endif // EFLPP_ETKAPPLICATION_H
