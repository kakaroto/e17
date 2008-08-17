#include "retina.h"

int
retina_img_load(char *file)
{
	int w, h;
	int r, g, b;

	/* delete image if one is currently loaded */
	if(retina_img) evas_object_del(retina_img);
	
	retina_img = evas_object_image_add(retina_evas);
	evas_object_image_file_set(retina_img, file, NULL);
	evas_object_move(retina_img, 0, 0);
	evas_object_image_size_get(retina_img, &w, &h);
	evas_object_resize(retina_img, w, h);
	evas_object_image_fill_set(retina_img, 0, 0, w, h);
	evas_object_layer_set(retina_img, 100);

	evas_object_color_get(retina_img, &r, &g, &b, NULL);
	evas_object_color_set(retina_img, r, g, b, 0);
	ecore_timer_add(0.01, retina_img_fade_in, retina_img);
	evas_object_show(retina_img);

	/* resize the window to image size, no smaller than 320x164 */
	if(w < 320 && h < 164)
		ecore_evas_resize(retina_win, 320, 164);
	else if(w < 320 && h >=164)
		ecore_evas_resize(retina_win, 320, h);
	else if(w >= 320 && h < 164)
		ecore_evas_resize(retina_win, w, 164);
	else
		ecore_evas_resize(retina_win, w, h);

	/* set image properties */
	retina_image.filename = file;
	retina_image.orig_w = w;
	retina_image.orig_h = h;
	retina_image.zoom_w = w;
	retina_image.zoom_h = h;

	if(img_count > 0)
		retina_gui_set_title(retina_image.filename);

	retina_cb_win_resize();

	return 0;
}

void
retina_img_load_next()
{
	if(img_count > 1){
		if(cur_sel == (img_count-1)){
			cur_sel = 0;
			retina_img_load(retina_image_list[cur_sel]);
		} else {
			cur_sel++;
			retina_img_load(retina_image_list[cur_sel]);
		}
	}
}

void retina_img_load_back()
{
	if(img_count > 1){
		if(cur_sel == 0){
			cur_sel = (img_count-1);
			retina_img_load(retina_image_list[cur_sel]);
		} else {
			cur_sel--;
			retina_img_load(retina_image_list[cur_sel]);
		}
	}
}

void
retina_img_drawchecks()
{
	/* draw a checkered bg so we can see alpha blended images better */
	int w, h;
	
	retina_bg = evas_object_image_add(retina_evas);
	evas_object_image_file_set(retina_bg, PACKAGE_DATA_DIR "/images/checks.png", NULL);
	evas_object_move(retina_bg, 0, 0);
	evas_object_image_size_get(retina_bg, &w, &h);
	evas_object_image_fill_set(retina_bg, 0, 0, w, h);
	evas_object_resize(retina_bg, 9999, 9999);
	evas_object_layer_set(retina_bg, 0); /* lowest layer */
	evas_object_show(retina_bg);
}

void
retina_img_resize(int w, int h)
{
	evas_object_resize(retina_img, w, h);
	evas_object_image_fill_set(retina_img, 0, 0, w, h);
}

void
retina_img_zoom_in()
{
	retina_image.zoom_w = retina_image.zoom_w * 1.1;
	retina_image.zoom_h = retina_image.zoom_h * 1.1;
	
	retina_img_resize(retina_image.zoom_w, retina_image.zoom_h);

	retina_cb_win_resize();
}

void
retina_img_zoom_out()
{
	retina_image.zoom_w = retina_image.zoom_w / 1.1;
	retina_image.zoom_h = retina_image.zoom_h / 1.1;

	retina_img_resize(retina_image.zoom_w, retina_image.zoom_h);

	retina_cb_win_resize();
}

void
retina_img_zoom_set()
{
	retina_img_resize(retina_image.orig_w, retina_image.orig_h);
	retina_image.zoom_w = retina_image.orig_w;
	retina_image.zoom_h = retina_image.orig_h;
	retina_cb_win_resize();
}

void
retina_img_show_help()
{
	if(diag_state == 0){
		retina_img_diag_render();
		help_state = 1;
		retina_cb_win_resize();
		evas_object_show(retina_help_img);
	} else {
		diag_state = 0;
		evas_object_hide(retina_diag_bg);
		evas_object_hide(retina_help_img);
	}
}

void
retina_img_show_image_info()
{
}

void
retina_img_diag_render()
{
	int w, h;

	diag_state = 1;

	ecore_evas_geometry_get(retina_win, NULL, NULL, &w, &h);
	evas_object_resize(retina_diag_bg, w, h);
	evas_object_show(retina_diag_bg);
}

int
retina_img_fade_in(void *data)
{
	/* fade the image in on load, kinda funky, oh well */
	Evas_Object *obj = data;
  int r, g, b, a;

	evas_object_color_get(obj, &r, &g, &b, &a);
	if(a >= 250){
		evas_object_color_set(obj, r, g, b, 255);
		return FALSE;
	}

	a = a + 40;
	evas_object_color_set(obj, r, g, b, a);

	return 1;
}
