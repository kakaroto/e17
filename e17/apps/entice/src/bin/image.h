#ifndef __IMAGE_H__
#define __IMAGE_H__

void image_add_from_dnd(char *item);

void image_create_list(int argc, char **argv);
void image_create_list_dir(char *dir);
void image_destroy_list(void);

void image_create_thumbnails(void);

Image *e_image_new(char *file);
void e_image_free(Image *im);
void image_delete(Image *im);

void e_delete_current_image(void);
void e_display_current_image(void);

void next_image(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void next_image_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void next_image_move(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

#endif /* __IMAGE_H__ */
