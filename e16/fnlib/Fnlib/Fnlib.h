#ifndef __FNLIB_H__
#define __FNLIB_H__

#include <Fnlib_types.h>

#ifdef __cplusplus
extern              "C"
{
#endif				/* __cplusplus */

  FnlibData          *Fnlib_init(ImlibData * id);
  FnlibFont          *Fnlib_load_font(FnlibData * fd, char *name);
  void                Fnlib_free_font(FnlibData * fd, FnlibFont * fn);
  void                Fnlib_add_dir(FnlibData * fd, char *dir);
  void                Fnlib_del_dir(FnlibData * fd, char *dir);
  char              **Fnlib_list_dirs(FnlibData * fd, int *count);
  void                Fnlib_draw(FnlibData * fd, FnlibFont * fn, Drawable win, Pixmap mask,
				 int x, int y, int width, int height, int xin, int yin,
				 int size, FnlibStyle * style, unsigned char *text);
  int                 Fnlib_measure(FnlibData * fd, FnlibFont * fn,
				    int x, int y, int width, int height, int xin, int yin,
				    int size, FnlibStyle * style, unsigned char *text,
				    int px, int py, int *ret_x, int *ret_y,
				    int *ret_width, int *ret_height, int *ret_char_x,
				    int *ret_char_y, int *ret_char_width, int *ret_char_height);
#ifdef __cplusplus
}
#endif				/* __cplusplus */

#endif
