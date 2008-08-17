/* thumbnail and thumbnail browser code */

#include "retina.h"

time_t _file_mod_time(char *file);

void
retina_thumbnail_browser_create()
{
}

void
retina_thumbnail_browser_show()
{
}

void
retina_thumbnail_generate(char *file)
{
	/* FIXME: this is code from old retina, few things need to be done
	 * here, such as storing modified times into the .retina.db file
	 * and truly comparing them to current mod'd time */
	Imlib_Image im;
	char save[4096];
	int mod_time, old_mod_time = 0;

	mod_time = _file_mod_time(file);
	if(old_mod_time < mod_time){
		im = imlib_load_image_immediately(file);
		if(im){
			Imlib_Image thumb;
			int w, h, ww, hh;

			imlib_context_set_image(im);
			w = imlib_image_get_width();
			h = imlib_image_get_height();
			if(w > h){
				ww = 32;
				hh = (h * 32) / w;
			} else {
				hh = 32;
				ww = (w * 32) / h;
			}
			imlib_context_set_anti_alias(1);
			thumb = imlib_create_cropped_scaled_image(0, 0, w, h, ww, hh);
			if(thumb){
				Imlib_Load_Error err;
				
				imlib_context_set_image(thumb);
				imlib_image_set_format("db");
				sprintf(save, "%s/.retina.db:/image/%s", getenv("HOME"), file);
				do{
					err = IMLIB_LOAD_ERROR_NONE;
					imlib_save_image_with_error_return(save, &err);
				} while (err != IMLIB_LOAD_ERROR_NONE);
				imlib_free_image();
			}
			imlib_context_set_image(im);
			imlib_free_image();
		}
	}
}

void
retina_thumbnail_draw(char *file)
{
}

time_t
_file_mod_time(char *file)
{
	struct stat st;
	if(stat(file, &st) < 0)
		return 0;
	return st.st_mtime;
}
