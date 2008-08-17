#include <string.h>
#include <etk/Etk.h>
#include "Entrance_Widgets.h"


int
ew_init(int *argc, char*** argv)
{
	return etk_init(argc, argv);
}

void 
ew_main()
{
	etk_main();
}

void 
ew_main_quit()
{
	etk_main_quit();
}

int
ew_shutdown()
{
	return etk_shutdown();
}

Entrance_Widget 
ew_new(void)
{
	Entrance_Widget ew = calloc(1, sizeof(*ew));
	if(ew) 
	{
		ew->title = NULL;
		ew->owner = NULL;
		ew->box = NULL;
	}

	return ew;
}

void
ew_title_set(Entrance_Widget ew, const char *title)
{
	if(!title)
		return;

	if(ew->title)
		free(ew->title);

	ew->title = strdup(title);
}


/* privates */
