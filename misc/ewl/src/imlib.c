#include "imlib.h"

/******************************/
/* IMAGE ALLOCATION FUNCTIONS */
/******************************/
Imlib_Image ewl_imlib_load_image(char *file)
{
	Imlib_Load_Error err = 0;
	Imlib_Image      im = NULL;
	FUNC_BGN("ewl_imlib_load_image");
	if (!file)	{
		ewl_debug("ewl_imlib_load_image", EWL_NULL_ERROR, "file");
	} else {
		im = imlib_load_image_with_error_return(file, &err);
		if (err)	{
			ewl_debug("ewl_imlib_load_image", EWL_GENERIC_ERROR,
			          ewl_imlib_get_error_string(file, err));
		}
	}
	FUNC_END("ewl_imlib_load_image");
	return im;
}

Imlib_Image ewl_imlib_clone_image(Imlib_Image im)
{
	Imlib_Image rim = NULL;
	FUNC_BGN("ewl_imlib_clone_image");
	if (!im)	{
		ewl_debug("ewl_imlib_clone_image", EWL_NULL_ERROR, "im");
	} else {
		imlib_context_set_image(im);
		rim = imlib_clone_image();
	}
	FUNC_END("ewl_imlib_clone_image");
	return rim;
}

void        ewl_imlib_save_image(Imlib_Image im, char *file)
{
	FUNC_BGN("ewl_imlib_save_image");
	if (!im)	{
		ewl_debug("ewl_imlib_save_image", EWL_NULL_ERROR, "im");
	} else if (!file) {
		ewl_debug("ewl_imlib_save_image", EWL_NULL_ERROR, "file");
	} else {
		imlib_context_set_image(im);
		imlib_save_image(file);
	}
	FUNC_END("ewl_imlib_save_image");
	return;
}

void        ewl_imlib_free_image(Imlib_Image im)
{
	FUNC_BGN("ewl_imlib_free_image");
	if (!im)	{
		ewl_debug("ewl_imlib_free_image", EWL_NULL_ERROR, "im");
	} else {
		imlib_context_set_image(im);
		imlib_free_image();
	}
	FUNC_END("ewl_imlib_free_image");
	return;
}


/*****************************/
/* IMAGE ATTRIBUTE FUNCTIONS */
/*****************************/
int         ewl_imlib_image_get_width(Imlib_Image im)
{
	int w = 0;
	FUNC_BGN("ewl_imlib_image_get_width");
	if (!im) {
		ewl_debug("ewl_imlib_image_get_width", EWL_NULL_ERROR, "im");
	} else {
		imlib_context_set_image(im);
		w = imlib_image_get_width(); 
	}
	FUNC_END("ewl_imlib_image_get_width");
	return w;
}

int         ewl_imlib_image_get_height(Imlib_Image im)
{
	int h = 0;
	FUNC_BGN("ewl_imlib_image_get_height");
	if (!im)	{
		ewl_debug("ewl_imlib_image_get_height", EWL_NULL_ERROR, "im");
	} else {
		imlib_context_set_image(im);
		h = imlib_image_get_height(); 
	}
	FUNC_END("ewl_imlib_image_get_height");
	return h;
}

char       *ewl_imlib_image_format(Imlib_Image im)
{
	char *fmt = NULL;
	FUNC_BGN("ewl_imlib_image_format");
	if (!im)	{
		ewl_debug("ewl_imlib_image_format", EWL_NULL_ERROR, "im");
	} else {
		imlib_context_set_image(im);
		fmt = imlib_image_format();
	}
	FUNC_END("ewl_imlib_image_format");
	return fmt;
}

char        ewl_imlib_image_has_alpha(Imlib_Image im)
{
	char r = FALSE;
	FUNC_BGN("ewl_imlib_image_has_alpha");
	if (!im) {
		ewl_debug("ewl_imlib_image_has_alpha", EWL_NULL_ERROR, "im");
	} else {
		imlib_context_set_image(im);
		r = imlib_image_has_alpha();
	}
	FUNC_END("ewl_imlib_image_has_alpha");
	return r;
}

void        ewl_imlib_image_set_has_alpha(Imlib_Image im, int alpha)
{
	FUNC_BGN("ewl_imlib_image_set_has_alpha");
	if (!im) {
		ewl_debug("ewl_imlib_image_set_has_alpha", EWL_NULL_ERROR, "im");
	} else {
		imlib_context_set_image(im);
		imlib_image_set_has_alpha(alpha);
	}
	FUNC_END("ewl_imlib_image_set_has_alpha");
	return;
}

void        ewl_imlib_image_tile(Imlib_Image im)
{
	FUNC_BGN("ewl_imlib_image_tile");
	if (!im) {
		ewl_debug("ewl_imlib_image_tile", EWL_NULL_ERROR, "im");
	} else {
		imlib_context_set_image(im);
		imlib_image_tile();
	}
	FUNC_END("ewl_imlib_image_tile");
	return;
}

void        ewl_imlib_image_blur(Imlib_Image im, int radius)
{
	FUNC_BGN("ewl_imlib_image_blur");
	if (!im) {
		ewl_debug("ewl_imlib_image_blur", EWL_NULL_ERROR, "im");
	} else {
		imlib_context_set_image(im);
		imlib_image_blur(radius);
	}
	FUNC_END("ewl_imlib_image_blur");
	return;
}

void        ewl_imlib_image_sharpen(Imlib_Image im, int radius)
{
	FUNC_BGN("ewl_imlib_image_sharpen");
	if (!im) {
		ewl_debug("ewl_imlib_image_sharpen", EWL_NULL_ERROR, "im");
	} else {
		imlib_context_set_image(im);
		imlib_image_sharpen(radius);
	}
	FUNC_END("ewl_imlib_image_sharpen");
	return;
}


/*****************************/
/* IMAGE DRAWING FUNCTIONS   */
/*****************************/
void        ewl_imlib_render_image_on_drawable(
                                          Drawable     d,
                                          Imlib_Image  im,
                                          int          x,
                                          int          y,
                                          double       angle,
                                          char         dither,
                                          char         blend,
                                          char         alias)
{
	Imlib_Image new_im = im;
	FUNC_BGN("ewl_imlib_render_image_on_drawable");

	imlib_context_set_image(im);
	imlib_context_set_anti_alias(alias);
	imlib_context_set_dither(dither);
	imlib_context_set_blend(blend);
	imlib_context_set_drawable(d);
	if (!BOUNDS(angle,0.0,_EWL_IMLIB_DELTA)) {
		imlib_context_set_angle(angle);
		new_im = imlib_create_rotated_image(angle);
		imlib_context_set_image(new_im);
	}
	imlib_render_image_on_drawable(x, y);
	if (!BOUNDS(angle,0.0,_EWL_IMLIB_DELTA))
		imlib_free_image();

	FUNC_END("ewl_imlib_render_image_on_drawable");
	return;
}

void        ewl_imlib_render_image_on_drawable_at_size(
                                          Drawable     d,
                                          Imlib_Image  im,
                                          int          x,
                                          int          y,
                                          int          w,
                                          int          h,
                                          double       angle,
                                          char         dither,
                                          char         blend,
                                          char         alias)
{
	Imlib_Image new_im = im;
	FUNC_BGN("ewl_imlib_render_image_on_drawable_at_size");

	imlib_context_set_image(im);
	imlib_context_set_drawable(d);
	imlib_context_set_anti_alias(alias);
	imlib_context_set_dither(dither);
	imlib_context_set_blend(blend);
	if (!BOUNDS(angle,0.0,_EWL_IMLIB_DELTA))	{
		imlib_context_set_angle(angle);
		new_im = imlib_create_rotated_image(angle);
		imlib_context_set_image(new_im);
	}
	imlib_render_image_on_drawable_at_size(x, y, w, h);
	if (!BOUNDS(angle,0.0,_EWL_IMLIB_DELTA))
		imlib_free_image_and_decache();

	FUNC_END("ewl_imlib_render_image_on_drawable_at_size");
	return;
}

void        ewl_imlib_render_image_part_on_drawable_at_size(
                                          Drawable     d,
                                          Imlib_Image  im,
                                          int          sx,
                                          int          sy,
                                          int          sw,
                                          int          sh,
                                          int          dx,
                                          int          dy,
                                          int          dw,
                                          int          dh,
                                          double       angle,
                                          char         dither,
                                          char         blend,
                                          char         alias)
{
	Imlib_Image new_im;
	FUNC_BGN("ewl_imlib_render_image_part_on_drawable");

	imlib_context_set_image(im);
	imlib_context_set_drawable(d);
	imlib_context_set_anti_alias(alias);
	imlib_context_set_dither(dither);
	imlib_context_set_blend(blend);
	if (!BOUNDS(angle,0.0,_EWL_IMLIB_DELTA))	{
		imlib_context_set_angle(angle);
		new_im = imlib_create_rotated_image(angle);
		imlib_context_set_image(new_im);
	}
	imlib_render_image_part_on_drawable_at_size(sx, sy, sw, sh, dx, dy, dw,
	                                            dh);
	if (!BOUNDS(angle,0.0,_EWL_IMLIB_DELTA))
		imlib_free_image_and_decache();

	FUNC_END("ewl_imlib_render_image_part_on_drawable");
	return;
}

void        ewl_imlib_blend_image_onto_image(
                                          Imlib_Image  dest_image,
                                          Imlib_Image  source_image,
                                          char         merge_alpha,
                                          int          sx,
                                          int          sy,
                                          int          sw,
                                          int          sh,
                                          int          dx,
                                          int          dy,
                                          int          dw,
                                          int          dh,
                                          double       angle,
                                          char         dither,
                                          char         blend,
                                          char         alias)
{
	FUNC_BGN("ewl_imlib_blend_image_onto_image");
	if (!source_image) {
		ewl_debug("ewl_imlib_blend_image_onto_image", EWL_NULL_ERROR,
		          "source_image");
	} else if (!dest_image) {
		ewl_debug("ewl_imlib_blend_image_onto_image", EWL_NULL_ERROR,
		          "dest_image");
	} else {
		imlib_context_set_image(dest_image);
		imlib_context_set_anti_alias(alias);
		imlib_context_set_dither(dither);
		imlib_context_set_blend(blend);
		if (!BOUNDS(angle,0.0,_EWL_IMLIB_DELTA))	{
			imlib_context_set_angle(angle);
			imlib_blend_image_onto_image_at_angle(source_image,
			                                      merge_alpha, sx, sy,
			                                      sw, sh, dx, dy, (int) angle,
			                                      (int) angle);
		} else {
			imlib_blend_image_onto_image(source_image, merge_alpha, sx, sy,
			                             sw, sh, dx, dy, sw, sh);
		}
	}
	FUNC_END("ewl_imlib_blend_image_onto_image");
	return;
	/*dw = 0;*/
	/*dh = 0;*/
}

Imlib_Image ewl_imlib_create_cropped_scaled_image(
                                          Imlib_Image  im,
                                          int          sx, 
                                          int          sy,
                                          int          sw,
                                          int          sh,
                                          int          dw,
                                          int          dh,
                                          char         alias)
{
	Imlib_Image rim = NULL;
	FUNC_BGN("ewl_imlib_create_cropped_scaled_image");
	if (!im)	{
		ewl_debug("ewl_imlib_greate_cropped_scaled_image", EWL_NULL_ERROR,
		          "im");
	} else {
		imlib_context_set_image(im);
		imlib_context_set_anti_alias(alias);
		rim = imlib_create_cropped_scaled_image(sx, sy, sw, sh, dw, dh);
	}
	FUNC_END("ewl_imlib_create_cropped_scaled_image");
	return rim;
}

Imlib_Image ewl_imlib_create_rotated_image(Imlib_Image im,
                                           double angle)
{
	FUNC_BGN("ewl_imlib_create_rotated_image");
	imlib_context_set_image(im);
	FUNC_END("ewl_imlib_create_rotated_image");
	return (imlib_create_rotated_image(angle));
}


/*****************************/
/* POLYGON DRAWING FUNCTIONS */
/*****************************/
void        ewl_imlib_image_draw_line(Imlib_Image im,
                                      int x1, int y1, int x2, int y2,
                                      char make_updates,
                                      int r, int g, int b, int a)
{
	FUNC_BGN("ewl_imlib_image_draw_line");
	imlib_context_set_image(im);
	imlib_context_set_color(r, g, b, a);
	imlib_image_draw_line(x1, y1, x2, y2, make_updates);
	FUNC_END("ewl_imlib_image_draw_line");
	return;
}

void        ewl_imlib_image_draw_rectangle(Imlib_Image  im,
                                           int x, int y,
                                           int w, int h,
                                           int r, int g, int b, int a)
{
	FUNC_BGN("ewl_imlib_image_draw_rectangle");
	imlib_context_set_image(im);
	imlib_context_set_color(r, g, b, a);
	imlib_image_draw_rectangle(x, y, w, h);
	FUNC_END("ewl_imlib_image_draw_rectangle");
	return;
}

void        ewl_imlib_image_fill_rectangle(Imlib_Image  im,
                                           int x, int y,
                                           int w, int h,
                                           int r, int g, int b, int a)
{
	FUNC_BGN("ewl_imlib_image_fill_rectangle");
	imlib_context_set_image(im);
	imlib_context_set_color(r, g, b, a);
	imlib_image_fill_rectangle(x, y, w, h);
	FUNC_END("ewl_imlib_image_fill_rectangle");
	return;
}


/*****************************************/
/* MISCELLANEOUS IMAGE/POLYGON FUNCTIONS */
/*****************************************/
void        ewl_imlib_apply_color_modifier_to_rectangle(
                                           Imlib_Image im,
                                           int x, int y, int w, int h,
                                           DATA8 * rtab, DATA8 * gtab,
                                           DATA8 * btab, DATA8 * atab)
{
	Imlib_Color_Modifier cm;
	FUNC_BGN("ewl_imlib_apply_color_modifier_to_rectangle");

	imlib_context_set_image(im);
	cm = imlib_create_color_modifier();
	imlib_context_set_color_modifier(cm);
	imlib_set_color_modifier_tables(rtab, gtab, btab, atab);
	imlib_apply_color_modifier_to_rectangle(x, y, w, h);
	imlib_free_color_modifier();
	FUNC_END("ewl_imlib_apply_color_modifier_to_rectangle");
	return;
}


/******************/
/* TEXT FUNCTIONS */
/******************/
void        ewl_imlib_text_draw(Imlib_Image           im,
                                Imlib_Font            fn,
                                int x, int y, char *text,
                                Imlib_Text_Direction  dir, 
                                int r, int g, int b, int a)
{
	FUNC_BGN("ewl_imlib_text_draw");
	imlib_context_set_image(im);
	imlib_context_set_color(r, g, b, a);
	imlib_context_set_font(fn);
	imlib_context_set_direction(dir);
	imlib_text_draw(x, y, text);
	FUNC_END("ewl_imlib_text_draw");
	return;
}

void        ewl_imlib_get_text_size(Imlib_Font fn, char *text,
                             int *w, int *h,
                             Imlib_Text_Direction dir)
{
	FUNC_BGN("ewl_imlib_get_text_size");
	imlib_context_set_font(fn);
	imlib_context_set_direction(dir);
	imlib_get_text_size(text, w, h);
	FUNC_END("ewl_imlib_get_text_size");
	return;
}

void        ewl_imlib_free_font(Imlib_Font fn)
{
	FUNC_BGN("ewl_imlib_free_font");
	imlib_context_set_font(fn);
	imlib_free_font();
	FUNC_END("ewl_imlib_free_font");
	return;
}


/******************/
/* MISC FUNCTIONS */
/******************/
char       *ewl_imlib_get_error_string(char *path, Imlib_Load_Error err)
{
	char *temp = malloc(1023);
	if (!temp)	{
		ewl_debug("ewl_imlib_get_error_string", EWL_NULL_ERROR, "temp");
		FUNC_END("ewl_imlib_get_error_string");
		return NULL;
	}
	FUNC_BGN("ewl_imlib_get_error_string");
	switch (err)	{
	case IMLIB_LOAD_ERROR_FILE_DOES_NOT_EXIST:
		snprintf(temp, 1023,"Imlib Error: File does not exist: %s", path);
		break;
	case IMLIB_LOAD_ERROR_FILE_IS_DIRECTORY:
		snprintf(temp, 1023,"Imlib Error: Directory specified for image filename: %s",
					path);
		break;
	case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_READ:
		snprintf(temp, 1023,"Imlib Error: No read access to directory: %s", path);
		break;
	case IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT:
		snprintf(temp, 1023,"Imlib Error: No Imlib2 loader for that file format: %s", path);
		break;
	case IMLIB_LOAD_ERROR_PATH_TOO_LONG:
		snprintf(temp, 1023,"Imlib Error: Path specified is too long: %s", path);
		break;
	case IMLIB_LOAD_ERROR_PATH_COMPONENT_NON_EXISTANT:
		snprintf(temp, 1023,"Imlib Error: Path component does not exist: %s", path);
		break;
	case IMLIB_LOAD_ERROR_PATH_COMPONENT_NOT_DIRECTORY:
		snprintf(temp, 1023,"Imlib Error: Path component is not a directory: %s", path);
		break;
	case IMLIB_LOAD_ERROR_PATH_POINTS_OUTSIDE_ADDRESS_SPACE:
		snprintf(temp, 1023,"Imlib Error: Path points outside address space: %s", path);
		break;
	case IMLIB_LOAD_ERROR_TOO_MANY_SYMBOLIC_LINKS:
		snprintf(temp, 1023,"Imlib Error: Too many levels of symbolic links: %s", path);
		break;
	case IMLIB_LOAD_ERROR_OUT_OF_MEMORY:
		snprintf(temp, 1023, "Imlib Error: Out of memory: While loading %s", path);
		break;
	case IMLIB_LOAD_ERROR_OUT_OF_FILE_DESCRIPTORS:
		snprintf(temp, 1023, "Imlib Error: Out of file descriptors: While loading %s", path);
		break;
	case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_WRITE:
		snprintf(temp, 1023,"Imlib Error: Cannot write to directory: %s", path);
		break;
	case IMLIB_LOAD_ERROR_OUT_OF_DISK_SPACE:
		snprintf(temp, 1023,"Imlib Error: out of disk space: %s - Cannot write", path);
		break;
	case IMLIB_LOAD_ERROR_UNKNOWN:
	default:
		snprintf(temp, 1023,"Imlib Error: Unknown error. Attempting to continue: %s", path);
		break;
	}
	FUNC_END("ewl_imlib_get_error_string");
	return temp;
}
