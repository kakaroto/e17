/* --------------------------------------
 * Retina - Evas powered image viewer
 * (C) 2000, Joshua Deere
 * dphase@dphase.net
 * --------------------------------------
 * See COPYING for license information
 * --------------------------------------
 */

#include "retina.h"

extern Evas_Object e_area, e_br_bg;
extern Evas_Object e_scr_t, e_scr_b1, e_scr_b2, e_scr_s;
extern Evas_Object e_t_img[500], e_t_txt[500], e_t_txt2[500];
extern Evas_Object e_t_shd;
extern E_DB_File *db;
extern GtkWidget *window;
extern char *r_images[500];
extern int br_status;
extern int t_y, b_y;
extern int cur_sel;
extern int img_c;
extern pid_t scanner_pid;
extern guint current_idle;

void r_browser_dismiss()
{
}

void
r_browser_init()
{
	int w, h;
	b_y = 37;
	cur_sel = 0;

	evas_font_add_path(e_area, "../fonts");
	
	e_br_bg = evas_add_image_from_file(e_area, "../img/browser_bg.png");
	evas_set_layer(e_area, e_br_bg, 1);
	evas_get_image_size(e_area, e_br_bg, &w, &h);
	evas_set_image_fill(e_area, e_br_bg, 0, 0, w, h);
	evas_move(e_area, e_br_bg, 0, 0);
	evas_resize(e_area, e_br_bg, 9999, 9999);

	/* setup scrollbar images */
	e_scr_t = evas_add_image_from_file(e_area, "../img/scrollbar_t.png");
	evas_set_image_border(e_area, e_scr_t, 3, 3, 3, 3);
	evas_set_layer(e_area, e_scr_t, 3);
	
	e_scr_s = evas_add_image_from_file(e_area, "../img/scrollbar_s.png");
	evas_set_image_border(e_area, e_scr_s, 3, 3, 3, 3);
	evas_set_layer(e_area, e_scr_s, 3);

	e_scr_b1 = evas_add_image_from_file(e_area, "../img/scrollbar_btn_up.png");
	evas_set_layer(e_area, e_scr_b1, 3);
	evas_resize(e_area, e_scr_b1, 14, 14);

	e_scr_b2 = evas_add_image_from_file(e_area, "../img/scrollbar_btn_down.png");
	evas_set_layer(e_area, e_scr_b2, 3);
	evas_resize(e_area, e_scr_b2, 14, 14);

	e_t_shd = evas_add_image_from_file(e_area, "../img/selector.png");
	evas_set_image_border(e_area, e_t_shd, 3, 3, 3, 3);
	evas_set_layer(e_area, e_t_shd, 2);
	evas_move(e_area, e_t_shd, 3, b_y);

	r_scrollbar_render();
}

void
r_scrollbar_render()
{
	int w, h;
	w = window->allocation.width;
	h = window->allocation.height;

	evas_set_image_fill(e_area, e_scr_t, 0, 0, 20, h);
	evas_resize(e_area, e_scr_t, 20, h);
	evas_move(e_area, e_scr_t, w-20, 0);

	evas_set_image_fill(e_area, e_scr_s, 0, 0, 14, h-34);
	evas_resize(e_area, e_scr_s, 14, h-34);
	evas_move(e_area, e_scr_s, w-17, 3);

	evas_move(e_area, e_scr_b1, w-17, h-31);
	evas_move(e_area, e_scr_b2, w-17, h-17);

	evas_set_image_fill(e_area, e_t_shd, 0, 0, w-27, 38);
	evas_resize(e_area, e_t_shd, w-27, 38);
}

void
r_browser_add(char *img)
{
}

void
r_add_text(char *txt, int x, int y)
{
}

void
r_gen_thumb(char *file)
{
	Imlib_Image im;
	char save[4096]; /* , key[4096]; */
	int mod_time, old_mod_time;
	int test = 0; /* , ok */

	mod_time = e_file_modified_time(file);
	if(test == 0) old_mod_time = 0;
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
				ww = (w  * 32) / h;
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
				} while(err != IMLIB_LOAD_ERROR_NONE);
				imlib_free_image();
			}
			imlib_context_set_image(im);
			imlib_free_image();
		}
	}
}

void
r_draw_thumb()
{
	char t[255];
	int i, w, h;
	
	for(i = 0; i <= img_c-1; i++){
		sprintf(t, "%s/.retina.db:/image/%s", getenv("HOME"), r_images[i]);
		e_t_img[i] = evas_add_image_from_file(e_area, t);
		evas_get_image_size(e_area, e_t_img[i], &w, &h);
		evas_resize(e_area, e_t_img[i], w, h);
		evas_move(e_area, e_t_img[i], 10, t_y);
		evas_set_layer(e_area, e_t_img[i], 100);
		//evas_show(e_area, e_t_img[i]);

		e_t_txt2[i] = evas_add_text(e_area, "arial", 8, r_images[i]);
		evas_set_layer(e_area, e_t_txt2[i], 100);
    evas_set_color(e_area, e_t_txt2[i], 0, 0, 0, 255);
    evas_move(e_area, e_t_txt2[i], 50+1, t_y+10+1);
    //evas_show(e_area, e_t_txt2[i]);

		e_t_txt[i] = evas_add_text(e_area, "arial", 8, r_images[i]);
    evas_set_layer(e_area, e_t_txt[i], 100);
    evas_set_color(e_area, e_t_txt[i], 255, 255, 255, 255);
    evas_move(e_area, e_t_txt[i], 50, t_y+10);
		//evas_show(e_area, e_t_txt[i]);
										
		t_y += 45;
	}
	QUEUE_DRAW;
}

void
r_browser_move_down()
{
	if((b_y+(43*2)) <= t_y){
		b_y += 45;
		cur_sel++;
		
		r_evas_load(r_images[cur_sel]);

		evas_set_color(e_area, e_t_txt[cur_sel], 255, 200, 0, 255);
		if(cur_sel > 0){
			evas_set_color(e_area, e_t_txt[cur_sel-1], 255, 255, 255, 255);
		}
		evas_move(e_area, e_t_shd, 3, b_y);
	}
	QUEUE_DRAW;
}

void
r_browser_move_up()
{
	if(b_y >= 40){
		b_y -= 45;
		cur_sel--;

		r_evas_load(r_images[cur_sel]);
		
		evas_set_color(e_area, e_t_txt[cur_sel], 255, 200, 0, 255);
		evas_set_color(e_area, e_t_txt[cur_sel+1], 255, 255, 255, 255);
		evas_move(e_area, e_t_shd, 3, b_y);
	}
	QUEUE_DRAW;
}

void
r_browser_show()
{
	int i;
	for(i = 0; i <= img_c-1; i++){
		evas_show(e_area, e_t_shd);
		evas_show(e_area, e_t_img[i]);
		evas_show(e_area, e_t_txt2[i]);
		evas_show(e_area, e_t_txt[i]);
	}
	QUEUE_DRAW;
}

void
r_browser_hide()
{
	int i;
	for(i = 0; i <= img_c-1; i++){
		evas_hide(e_area, e_t_shd);
	  evas_hide(e_area, e_t_img[i]);
	  evas_hide(e_area, e_t_txt2[i]);
	  evas_hide(e_area, e_t_txt[i]);
	}
	QUEUE_DRAW;
}
