#ifndef _ENTICE_CONFIG_H
#define _ENTICE_CONFIG_H

#define SOFTWARE_X11 0
#define GL_X11 1

#include<Evas.h>

/** 
 * _Entice_Config - there is a static struct of this type in
 * entice_config.c, add the attributes you want to be in the config
 * here.  Then add an accessor function to query it from somewhere else
 */
struct _Entice_Config
{
   char *theme;
   char *db;
   char *editor;

   int engine;
   int image_quality;
   int auto_orient;

   struct
   {
      int image, font;
   }
   cache;
   int x, y, w, h;
   struct
   {
      Evas_List *list;
      Evas_Hash *hash;
   }
   themes;
};
typedef struct _Entice_Config Entice_Config;

void entice_config_init(void);

/* Accessors */
const char *entice_config_theme_get(void);
int entice_config_engine_get(void);
int entice_config_font_cache_get(void);
int entice_config_image_cache_get(void);
void entice_config_geometry_get(int *x, int *y, int *w, int *h);
void entice_config_geometry_set(int x, int y, int w, int h);
int entice_config_image_quality_get(void);
const char *entice_config_editor_get(void);
int entice_config_image_auto_orient_get(void);

#endif
