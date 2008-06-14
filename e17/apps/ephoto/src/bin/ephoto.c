#include "ephoto.h"

Ephoto_Main *em;

int 
main(int argc, char **argv)
{
        if (!ewl_init(&argc, argv))
        {
                printf("Ewl is not usable, check your installation!\n");
                return 1;
        }

#ifdef ENABLE_NLS
	setlocale(LC_MESSAGES, "");
	bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif

	create_main();
	
	ewl_main();

	return 0;
}

