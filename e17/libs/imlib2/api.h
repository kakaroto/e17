#ifndef __IMLIB_API_H
#define __IMLIB_API_H 1

#ifndef DATA64
#define DATA64 u_int64_t
#define DATA32 u_int32_t
#define DATA16 u_int16_t
#define DATA8  u_int8_t
#endif

/* data types - guess what - no transparent datatypes - all hidden */
typedef void * Imlib_Image;
typedef void * Imlib_Color_Modifier;
typedef void * Imlib_Updates;
typedef void * Imlib_Font;
typedef struct _imlib_border Imlib_Border;
typedef struct _imlib_color Imlib_Color;
typedef struct _imlib_rectangle Imlib_Rectangle;
typedef enum _imlib_operation Imlib_Operation;
typedef enum _imlib_load_error Imlib_Load_Error;
typedef enum _imlib_chanel_mask Imlib_Chanel_Mask;

struct _imlib_border
{
   int left, right, top, bottom;
};

struct _imlib_color
{
   int alpha, red, green, blue;
};

struct _imlib_rectangle
{
   int x, y, width, height;
};

enum _imlib_operation
{
   IMLIB_OP_COPY,
   IMLIB_OP_ADD,
   IMLIB_OP_SUBTRACT,
   IMLIB_OP_RESHADE
};

enum _imlib_chanel_mask
{
   IMLIB_ALPHA = 1,
   IMLIB_RED = 2,
   IMLIB_GREEN = 4,
   IMLIB_BLUE = 8
};

enum _imlib_load_error
{
   IMLIB_LOAD_ERROR_NONE,
   IMLIB_LOAD_ERROR_FILE_DOES_NOT_EXIST,
   IMLIB_LOAD_ERROR_FILE_IS_DIRECTORY,
   IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_READ,
   IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT,
   IMLIB_LOAD_ERROR_PATH_TOO_LONG,
   IMLIB_LOAD_ERROR_PATH_COMPONENT_NON_EXISTANT,
   IMLIB_LOAD_ERROR_PATH_COMPONENT_NOT_DIRECTORY,
   IMLIB_LOAD_ERROR_PATH_POINTS_OUTSIDE_ADDRESS_SPACE,
   IMLIB_LOAD_ERROR_TOO_MANY_SYMBOLIC_LINKS,
   IMLIB_LOAD_ERROR_OUT_OF_MEMORY,
   IMLIB_LOAD_ERROR_OUT_OF_FILE_DESCRIPTORS,
   IMLIB_LOAD_ERROR_UNKNOWN
};

typedef void (*Imlib_Progress_Function)(Imlib_Image *im, char percent,
					int update_x, int update_y,
					int update_w, int update_h);

/* init and setup functions */
int         imlib_get_cache_size(void);
void        imlib_set_cache_size(int bytes);
int         imlib_get_color_usage(void);
void        imlib_set_color_usage(int max);

int         imlib_get_visual_depth(Display *display, Visual *visual);
Visual     *imlib_get_best_visual(Display *display, int screen, 
				  int *depth_return);

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
Imlib_Image imlib_load_image_with_progress_callback_and_error_return (char *file,
								      Imlib_Progress_Function progress_function,
								      char progress_granulatiy,
								      Imlib_Load_Error *error_return);
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
				    Imlib_Color_Modifier color_modifier,
				    Imlib_Operation operation);
void imlib_render_image_on_drawable_at_size(Imlib_Image image, Display *display,
					    Drawable drawable, Visual *visual,
					    Colormap colormap, int depth,
					    char anti_aliased_scaling,
					    char dithered_rendering,
					    char alpha_blending,
					    int x, int y, int width, int height,
					    Imlib_Color_Modifier color_modifier,
					    Imlib_Operation operation);
void imlib_render_image_part_on_drawable_at_size(Imlib_Image image, Display *display,
						 Drawable drawable, Visual *visual,
						 Colormap colormap, int depth,
						 char anti_aliased_scaling,
						 char dithered_rendering,
						 char alpha_blending,
						 int source_x, int source_y,
						 int source_width, int source_height,
						 int x, int y, int width, int height,
						 Imlib_Color_Modifier color_modifier,
						 Imlib_Operation operation);
/* rgba space ops */
void imlib_blend_image_onto_image(Imlib_Image source_image,
				  Imlib_Image destination_image,
				  char antialias, char blend, char merge_alpha,
				  int source_x, int source_y, 
				  int source_width, int source_height,
				  int destination_x, int destination_y,
				  int destination_width, int destination_height,
				  Imlib_Color_Modifier color_modifier,
				  Imlib_Operation operation);


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
Imlib_Image imlib_create_scaled_image_from_drawable(Display *display, 
						    Drawable drawable, 
						    Pixmap mask, 
						    Visual *visual, 
						    Colormap colormap, 
						    int depth,
						    int source_x, 
						    int source_y, 
						    int source_width, 
						    int source_height,
						    int destination_width, 
						    int destination_height,
						    char need_to_grab_x,
						    char get_mask_from_shape);
Imlib_Image imlib_clone_image(Imlib_Image image);
char imlib_copy_drawable_to_image(Imlib_Image image, Display *display,
				  Drawable drawable,
				  Pixmap mask, Visual *visual,
				  Colormap colormap, int depth,
				  int x, int y,
				  int width, int height,
				  int destination_x, int destination_y,
				  char need_to_grab_x);
Imlib_Image imlib_create_cropped_image(Imlib_Image image,
				       int x, int y, int width, int height);
Imlib_Image imlib_create_cropped_scaled_image(Imlib_Image image,
					      char antialias,
					      int source_x, int source_y, 
					      int source_width, 
					      int source_height,
					      int destination_width,
					      int destination_height);
Imlib_Updates imlib_updates_init(void);
Imlib_Updates imlib_updates_append_updates(Imlib_Updates updates, 
					   Imlib_Updates appended_updates);
Imlib_Updates imlib_update_append_rect(Imlib_Updates updates, 
				       int x, int y, int w, int h);
Imlib_Updates imlib_updates_merge(Imlib_Updates updates, int w, int h);
void imlib_updates_free(Imlib_Updates updates);
Imlib_Updates imlib_updates_get_next(Imlib_Updates updates);
void imlib_updates_get_coordinates(Imlib_Updates updates, 
				   int *x_return, int *y_return,
				   int *width_return, int *height_return);
void imlib_render_image_updates_on_drawable(Imlib_Image image,
					    Imlib_Updates updates,
					    Display *display,
					    Drawable drawable, Visual *visual,
					    Colormap colormap, int depth,
					    char dithered_rendering,
					    int x, int y,
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

#if 0

void imlib_image_copy_alpha_to_image(Imlib_Image image_source,
				     Imlib_Image image_destination,
				     int x, int y);
void imlib_image_scroll_rect(Imlib_Image image, int x, int y, 
			     int width, int height, int delta_x,
			     int delta_y);
void imlib_image_draw_line(Imlib_Image image, int x1, int y1, int x1, int y2,
			   Imlib_Color *color, Imlib_Channel_Mask, 
			   Imlib_Operation operation);
void imlib_image_draw_rectangle(Imlib_Image image, int x, int y, int width,
				int height, Imlib_Color *color, 
				Imlib_Channel_Mask, Imlib_Operation operation);
void imlib_image_fill_rectangle(Imlib_Image image, int x, int y, int width,
				int height, Imlib_Color *color, 
				Imlib_Channel_Mask, Imlib_Operation operation);
void imlib_image_fill_rectangle_gradient(Imlib_Image image, 
					 int x, int y, int width, int height, 
					 Imlib_Color *top_left_color, 
					 Imlib_Color *top_right_color, 
					 Imlib_Color *bottom_left_color, 
					 Imlib_Color *bottom_right_color,
					 Imlib_Channel_Mask mask,
					 Imlib_Operation operation);
/* text functions needed */
Imlib_Font imlib_load_font(char *font_name);
void imlib_font_set_tab_size(Imlib_Font font, int size);
void imlib_free_font(Imlib_Font font);
void imlib_text_draw(Imlib_font font, Imlib_Image image, int x, int y,
		     int width, int height, Imlib_Text_Direction direction, 
		     char *text, Imlib_Color *color, 
		     Imlib_Operation operation, char wrap);
void imlib_get_text_size(Imlib_font font, Imlib_Text_Direction direction, 
			 char *text, int *width_return, 
			 int *height_return);
Imlib_Rectangle *imlib_get_text_in_text_size_and_position(Imlib_font font, 
							  Imlib_Text_Direction direction, 
							  int width, int height,
							  char *text,
							  int text_start_index,
							  int text_length,
							  int *num_rectangles_return);
int imlib_get_character_index_and_location_in_text(Imlib_font font, 
						   Imlib_Text_Direction direction, 
						   int width, int height,
						   char *text, char wrap,
						   int *x_return,
						   int *y_return,
						   int *width_return,
						   int *height_return);

/* FIXME: have to figure out generic saving mechanism that lets savers have */
/* options like quality, color , compression etc. */

/* image saving functions */
void imlib_save_image(Imlib_Image image, char *filename);
#endif

#if 0
/* do this later as none of the color lookup code is in the rendering */
/* backend yet */
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
#endif

#endif
