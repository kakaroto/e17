#include <Edje.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/soundcard.h>
#include "eplayer.h"

/************ STOLEN FROM MOC (Music on CLI) *********************/
/* Get PCM volume, return -1 on error */
int read_mixer (ePlayer *player) {
	int vol;
	int mixer_fd;

#ifdef DEBUG
	printf("DEBUG: Reading mixer\n");
#endif

	mixer_fd = open ("/dev/mixer", O_RDWR);

        if (mixer_fd == -1) {
                open ("/dev/mixer0", O_RDWR);
        }
        if (mixer_fd == -1) {
                printf("MIXER: Can't open mixer device\n");
		return 0;
        }

        if (mixer_fd != -1) {
                if (ioctl(mixer_fd, MIXER_READ(SOUND_MIXER_PCM), &vol) == -1) {
#ifdef DEBUG
                        printf("MIXER: Can't read from mixer\n");
#endif
				} else {
			int return_val;
                        /* Average between left and right */
                        return_val =  ((vol & 0xFF) + ((vol >> 8) & 0xFF)) / 2;
#ifdef DEBUG
			printf("MIXER: Returning value: %d\n", return_val);
#endif
			close(mixer_fd);
	

	/* Update the display with the volume level */
	{

                char vol_str[3];

                sprintf(vol_str, "%d", (int)return_val);

#ifdef DEBUG
                printf("DEBUG: Setting the mixer vol: %s\n", vol_str);
#endif

                edje_object_part_text_set(player->gui.edje, "vol_display_text", vol_str);
	}
/*

		if(player->edje) {
			if(return_val > 99) 
				return_val == 99;
			if(return_val < 1) 
				edje_object_part_text_set(player->edje, "vol_display_text", "--");
			if(return_val < 10) 
				sprintf(return_val, "0%s", (int)return_val);

			edje_object_part_text_set(player->edje, "vol_display_text", return_val);
		}
*/
			return return_val;
                }
        }

        return -1;
}

/* Set PCM volume */
void set_mixer(int vol) {

        int mixer_fd;

#ifdef DEBUG
        printf("DEBUG: Setting mixer\n");
#endif

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
                if (ioctl(mixer_fd, MIXER_WRITE(SOUND_MIXER_PCM), &vol) == -1) {
#ifdef DEBUG
                        printf("DEBUG: Can't set mixer\n");
#endif
				}
        }
	close(mixer_fd);
}

