#ifndef OSS_MIXER_H
#define OSS_MIXER_H

#include <Evas.h>

Evas_List *oss_get_cards (void);
void      *oss_get_card  (int id);

#endif
