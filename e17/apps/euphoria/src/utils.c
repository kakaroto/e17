/*
 * $Id$
 * vim:noexpandtab:sw=4:sts=4:ts=4
 */

#include <config.h>
#include <Evas.h>
#include <Edje.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include "utils.h"

/**
 * Finds the filename for the theme @name.
 * Looks in: ~/.e/apps/euphoria/themes
 *           $prefix/share/euphoria/themes
 */
char *find_theme(const char *name) {
	static char eet[PATH_MAX + 1];
	struct stat st;

	snprintf(eet, sizeof(eet),
	         "%s/.e/apps/" PACKAGE "/themes/%s.eet",
	         getenv("HOME"), name);

	if (!stat(eet, &st))
		return eet;

	snprintf(eet, sizeof(eet), DATA_DIR "/themes/%s.eet", name);

	return stat(eet, &st) ? NULL : eet;
}

bool is_dir(const char *dir) {
	struct stat st;

	if (stat(dir, &st))
		return false;

	return (S_ISDIR(st.st_mode));
}

/**
 * Removes leading and trailing whitespace from a string.
 *
 * @param str String to strip
 * @return Stripped string
 */
char *strstrip(char *str) {
	char *start, *ptr = str;

	/* step over leading whitespace */
	for (start = str; isspace(*start); start++);

	if (str != start) {
		while ((*ptr++ = *start++));
		*ptr = 0;
	}

	if (!*str)
		return str;

	/* remove trailing whitespace */
	ptr = &str[strlen(str) - 1];

	if (!isspace(*ptr))
		return str;

	while (isspace(*ptr) && ptr > str)
		ptr--;

	ptr[1] = 0;

	return str;
}

void debug(DebugLevel level, const char *fmt, ...) {
	va_list list;

	if (level > DEBUG_LEVEL || !fmt || !fmt[0])
		return;

	va_start(list, fmt);
	vprintf(fmt, list);
	va_end(list);
}

const char *get_login() {
	int uid = getuid();
	struct passwd *pw;
	static char ret[64] = {0};

	if (ret[0])
		return ret;

	setpwent();

	while ((pw = getpwent()))
		if (pw->pw_uid == uid)
			snprintf(ret, sizeof(ret), "%s", pw->pw_name);

	endpwent();

	return ret;
}

/************ STOLEN FROM MOC (Music on CLI) *********************/
/* Get PCM volume, return -1 on error */
int read_mixer(Euphoria *e) {
        int vol;
        int mixer_fd;

        printf("DEBUG: Reading mixer\n");

        mixer_fd = open ("/dev/mixer", O_RDWR);

        if (mixer_fd == -1) {
                open ("/dev/mixer0", O_RDWR);
        }
        if (mixer_fd == -1) {
                printf("MIXER: Can't open mixer device\n");
                return 0;
        }

        if (mixer_fd != -1) {
                if (ioctl(mixer_fd, MIXER_READ(SOUND_MIXER_PCM), &vol) == -1)
                        printf("MIXER: Can't read from mixer\n");
                else {
                        int return_val;
                        /* Average between left and right */
                        return_val =  ((vol & 0xFF) + ((vol >> 8) & 0xFF)) / 2;
                        printf("MIXER: Returning value: %d\n", return_val);
                        close(mixer_fd);


        /* Update the display with the volume level */
        {

                char vol_str[3];

                sprintf(vol_str, "%d", (int)return_val);
                printf("DEBUG: Setting the mixer vol: %s\n", vol_str);

                edje_object_part_text_set(e->gui.edje, "vol_display_text",  vol_str);
        }
/*

                if(st_session->edje) {
                        if(return_val > 99)
                                return_val == 99;
                        if(return_val < 1)
                                edje_object_part_text_set(st_session->edje, "vol_display_text", "--");
                        if(return_val < 10)
                                sprintf(return_val, "0%s", (int)return_val);

                        edje_object_part_text_set(st_session->edje, "vol_display_text", return_val);
                }
*/
                        return return_val;
                }
        }

        return -1;
}

/* Set PCM volume */
void set_mixer(int vol){

        int mixer_fd;

        printf("DEBUG: Setting mixer\n");

        mixer_fd = open ("/dev/mixer", O_RDWR);

        if (mixer_fd == -1)
                mixer_fd = open ("/dev/mixer0", O_RDWR);

        if (mixer_fd == -1) {
                printf("MIXER: Can't open mixer device\n");
                return;
        }

        
        if (mixer_fd != -1) {
                if (vol > 100)
                        vol = 100;
                else if (vol < 0)
                        vol = 0;
                
                vol = vol | (vol << 8);
                if (ioctl(mixer_fd, MIXER_WRITE(SOUND_MIXER_PCM), &vol) == -1)
                        printf("DEBUG: Can't set mixer\n");
        }
        close(mixer_fd);
}
/********** END THEFT *********************************************/

