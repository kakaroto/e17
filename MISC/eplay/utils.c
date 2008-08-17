/*
   Module       : utils.c
   Purpose      : Various utilities for eplay
   More         : see eplay README
   Policy       : GNU GPL
   Homepage     : http://mandrake.net
 */

#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gdk/gdkx.h>
#include "eplay.h"

/* copy current image to .eplay-trash */
int move2trash(char *filename)
{
	char *trashdir = ".eplay-trash";
	char *ptr, *ptr2;
	char trashfile[1024];
	int i = 0;

	ptr2 = malloc(sizeof(char) * 1024);

	if (!realpath(filename, ptr2)) {
		g_print("Error: Could not move file to trash\n");
		return 1;
	}
	strcpy(trashfile, trashdir);
	strcat(trashfile, ptr2);

	ptr = ptr2 = trashfile;
	while ((ptr = strchr(ptr, '/'))) {
		*ptr = '\0';
		if (access(ptr2, F_OK)) {
			if (mkdir(ptr2, 0700)) {
				g_print("Error: Could not make directory %s\n", ptr2);
				return 1;
			}
		}
		*ptr = '/';
		ptr += 1;
	}

	if (link(filename, trashfile)) {
		g_print("Error: Could not copy file to %s\n", trashfile);
		return 1;
	}
	if (!unlink(filename)) {
		--images;
		for (i = image_idx; i < images; ++i) {
			image_names[i] = image_names[i + 1];
		}

		/* If deleting the last file out of x */
		if (images == image_idx)
			--image_idx;

		/* If deleting the only file left */
		if (!images)
			gdk_exit(0);
	} else {
		g_print("Error: Could not write to %s\n", trashdir);
		return 1;
	}
	return 0;
}

/* 
   This routine jumps x images forward or backward or
   directly to image x
   Enter jf10\n ... jumps 10 images forward
   Enter jb5\n  ... jumps 5 images backward
   Enter jt15\n ... jumps to image 15
 */
void jump2image(char *cmd)
{
	int direction = 0;
	int x;

	if (cmd[0] == 'f' || cmd[0] == 'F')
		direction = 1;
	else if (cmd[0] == 'b' || cmd[0] == 'B')
		direction = -1;
	else if (!(cmd[0] == 't' || cmd[0] == 'T'))
		return;

	/* get number of images to jump or image to jump to */
	x = atoi(cmd + 1);

	if (direction == 1) {
		if ((image_idx + x) > (images - 1))
			image_idx = images - 1;
		else
			image_idx += x;
	} else if (direction == -1) {
		if ((image_idx - x) < 0)
			image_idx = 0;
		else
			image_idx -= x;
	} else {
		if (x > images || x < 1)
			return;
		else
			image_idx = x - 1;
	}
}

void finish(int sig)
{
	gdk_pointer_ungrab(CurrentTime);
	gdk_keyboard_ungrab(CurrentTime);
	gdk_exit(0);
}

void usage(char *name, int exit_status)
{
	g_print("eplay (the e flipbook) v%s\n"
			"Usage: %s files ...\n"
			,VERSION, name);

	gdk_exit(exit_status);
}

void show_help(char *name, int exit_status)
{
	int i;

	g_print("\neplay (the e flipbook) v%s\n"
			"Usage: %s files ...\n\n"
			,VERSION, name);

	g_print("Valid image extensions:\n");

	for (i = 0; strcmp(image_extensions[i], "end") != 0; i++)
		g_print("%s%s", image_extensions[i], (i == 8) ? "\n" : " ");
	g_print("\n\n");

	g_print("Homepage: http://mandrake.net/\n"
			"Please mail bug reports and comments to Mandrake <mandrake@mandrake.net>\n\n");

	gdk_exit(exit_status);
}

void set_bg_color(char *option)
{
	char temp[5], color[6] = "000000", color_found = 0;
	int i = 0;

	while (strcmp(color_names[i], "end")) {
		if (!strcasecmp(color_names[i], option)) {
			strncpy(color, color_numbers[i], 6);
			color_found = 1;
			break;
		}
		++i;
	}

	if (!color_found)
		strncpy(color, option, 6);

	temp[0] = temp[2] = color[0];
	temp[1] = temp[3] = color[1];
	temp[4] = '\0';
	color_bg.red = strtol(temp, NULL, 16);

	temp[0] = temp[2] = color[2];
	temp[1] = temp[3] = color[3];
	color_bg.green = strtol(temp, NULL, 16);

	temp[0] = temp[2] = color[4];
	temp[1] = temp[3] = color[5];
	color_bg.blue = strtol(temp, NULL, 16);

	return;
}
