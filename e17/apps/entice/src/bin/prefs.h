#ifndef _ENTICE_CONFIG_H
#define _ENTICE_CONFIG_H

#define SOFTWARE_X11 0
#define GL_X11 1

/** 
 * _Entice_Config - there is a static struct of this type in
 * entice_config.c, add the attributes you want to be in the config
 * here.  Then add an accessor function to query it from somewhere else
 */
struct _Entice_Config
{
   char *theme;
   int engine;
   struct
   {
      int image, font;
   } cache;
   int x, y, w, h;
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

#endif
