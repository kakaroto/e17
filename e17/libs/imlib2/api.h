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
typedef struct _imlib_border Imlib_Border;

struct _imlib_border
{
   int left, right, top, bottom;
};

typedef void (*Imlib_Progress_Function)(Imlib_Image *im, char percent,
					int update_x, int update_y,
					int update_w, int update_h);

/* init and setup functions */
char        imlib_init(void);
int         imlib_get_cache_size(void);
void        imlib_set_cache_size(int bytes);
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
int     imlib_get_image_width(Imlib_Image image);
int     imlib_get_image_height(Imlib_Image image);
DATA32 *imlib_get_image_data(Imlib_Image image);
void    imlib_put_back_image_data(Imlib_Image image);
char    imlib_image_has_alpha(Imlib_Image image);
void    imlib_set_image_never_changes_on_disk(Imlib_Image image);
void    imlib_image_get_border(Imlib_Image image, Imlib_Border *border);
void    imlib_image_set_border(Imlib_Image image, Imlib_Border *border);
char   *imlib_image_format(Imlib_Image image);

/* image drawing/rendering functions */

void imlib_render_pixmaps_for_whole_image(Imlib_Image image, Display *display,
					  Drawable drawable, Visual *visual,
					  Colormap colormap, int depth,
					  Pixmap *pixmap_return,
					  Pixmap *mask_return,
					  char anti_aliased_scaling,
					  char dithered_rendering,
					  char create_dithered_mask);
void imlib_render_pixmaps_for_whole_image_at_size(Imlib_Image image, Display *display,
						  Drawable drawable, Visual *visual,
						  Colormap colormap, int depth,
						  Pixmap *pixmap_return,
						  Pixmap *mask_return,
						  char anti_aliased_scaling,
						  char dithered_rendering,
						  char create_dithered_mask,
						  int width, int height);
void imlib_render_image_on_drawable(Imlib_Image image, Display *display,
				    Drawable drawable, Visual *visual,
				    Colormap colormap, int depth,
				    char anti_aliased_scaling,
				    char dithered_rendering,
				    char alpha_blending,
				    int x, int y);
void imlib_render_image_on_drawable_at_size(Imlib_Image image, Display *display,
					    Drawable drawable, Visual *visual,
					    Colormap colormap, int depth,
					    char anti_aliased_scaling,
					    char dithered_rendering,
					    char alpha_blending,
					    int x, int y, int width, int height);
/* rgba space ops */
void imlib_blend_image_onto_image(Imlib_Image source_image,
				  Imlib_Image destination_image,
				  int source_x, int source_y, 
				  int source_width, int source_height,
				  int destination_x, int destination_y,
				  int destination_width, int destination_height);

/* image creation and grabbing */
Imlib_Image imlib_create_image_using_data(int width, int height, 
					  DATA32 *data);
Imlib_Image imlib_create_image_using_copied_data(int width, int height, 
						 DATA32 *data);
Imlib_Image imlib_create_image_from_drawable(Display *display, 
					     Drawable drawable, 
					     Pixmap mask, Visual *visual, 
					     Colormap colormap, int depth,
					     int x, int y, 
					     int width, int height);

Imlib_Image imlib_clone_image(Imlib_Image image);
#endif
