#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include "Epsilon_Request.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int incomplete_thumbs = 0;
static Ecore_Event_Handler *thumb_done = NULL;

int thumb_complete_cb(void *data, int type, void *event)
{
	Epsilon_Request *thumb = event;

	incomplete_thumbs--;
	if (thumb) 
		printf("\n! EVENT ! THUMB %s -> %s COMPLETE ! STATUS %d ! %d REMAINING !\n\n",
			thumb->path, thumb->dest, thumb->status,  incomplete_thumbs);
	else
		fprintf(stderr,"!!Warning!! NULL pointer (*event) : thumb_complete_cb(void *data, int type, void *ev)\n");

	if (incomplete_thumbs < 1)
		ecore_main_loop_quit();
	return 1;
	data = NULL;
	type = 0;
	event = NULL;
}

int main(int argc, char** argv)
{
	char *file;
	Ecore_List *files;
	double start, end;
	Epsilon_Thumb_Size thumb_size = EPSILON_THUMB_NORMAL;
	Epsilon_Thumb_Format thumb_format = EPSILON_THUMB_FDO;
	int i;

	if (argc < 2) {
		printf ("Usage: epsilon_thumb_test [-large] [-jpg] <directory path>\n");
		exit(0);
	}

	epsilon_request_init();

	thumb_done = ecore_event_handler_add(EPSILON_EVENT_DONE, thumb_complete_cb, NULL);

	if (!ecore_file_is_dir(argv[argc - 1])) {
		printf("Need a directory\n");
		exit(-1);
	}

	for(i=1;i<argc;i++)
		if (!strcmp(argv[i], "-large")) thumb_size = EPSILON_THUMB_LARGE;
		else if (!strcmp(argv[i], "-jpg")) thumb_format = EPSILON_THUMB_LARGE;

	files = ecore_file_ls(argv[argc - 1]);

	start = ecore_time_get();

	ecore_list_first_goto(files);
	while ((file = ecore_list_next(files))) {
		char *realpath;
		char fullpath[PATH_MAX];

		snprintf(fullpath, PATH_MAX, "%s/%s", argv[argc - 1], file);
		realpath = ecore_file_realpath(fullpath);
		if (ecore_file_exists(realpath) && !ecore_file_is_dir(realpath)) {
			epsilon_request_add_advanced(realpath, thumb_size, thumb_format, NULL);
			incomplete_thumbs++;
		}
		free(realpath);
	}
	ecore_list_destroy(files);

	ecore_main_loop_begin();

	end = ecore_time_get();
	printf("\nEpsilon Thumb completed in %g seconds\n\n", end - start);

	epsilon_request_shutdown();
	return 0;
}
