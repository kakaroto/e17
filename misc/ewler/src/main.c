#include <getopt.h>
#include <termios.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "ewler.h"

static char *ewler_spec = PACKAGE_DATA_DIR"/schemas/widgets.xml"; 

int
main( int argc, char *argv[] )
{
	int c, nfiles;
	char **files;
	int batch = false;
	const char *flipper = "/-\\|";
	const char *flip = flipper;
	
	ewl_init( &argc, argv );

	ewler_spec_init();

	if( ewler_spec_read( ewler_spec ) < 0 )
		ewler_fatal( "Unable to load spec file: %s", ewler_spec );

	nfiles = argc;
	files = &argv[1];

	while( (c = getopt( argc, argv, "d:s:b" )) != -1 )
		switch( c ) {
			case 'd':
				options.debug = optarg[0] - '0';
				break;
			case 's':
				if( ewler_spec_read( optarg ) < 0 )
					ewler_error( "Unable to load user supplied spec file: %s", optarg );
				break;
			case 'b':
				/* batch mode */
				batch = true;
				break;
			case '?':
				if( optopt == 'c' )
					options.debug = 1;
				else {
					usage();
					ewler_fatal( "Unknown option: %c", optopt );
				}
				break;
		}

	files = argv + optind;
	nfiles = argc - optind;

	options.files = ecore_list_new();
	while( *files ) {
		printf( "opening %s\n", *files );
		ecore_list_append( options.files, *files++ );
	}

	ewler_project_init();
	ewler_ui_init();

	if( !batch )
		ewl_main();
	else {
		while( !ewler_is_done() ) {
			char *cmd;
			fd_set rfds;
			struct timeval tv;
			sigset_t sig, savesig;
			struct termios modes, savemodes;

			FD_ZERO(&rfds);
			FD_SET(0, &rfds);
			tv.tv_sec = 0;
			tv.tv_usec = 10000;

			sigemptyset(&sig);
			sigaddset(&sig, SIGINT);
			sigaddset(&sig, SIGQUIT);
			sigaddset(&sig, SIGTSTP);
			sigprocmask(SIG_BLOCK, &sig, &savesig);

			if( tcgetattr(0, &modes) < 0 ) {
				perror( "tcgetattr" );
				return -1;
			}

			savemodes = modes;

			modes.c_cc[VMIN] = 1;
			modes.c_cc[VTIME] = 0;
			modes.c_lflag &= ~ICANON;
			modes.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHOKE);

			if( tcsetattr(0, TCSAFLUSH, &modes) < 0 ) {
				perror( "tcsetattr" );
				return -1;
			}

			if( select( 1, &rfds, NULL, NULL, &tv ) > 0 ) {
				if( tcsetattr(0, TCSAFLUSH, &savemodes) < 0 ) {
					perror( "tcsetattr" );
					return -1;
				}

				sigprocmask( SIG_SETMASK, &savesig, (sigset_t *) 0);

				cmd = readline( "ewler% " );
				printf( "%s\n", cmd );
				if( !strcmp( cmd, "quit" ) )
					ewler_shutdown();
				free( cmd );
			}

			if( tcsetattr( 0, TCSAFLUSH, &savemodes ) < 0 ) {
				perror( "tcsetattr" );
				return -1;
			}

			sigprocmask( SIG_SETMASK, &savesig, (sigset_t *) 0 );
			
			if( !ewler_is_done() ) {
				printf( "%c\r", *flip++ );
				fflush( stdout );
				if( flip == &flipper[4] )
					flip = flipper;
			}

#if 0
			ecore_main_loop_iterate();
#endif
		}

		ewl_shutdown();
		/* do single iterations with readline as well */
	}

	return EXIT_SUCCESS;
}
