/*
   Module       : utils.c
   Purpose      : Various utilities for qiv
   More         : see qiv README
   Policy       : GNU GPL
   Homepage     : http://www.klografx.de/
 */

#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gdk/gdkx.h>
#include "eplay.h"

/* copy current image to .qiv-trash */
int move2trash(char *filename)
{
	char *trashdir = ".qiv-trash";
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

/*
   Update selected image index image_idx
   Direction determines if the next or the previous
   image is selected.
 */
void next_image(char direction)
{
	image_idx = (image_idx + 1 + images) % images;
}

void usage(char *name, int exit_status)
{
	g_print("qiv (Quick Image Viewer) v%s\n"
			"Usage: %s [options] files ...\n"
			"See 'man qiv' or type '%s --help' for options.\n\n"
			,VERSION, name, name);

	gdk_exit(exit_status);
}

void show_help(char *name, int exit_status)
{
	int i;

	g_print("\nqiv (Quick Image Viewer) v%s\n"
			"Usage: %s [options] files ...\n\n"
			,VERSION, name);

	g_print("General options:\n"
			"     --help, -h          : This help screen\n"
			"     --display <disp>    : Open qiv window on display <disp>\n"
			"     --wm, -w x          : Window Manager handling (0-2)\n"
			"     --center, -e        : Disable window centering. Use this if no -w x works.\n"
			"     --root, -x          : Set image as desktop background (centered/and exit)\n"
			"     --root_t, -y        : Set image as desktop background (tiled/and exit)\n"
			"     --root_s, -z        : Set image as desktop background (stretched/and exit)\n"
			"     --maxpect, -m       : Zoom to screen size while preserving aspect ratio\n"
			"     --scale_down, -t    : Shrink image(s) which are larger than screen size\n"
			"                           to fit.\n"
			"     --fullscreen, -f    : Use fullscreen window on start-up\n"
			"     --brightness, -b x  : Set brightness to x (-32..32)\n"
			"     --contrast, -c x    : Set contrast to x (-32..32)\n"
			"     --gamma, -g x       : Set gamma to x (-32..32)\n"
			"     --no_filter, -n     : Do not filter images by extension\n"
			"     --no_statusbar, -i  : Disable statusbar in fullscreen_mode\n"
	 "     --transparency, -p  : Enable transparency for transparent images\n"
			"     --do_grab, -a       : Grab the pointer in windowed mode\n"
			"     --version, -v       : Print version information and exit\n"
			"     --bg_color, -o x    : Set root background color to 24 bit hex RGB tripple x\n"
			"Slideshow options:\n"
			"     --slide, -s         : Start slideshow immediately\n"
			"     --random, -r        : Random order\n"
			"     --delay,-d <delay>  : Wait <delay> seconds between images [default=3]\n\n"
			"Please look into 'man qiv' for some examples.\n\n"
			"Keys:\n"
			"     space/PgDn/left button/scroll down\t\tnext picture\n"
			"     backspace/PgUp/right button/scroll up\tprevious picture\n"
			"     d/D/delete\t\t\t\t\tmove picture to .qiv-trash\n"
			"     +/=\t\t\t\t\tzoom_in  (10%%)\n"
			"     -\t\t\t\t\t\tzoom_out (10%%)\n"
			"     f\t\t\t\t\t\tfullscreen mode on/off\n"
			"     t\t\t\t\t\t\tscale_down on/off\n"
			"     s\t\t\t\t\t\tslide show on/off\n"
			"     r\t\t\t\t\t\trandom order (slideshow) on/off\n"
			"     b\t\t\t\t\t\t- brightness\n"
			"     B\t\t\t\t\t\t+ brightness\n"
			"     c\t\t\t\t\t\t- contrast\n"
			"     C\t\t\t\t\t\t+ contrast\n"
			"     g\t\t\t\t\t\t- gamma\n"
			"     G\t\t\t\t\t\t+ gamma\n"
			"     h\t\t\t\t\t\tflip horizontal\n"
			"     v\t\t\t\t\t\tflip vertical\n"
			"     right arrow\t\t\t\trotate right\n"
			"     left arrow\t\t\t\t\trotate left\n"
			"     jtx<return>\t\t\t\tjump to image number x\n"
			"     jfx<return>\t\t\t\tjump forward x images\n"
			"     jbx<return>\t\t\t\tjump backward x images\n"
			"     enter/return\t\t\t\treset zoom and color settings\n"
			"     i\t\t\t\t\t\tstatusbar on/off\n"
			"     x\t\t\t\t\t\tcenter image on background\n"
			"     y\t\t\t\t\t\ttile image on background\n"
			"     z\t\t\t\t\t\tstretch image on background\n"
			"     q/ESC/middle button\t\t\texit\n\n");

	g_print("Valid image extensions:\n");

	for (i = 0; strcmp(image_extensions[i], "end") != 0; i++)
		g_print("%s%s", image_extensions[i], (i == 8) ? "\n" : " ");
	g_print("\n\n");

	g_print("Homepage: http://www.klografx.de/\n"
			"Please mail bug reports and comments to Adam Kopacz <Adam.K@klografx.de>\n\n");

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
