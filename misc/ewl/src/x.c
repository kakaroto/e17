#include "x.h"

Display *ewl_get_display()
{
	static Display *display = NULL;
	char           *t[2] = {NULL,NULL};
	if (!display)	{
		display = XOpenDisplay(NULL);
		if (!display)	{
			t[0] = getenv("DISPLAY");
			t[1] = malloc(1024);
			sprintf(t[1], "Could not open X Display (DISPLAY=\"%s\").",
			        t[0]?t[0]:"null");
			ewl_fatal(t[1]);
		}
	}
	return display;
}

