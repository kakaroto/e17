#include "eflpp_housekeeping.h"

/* EFL++ */
#include <eflpp_sys.h>
#include <eflpp_debug_internal.h>

/* EFL */
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Config.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include <iostream>
using std::cout;
using std::endl;

#ifdef CWDEBUG
    libcwd::marker_ct* global_memory_marker = NULL;
#endif

void __attribute__ ((constructor)) eflplusplus_init(void)
{
    // initialize C libraries
    evas_init();
    ecore_init();
    if ( ecore_config_system_init() != ECORE_CONFIG_ERR_SUCC )
        Dout(dc::warning, "Couldn't initialize ecore_config_system" );
    ecore_evas_init();
    edje_init();

    // Don't show allocations that are allocated before main()
    Debug( make_all_allocations_invisible_except(NULL) );

    // Select channels
    ForAllDebugChannels( if (debugChannel.is_on()) debugChannel.off() );
    Debug( dc::notice.on() );
    Debug( dc::malloc.off() );
    Debug( dc::warning.on() );
    // Write debug output to cout
    Debug( libcw_do.set_ostream(&cout) );
    // Turn debug object on
    Debug( libcw_do.on() );
    Dout(dc::notice, "This is the EFL++ Debug Version." );
    Dout(dc::notice, "Debug Channels initialized.");

    Debug( global_memory_marker = new libcwd::marker_ct( "EFL++ malloc marker" ) );
}

void __attribute__ ((destructor)) eflplusplus_fini(void)
{
    Debug( delete global_memory_marker );

    edje_shutdown();
    ecore_evas_shutdown();
    ecore_config_system_shutdown();
    ecore_shutdown();
    evas_shutdown();
}

