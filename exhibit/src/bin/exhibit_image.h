/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef _EX_IMAGE_H
#define _EX_IMAGE_H

Etk_Bool      _ex_image_mouse_wheel(Etk_Object *object, void *event, void *data);
void          _ex_image_mouse_down(Etk_Object *object, void *event, void *data);
void          _ex_image_mouse_up(Etk_Object *object, void *event, void *data);
void          _ex_image_mouse_move(Etk_Object *object, void *event, void *data);
void          _ex_image_undo(Etk_Image *im);
void          _ex_image_flip_horizontal(Etk_Image *im);
void          _ex_image_flip_vertical(Etk_Image *im);
void          _ex_image_flip_diagonal(Etk_Image *im, int direction);    
void          _ex_image_blur(Etk_Image *im);
void          _ex_image_sharpen(Etk_Image *im);
void          _ex_image_save(Etk_Image *im);
void	      _ex_image_save_as_entry_cb(Etk_Object *object, Etk_Event_Key_Down *ev, void *data);
int	      _ex_image_save_as_cb(void *data);
Ex_Filedialog *_ex_image_file_dialog_new(void);
void          _ex_image_file_dialog_destroy(Ex_Filedialog *fd);
void	      _ex_image_move_entry_cb(Etk_Object *object, Etk_Event_Key_Down *ev, void *data);
void	      _ex_image_move_cb(void *data);
void          _ex_image_save_as(void);
void	      _ex_image_delete(Exhibit *e);
void	      _ex_image_move(void);
void	      _ex_image_rename(void);
void	      _ex_image_refresh(void);
void          _ex_image_run(const char *app);
void          _ex_image_zoom(Etk_Image *im, int zoom); 
void          _ex_image_brightness(Etk_Image *im, int brightness);
void          _ex_image_wallpaper_set(Etk_Image *im);
Etk_Bool      _ex_image_is_favorite(Exhibit *e);
Etk_Tree_Row *_ex_image_find_row_from_file(Ex_Tab *tab, const char *filename);

#endif
