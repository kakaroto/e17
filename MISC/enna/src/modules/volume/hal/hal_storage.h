#ifndef HAL_STORAGE_H
#define HAL_STORAGE_H

typedef struct storage_s {
    LibHalDrive *drv;
    LibHalDriveType type;
    char *udi;
    char *bus;
    char *drive_type;

    char *model;
    char *vendor;
    char *serial;

    int removable;
    int media_available;
    unsigned long media_size;

    int requires_eject;
    int hotpluggable;
} storage_t;

void storage_free (storage_t *s);
storage_t *storage_append (LibHalContext *ctx, const char *udi);
storage_t * storage_find (Ecore_List *list, const char *udi);

#endif /* HAL_STORAGE_H */
