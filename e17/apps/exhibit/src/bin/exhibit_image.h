#ifndef _EX_IMAGE_H
#define _EX_IMAGE_H

void         _ex_image_mouse_wheel(Etk_Object *object, void *event, void *data);
void         _ex_image_mouse_down(Etk_Object *object, void *event, void *data);
void         _ex_image_mouse_up(Etk_Object *object, void *event, void *data);
void         _ex_image_mouse_move(Etk_Object *object, void *event, void *data);
void         _ex_image_flip_horizontal(Etk_Image *im);
void         _ex_image_flip_vertical(Etk_Image *im);
void         _ex_image_flip_diagonal(Etk_Image *im, int direction);    
void         _ex_image_blur(Etk_Image *im);
void         _ex_image_sharpen(Etk_Image *im);
void         _ex_image_save(Etk_Image *im);
void         _ex_image_zoom(Etk_Image *im, int zoom); 
void         _ex_image_brightness(Etk_Image *im, int brightness);
void         _ex_image_wallpaper_set(Etk_Image *im);

#endif
