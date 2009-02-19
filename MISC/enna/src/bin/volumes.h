#ifndef __ENNA_VOLUMES_H__
#define __ENNA_VOLUMES_H__

#include "enna.h"

typedef struct _Enna_Volume Enna_Volume;

struct _Enna_Volume
{
    const char *name;
    const char *label;
    const char *icon; /* edje or file icon*/
    const char *uri;  /* Uri of root : file:///media/disk-1 or cdda:// */
    const char *type; /* Uri type : cdda:// dvdnav:// file:// ...*/
};

int ENNA_EVENT_VOLUME_ADDED;
int ENNA_EVENT_VOLUME_REMOVED;

void enna_volumes_init(void);
void enna_volumes_shutdown(void);
void enna_volumes_append(const char *type, Enna_Volume *v);
void enna_volumes_remove(const char *type, Enna_Volume *v);
Eina_List *enna_volumes_get(const char *type);

#endif
