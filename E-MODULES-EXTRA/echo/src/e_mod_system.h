#ifndef E_MOD_SYSTEM_H
#define E_MOD_SYSTEM_H

typedef struct _System System;
struct _System 
{
   /* functions todo any init or shutdown 
    * needed by the system (alsa, oss, etc) */
   int (*init) (int card);
   int (*shutdown) (void);

   /* functions to retrieve cards or channels 
    * NB - should return just a list of names */
   Evas_List *(*get_cards) (void);
   Evas_List *(*get_channels) (void);

   /* function to return a channel name */
   char *(*get_channel_name) (int channel);

   /* functions to get or set volume
    * NB - returns 1 on success, 0 on failure */
   int (*get_volume) (const char *channel, int *left, int *right);
   int (*set_volume) (const char *channel, int left, int right);

   /* functions to get or set mute
    * NB - these will only work if the channel supports it
    * NB - returns 1 on success, 0 on failure */
   int (*can_mute) (const char *channel);
   int (*get_mute) (const char *channel, int *mute);
   int (*set_mute) (const char *channel, int mute);
};

EAPI int e_mod_system_init(int card);
EAPI int e_mod_system_shutdown(void);

extern System *echo_sys;

#endif
