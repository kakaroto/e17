#ifndef GEIST_OBJECT_H
#define GEIST_OBJECT_H

typedef enum __geist_object_type {TYPE_OBJECT, TYPE_IMAGE, TYPE_LABEL}  geist_object_type;

struct __geist_object
{
   geist_object_type type;
   char *name;
   int width;
   int height;
   int x;
   int y;
   int visible;
   enum
   { SIZEMODE_ZOOM, SIZEMODE_STRETCH, SIZEMODE_CENTER, SIZEMODE_LEFT,
      SIZEMODE_RIGHT
   }
   sizemode;
   void (*free)(geist_object *obj);
   void (*render)(geist_object *obj, Imlib_Image im);
};

/* allocation functions */
geist_object    *geist_object_new(void);
void             geist_object_init(geist_object *obj);
void             geist_object_free(geist_object *obj);

void geist_object_render(geist_object *obj, Imlib_Image dest);
void geist_object_show(geist_object *obj);
void geist_object_int_free(geist_object *obj);


#endif
