#include "Elapse.h"
#include "config.h"

int main(int argc, char *argv[])
{
	Elapse elapse;
	int i = 0;

	elapse.conf.sticky = 1;
	elapse.conf.lower = 1;
	strncpy(elapse.conf.theme, PACKAGE_DATA_DIR "/themes/elapse.eet",
			PATH_MAX);
	elapse.conf.debug = DEBUG_LEVEL;
	elapse.conf.format = NULL;

	while (++i < argc) {
		if (!strcmp(argv[i], "-h") || 
		    !strcmp(argv[i], "--help")) {
			show_help();
		}
		
		if (!strcmp(argv[i], "-q") ||
		    !strcmp(argv[i], "--quiet")) {
		    	elapse.conf.debug = 1;
		}
		
		if (!strcmp(argv[i], "-v") ||
		    !strcmp(argv[i], "--verbose")) {
		    	if (atoi(argv[argc]) < 0 ) elapse.conf.debug = 1;
		    	else elapse.conf.debug = atoi(argv[argc]);
		}

		if (!strcmp(argv[i], "-f") ||
		    !strcmp(argv[i], "--format")) {
		    	if (!argv[++i]) show_help();
			elapse.conf.format = malloc(sizeof(argv[i]));
			strcpy(elapse.conf.format, argv[i]);
		}

		if (!strcmp(argv[i], "-n") ||
		    !strcmp(argv[i], "--not-sticky")) {
			elapse.conf.sticky = 0;
		}

		if (!strcmp(argv[i], "-t") ||
		    !strcmp(argv[i], "--theme")) {
			if (!argv[++i]) show_help();
			strncpy(elapse.conf.theme, argv[i], PATH_MAX);
		}
	}

	elapse_gui_init(&elapse);

	debug(DEBUG_INFO, &elapse, "ecore_timer_add()");
	elapse.timer = ecore_timer_add(1, (void *)elapse_time_set, &elapse);

	debug(DEBUG_INFO, &elapse, "ecore_main_loop_begin()");
	ecore_main_loop_begin();

	debug(DEBUG_INFO, &elapse, "freeing memory");
	free(elapse.conf.format);

	debug(DEBUG_INFO, &elapse, "exiting elapse");	
	return 0;
}

void show_help(void)
{
	printf("Elapse - The Enlightened Clock\n");
	printf("By DigitalFallout\n");
	printf("=-=-=-=-=-=-=-=-=\n");
	printf("Runtime Options\n");
	printf("-f   --format\t\tSpecify a time format, see strftime(3)\n");
	printf("-h   --help\t\tPrint this help screen and exit\n");
	printf("-n   --not-sticky\tDisable sticky behaviour\n");
	printf("-q   --quiet\t\tSurpress all output\n");
	printf("-t   --theme\t\tPath to the theme you wish to use\n");
	printf("-v   --verbose\t\tSet a new verbosity level (1-4)\n");
	exit(1);
}
	
