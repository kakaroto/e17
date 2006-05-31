#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include "Epsilon_Request.h"
#include "Epsilon.h"
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
	printf("\n! EVENT ! THUMB %s -> %s COMPLETE ! STATUS %d ! %d REMAINING !\n\n",
			thumb->path, thumb->dest, thumb->status,  incomplete_thumbs);

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

	if (argc < 2) {
		printf ("Usage: epsilon_thumb_test <directory path>\n");
		exit(0);
	}

	epsilon_thumb_init();

	thumb_done = ecore_event_handler_add(EPSILON_EVENT_DONE, thumb_complete_cb, NULL);

	if (!ecore_file_is_dir(argv[1])) {
		printf("Need a directory\n");
		exit(-1);
	}

	files = ecore_file_ls(argv[1]);

	start = ecore_time_get();

	ecore_list_goto_first(files);
	while ((file = ecore_list_next(files))) {
		char *realpath;
		char fullpath[PATH_MAX];

		snprintf(fullpath, PATH_MAX, "%s/%s", argv[1], file);
		realpath = ecore_file_realpath(fullpath);
		if (ecore_file_exists(realpath) && !ecore_file_is_dir(realpath))
			epsilon_add(realpath, NULL, EPSILON_THUMB_NORMAL, NULL);
		incomplete_thumbs++;
	}

	ecore_main_loop_begin();

	end = ecore_time_get();
	printf("\nEpsilon Thumb completed in %g seconds\n\n", end - start);

	epsilon_shutdown();
#if 0
	sleep(5);

	start = ecore_time_get();
	ecore_list_goto_first(files);
	while ((file = ecore_list_next(files))) {
		pid_t pid;
		char *realpath;
		char fullpath[PATH_MAX];

		snprintf(fullpath, PATH_MAX, "%s/%s", PATH, file);
		realpath = ecore_file_realpath(fullpath);

		if (ecore_file_exists(realpath) && !ecore_file_is_dir(realpath)) {
			if ((pid = fork()) == 0) {
				Epsilon *ep;

				ep = epsilon_new(realpath);
				if (epsilon_exists(ep) == EPSILON_FAIL) {
					epsilon_generate(ep);
				}
				epsilon_free(ep);
				exit(0);
			}
			else if (pid > 0) {
				int status;
				waitpid(pid, &status, WNOHANG);
				printf("\n! EVENT ! THUMB COMPLETE ! %d REMAINING !\n\n", incomplete_thumbs);
			}
			incomplete_thumbs++;
		}
	}

	end = ecore_time_get();
	printf("\nFork & Wait completed in %g seconds\n\n", end - start);
#endif

	return 0;
}
