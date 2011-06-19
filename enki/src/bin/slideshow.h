#ifndef SLIDESHOW_H_
#define SLIDESHOW_H_

#include <Elementary.h>
#include <Enlil.h>
#include "enlil_data.h"

extern const char *Theme;

//slideshow
void
slideshow_show();
void
slideshow_album_add(Enlil_Album *_album, Enlil_Photo *photo);
void
slideshow_library_add(Enlil_Library *root, Enlil_Photo *photo);
void
slideshow_clear();
void
slideshow_hide();
Evas_Object *
slideshow_win_get();
void
slideshow_start();

#endif /* SLIDESHOW_H_ */
