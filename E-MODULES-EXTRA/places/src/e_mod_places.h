#ifndef E_MOD_PLACES_H
#define E_MOD_PLACES_H


typedef struct _Volume
{
   const char *udi;
   const char *label;
   unsigned char mounted;
   const char *mount_point;
   const char *fstype;
   unsigned long long size;

   const char *bus;
   const char *drive_type;
   const char *model;
   const char *vendor;
   const char *serial;
   unsigned char removable;
   unsigned char requires_eject;

   E_DBus_Signal_Handler *sh_prop;
   unsigned char valid;
   unsigned char to_mount;
   unsigned char force_open;
   Evas_Object *obj;
   const char *icon;

}Volume;

void places_init(void);
void places_shutdown(void);
void places_update_all_gadgets(void);
void places_fill_box(Evas_Object *box);
void places_empty_box(Evas_Object *box);

void places_generate_menu(void *data, E_Menu *em);
void places_augmentation(void *data, E_Menu *em);


#endif

