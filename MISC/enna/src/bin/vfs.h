#ifndef __ENNA_VFS_H__
#define __ENNA_VFS_H__

#include "enna.h"

typedef enum _ENNA_VFS_CAPS ENNA_VFS_CAPS;
typedef struct _Enna_Class_Vfs Enna_Class_Vfs;
typedef struct _Enna_Vfs_File Enna_Vfs_File;

enum _ENNA_VFS_CAPS
{
    ENNA_CAPS_MUSIC = 0x01,
    ENNA_CAPS_VIDEO = 0x02,
    ENNA_CAPS_PHOTO = 0x04
};

struct _Enna_Vfs_File
{
    char *uri;
    char *label;
    char *icon;
    char *icon_file;
    unsigned char is_directory : 1;
    unsigned char is_selected : 1;

};

struct _Enna_Class_Vfs
{
    const char *name;
    int pri;
    const char *label;
    const char *icon_file;
    const char *icon;
    struct
    {
        void (*class_init)(int dummy);
        void (*class_shutdown)(int dummy);
        Eina_List *(*class_browse_up)(const char *path);
        Eina_List *(*class_browse_down)(void);
        Enna_Vfs_File *(*class_vfs_get)(void);
    } func;

};
EAPI int enna_vfs_init(Evas *evas);
EAPI int enna_vfs_append(const char *name, unsigned char type,
        Enna_Class_Vfs *vfs);
EAPI Eina_List *enna_vfs_get(ENNA_VFS_CAPS type);
EAPI Enna_Vfs_File *enna_vfs_create_file (char *uri, char *label, char *icon, char *icon_file);
EAPI Enna_Vfs_File *enna_vfs_create_directory (char *uri, char *label, char *icon, char *icon_file);
EAPI void enna_vfs_remove(Enna_Vfs_File *f);
#endif
