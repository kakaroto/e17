#ifndef __IMLIB_API_H
#define __IMLIB_API_H 1

#ifndef DATA64
# define DATA64 unsigned long long
# define DATA32 unsigned int
# define DATA16 unsigned short
# define DATA8  unsigned char
#endif

/* data types - guess what - no transparent datatypes - all hidden */
typedef void * Imlib_Image;
typedef void * Imlib_Color_Modifier;
typedef struct _imlib_border Imlib_Border;

struct _imlib_border
{
   int left, right, top, bottom;
};

typedef void (*Imlib_Progress_Function)(Imlib_Image *im, char percent,
					int update_x, int update_y,
					int update_w, int update_h);

/* init and setup functions */
int         imlib_get_cache_size(void);
void        imlib_set_cache_size(int bytes);
int         imlib_get_color_usage(void);
void        imlib_set_color_usage(int max);

/* image loading functions */
Imlib_Image imlib_load_image(char *file);
Imlib_Image imlib_load_image_with_progress_callback(char *file,
						    Imlib_Progress_Function progress_function,
						    char progress_granulatiy);
Imlib_Image imlib_load_image_immediately(char *file);
Imlib_Image imlib_load_image_without_cache(char *file);
Imlib_Image imlib_load_image_with_progress_callback_without_cache (char *file,
								   Imlib_Progress_Function progress_function,
								   char progress_granulatiy);
Imlib_Image imlib_load_image_immediately_without_cache(char *file);

/* image destruction functions */
void imlib_free_image(Imlib_Image image);
void imlib_free_image_and_decache(Imlib_Image image);

/* image information retrieval and basic manipulation functions */
int     imlib_image_get_width(Imlib_Image image);
int     imlib_image_get_height(Imlib_Image image);
DATA32 *imlib_image_get_data(Imlib_Image image);
char    imlib_image_has_alpha(Imlib_Image image);
char   *imlib_image_format(Imlib_Image image);
void    imlib_image_put_back_data(Imlib_Image image);
void    imlib_image_set_has_alpha(Imlib_Image image, char has_alpha);
void    imlib_image_set_never_changes_on_disk(Imlib_Image image);
void    imlib_image_get_border(Imlib_Image image, Imlib_Border *border);
void    imlib_image_set_border(Imlib_Image image, Imlib_Border *border);
void    imlib_image_set_format(Imlib_Image image, char *format);

/* image drawing/rendering functions */

void imlib_render_pixmaps_for_whole_image(Imlib_Image image, Display *display,
					  Drawable drawable, Visual *visual,
					  Colormap colormap, int depth,
					  Pixmap *pixmap_return,
					  Pixmap *mask_return,
					  char dithered_rendering,
					  char create_dithered_mask,
					  Imlib_Color_Modifier color_modifier);
void imlib_render_pixmaps_for_whole_image_at_size(Imlib_Image image, Display *display,
						  Drawable drawable, Visual *visual,
						  Colormap colormap, int depth,
						  Pixmap *pixmap_return,
						  Pixmap *mask_return,
						  char anti_aliased_scaling,
						  char dithered_rendering,
						  char create_dithered_mask,
						  int width, int height,
						  Imlib_Color_Modifier color_modifier);
void imlib_render_image_on_drawable(Imlib_Image image, Display *display,
				    Drawable drawable, Visual *visual,
				    Colormap colormap, int depth,
				    char dithered_rendering,
				    char alpha_blending,
				    int x, int y,
				    Imlib_Color_Modifier color_modifier);
void imlib_render_image_on_drawable_at_size(Imlib_Image image, Display *display,
					    Drawable drawable, Visual *visual,
					    Colormap colormap, int depth,
					    char anti_aliased_scaling,
					    char dithered_rendering,
					    char alpha_blending,
					    int x, int y, int width, int height,
					    Imlib_Color_Modifier color_modifier);
void imlib_render_image_part_on_drawable_at_size(Imlib_Image image, Display *display,
						 Drawable drawable, Visual *visual,
						 Colormap colormap, int depth,
						 char anti_aliased_scaling,
						 char dithered_rendering,
						 char alpha_blending,
						 int source_x, int source_y,
						 int source_width, int source_height,
						 int x, int y, int width, int height,
						 Imlib_Color_Modifier color_modifier);
/* rgba space ops */
void imlib_blend_image_onto_image(Imlib_Image source_image,
				  Imlib_Image destination_image,
				  int source_x, int source_y, 
				  int source_width, int source_height,
				  int destination_x, int destination_y,
				  int destination_width, int destination_height);

#if 0

/* FIXME: */
/* draw line, polygon, rect - with option of drawing in rgb or alpha or both */
/* apply alpha of one image to another */
void imlib_image_updates_reset(Imlib_Image image);
void imlib_image_updates_flush_to_drawable(Imlib_Image image, Display *display,
					   Drawable drawable, Visual *visual,
					   Colormap colormap, int depth,
					   char dithered_rendering,
					   char alpha_blending,
					   int x, int y, 
					   int width, int height,
					   Imlib_Color_Modifier color_modifier);
void imlibimage_updates_add(Imlib_Image image, int x, int y, 
			    int width, int height);

#endif

/* image creation and grabbing */
Imlib_Image imlib_create_image(int width, int height);
Imlib_Image imlib_create_image_using_data(int width, int height, 
					  DATA32 *data);
Imlib_Image imlib_create_image_using_copied_data(int width, int height, 
						 DATA32 *data);
Imlib_Image imlib_create_image_from_drawable(Display *display, 
					     Drawable drawable, 
					     Pixmap mask, Visual *visual, 
					     Colormap colormap, int depth,
					     int x, int y, 
					     int width, int height,
					     char need_to_grab_x);
Imlib_Image imlib_clone_image(Imlib_Image image);
char imlib_copy_drawable_to_image(Imlib_Image image, Display *display,
				  Drawable drawable,
				  Pixmap mask, Visual *visual,
				  Colormap colormap, int depth,
				  int x, int y,
				  int width, int height,
				  int destination_x, int destination_y,
				  char need_to_grab_x);

#if 0
Imlib_image imlib_create_cropped_image(Imlib_Image image,
				       int x, int y, int width, int height);
Imlib_image imlib_create_cropped_scaled_image(Imlib_Image image,
					      int source_x, int source_y, 
					      int source_width, 
					      int source_height,
					      int destination_width,
					      int destination_height);
Imlib_image imlib_create_image_from_xpm_data(unsigned char *data);

/* color stuff */
int imlib_match_color(int red, int green, int blue);

/* image modification - geometry */
void imlib_create_cropped_image(Imlib_Image image, int x, int y, int width, 
				int height);
void imlib_cropp_scale_image(Imlib_Image image, int source_x, int source_y, 
			     int source_width, int source_height);

/* image modification - color */
Imlib_Color_Modifier imlib_create_color_modifier(void);
void imlib_free_color_modifier(Imlib_Color_Modifier color_modifier);
void imlib_set_color_modifier_gamma(Imlib_Color_Modifier color_modifier,
				    double gamma_value);
void imlib_set_color_modifier_brightness(Imlib_Color_Modifier color_modifier,
					 double brightness_value);
void imlib_set_color_modifier_contrast(Imlib_Color_Modifier color_modifier,
				       double contrast_value);
void imlib_set_color_modifier_tables(Imlib_Color_Modifier color_modifier,
				     DATA8 *red_table,
				     DATA8 *green_table,
				     DATA8 *blue_table);
void imlib_apply_color_modifier(Imlib_Image image, 
				Imlib_Color_Modifier color_modifier);

/* image modification - rotation / flipping */
void imlib_image_flip_horizontal(Imlib_Image image);
void imlib_image_flip_vertical(Imlib_Image image);
void imlib_image_flip_diagonal(Imlib_Image image);

/* image modification - blur / sharpen */
void imlib_image_blur(Imlib_Image image, int radius);
void imlib_image_sharpen(Imlib_Image image, int radius);

/* image modification - special (seamless tile (h, v & both) etc) */
void imlib_image_tile_horizontal(Imlib_Image image);
void imlib_image_tile_vertical(Imlib_Image image);
void imlib_image_tile(Imlib_Image image);

/* image saving functions */
void imlib_save_image(Imlib_Image image, char *filename);

/* FIXME: have to figure out generic saving mechanism that lets savers have */
/* options like quality, color , compression etc. */

#endif


#endif
