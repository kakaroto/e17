#ifndef HAL_VOLUME_H
#define HAL_VOLUME_H

typedef enum {
    VOLUME_TYPE_UNKNOWN,
    VOLUME_TYPE_HDD,
    VOLUME_TYPE_CD,
    VOLUME_TYPE_CDDA,
    VOLUME_TYPE_DVD,
    VOLUME_TYPE_DVD_VIDEO,
    VOLUME_TYPE_VCD,
    VOLUME_TYPE_SVCD,
} volume_type_t;

typedef struct volume_s {
    LibHalVolume *vol;
    volume_type_t type;
    char *name;
    char *udi;
    char *parent;
    char *cd_type;
    char *cd_content_type;
    char *label;
    char *fstype;

    char *partition_label;
    int mounted;
    char *mount_point;
    unsigned long size;

    storage_t *s;

    Enna_Volume *enna_volume;

} volume_t;

void volume_free (volume_t *v);
volume_t *volume_append (LibHalContext *ctx, const char *udi);
volume_t *volume_find (Ecore_List *list, const char *udi);

#endif /* HAL_VOLUME_H */
