#ifndef __IMAGE_H__
#define __IMAGE_H__

void                image_add_from_dnd(char *item);

void                image_create_list(int argc, char **argv);
void                image_create_list_dir(char *dir);
void                image_destroy_list(void);

void                image_create_thumbnails(void);

Image              *e_image_new(char *file);
void                e_image_free(Image * im);
void                image_delete(Image * im);

void                e_rotate_r_current_image(void);
void                e_rotate_l_current_image(void);

void                e_flip_h_current_image(void);
void                e_flip_v_current_image(void);

void                e_delete_current_image(void);
void                e_display_current_image(void);

void                next_image(void *data, Evas * e, Evas_Object * obj,
			       void *event_info);

void                next_image_up(void *data, Evas * e, Evas_Object * obj,
				  void *event_info);
void                next_image_move(void *data, Evas * e, Evas_Object * obj,
				    void *event_info);

#endif /* __IMAGE_H__ */
