#ifndef _ENESIM_TEST_H_
#define _ENESIM_TEST_H_

Enesim_Surface * surface_new(int w, int h, Enesim_Surface_Format fmt);
void surface_free(Enesim_Surface *s);
void png_load(Enesim_Surface *s, char *file);
void png_save(Enesim_Surface *s, char *file, int compress);

#endif /*_ENESIM_TEST_H_*/
