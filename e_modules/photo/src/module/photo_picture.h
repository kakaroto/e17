#ifdef E_MOD_PHOTO_TYPEDEFS

typedef struct _Picture Picture;
typedef struct _Picture_Event_Fill Picture_Event_Fill;

#else

#ifndef PHOTO_PICTURE_H_INCLUDED
#define PHOTO_PICTURE_H_INCLUDED

#define PICTURE_VIEWER_DEFAULT "exhibit"

#define PICTURE_FROM_DEFAULT 1
#define PICTURE_BOTH 0
#define PICTURE_LOCAL 1
#define PICTURE_NET 2

#define PICTURE_SET_BG_PURGE_DEFAULT 1

#define PICTURE_THUMB_SIZE_DEFAULT 300
#define PICTURE_THUMB_SIZE_MIN 100
#define PICTURE_THUMB_SIZE_MAX 600

struct _Picture
{
   Photo_Item *pi;

   const char *path;
   const char *thumb_path;
   Evas_Object *picture;

   struct
   {
      const char *name;
      const char *author;
      const char *where_from;
      const char *date;
      const char *comments;
   } infos;
   int delete;
   int from;
   int original_w, original_h;

   /*
    * each photo item where the picture is in the histo
    * is attached here, only one time
    */
   Evas_List *items_histo;
};

struct _Picture_Event_Fill
{
   int new;
   int type;
};

int          photo_picture_init(void);
void         photo_picture_shutdown(void);

int          photo_picture_load(Picture *pic, Evas *evas);
void         photo_picture_unload(Picture *pic);

int          photo_picture_free(Picture *p, int force, int force_now);
Evas_Object *photo_picture_object_get(Picture *pic, Evas *evas);
const char  *photo_picture_name_get(char *name);
char        *photo_picture_infos_get(Picture *p);

void         photo_picture_setbg_add(const char *name);
void         photo_picture_setbg_purge(int shutdown);

#endif
#endif
