#include "entropy.h"

int allocated_events = 0;
int allocated_files = 0;
int allocated_thumbnails = 0;
int allocated_gui_file = 0;

void print_allocation() {
	return;
	
	printf ("%d allocated events\n", allocated_events);
	printf ("%d allocated files\n", allocated_files);
	printf ("%d allocated thumbnails\n", allocated_thumbnails);
	printf ("%d allocated gui files\n", allocated_gui_file);

	printf("\n");
	
}
