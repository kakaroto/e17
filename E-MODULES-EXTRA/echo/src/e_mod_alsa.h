#ifndef E_MOD_ALSA_H
#define E_MOD_ALSA_H

EAPI int e_mod_alsa_init(int card);
EAPI int e_mod_alsa_shutdown(void);
EAPI Evas_List *e_mod_alsa_get_cards(void);
EAPI Evas_List *e_mod_alsa_get_channels(void);
EAPI char *e_mod_alsa_get_channel_name(int channel);
EAPI int e_mod_alsa_get_volume(const char *channel, int *left, int *right);
EAPI int e_mod_alsa_set_volume(const char *channel, int left, int right);
EAPI int e_mod_alsa_can_mute(const char *channel);
EAPI int e_mod_alsa_get_mute(const char *channel, int *mute);
EAPI int e_mod_alsa_set_mute(const char *channel, int mute);

#endif
