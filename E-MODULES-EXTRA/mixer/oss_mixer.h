#ifndef OSS_MIXER_H
#define OSS_MIXER_H

#include <Evas.h>

Evas_List *oss_get_cards(void);
void *oss_get_card(int id);
Evas_List *oss_card_get_channels(void *data);
void *oss_card_get_channel(void *data, int channel_id);
void oss_free_cards(void *data);

int oss_get_volume(int card_id, int channel_id);
int oss_set_volume(int card_id, int channel_id, double vol);

int oss_get_mute(int card_id, int channel_id);
int oss_set_mute(int card_id, int channel_id, int mute);
#endif
