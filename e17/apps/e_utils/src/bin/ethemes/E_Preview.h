#ifndef _E_PREVIEW_H
#define _E_PREVIEW_H

#include <Evas.h>

/**
 * E_Preview - This is a smart object to be used to preview Enligthtenment DR17
 * themes. You simply provide the theme name, and let it do all the dirty work.
 */


/**
 * Create a new E_Preview objet.
 * @evas - the evas we want to use.
 */
Evas_Object * e_preview_new(Evas *evas);

/**
 * Set the theme file to use.
 * @object - the E_Preview objet
 * @theme - the name of the theme file, not the full path. i.e: default.edj
 */
void e_preview_theme_set(Evas_Object *object, const char * theme);


int e_preview_is_theme(Evas * evas, const char * theme);

/**
 * initialize thumb generation.
 */
int e_preview_thumb_init(void);

/**
 * Get the full path of the thumbnail
 */
char * e_preview_thumb_file_get(const char * theme);

/**
 * Check the validity of the thumbnail
 * @theme - the name of the theme file.
 */
int e_preview_thumb_check(const char * theme);

/**
 * Create a thumbnail for the current theme
 * @theme - the name of the theme file.
 */
int e_preview_thumb_generate(const char * theme);

/**
 * Set an evas object's data to the thumbnail's image
 * @theme - the name of the theme file
 * @obj - the evas image object
 */
int e_preview_thumb_image(const char * theme, Evas_Object * obj);

#endif

