#include "entice.h"

void
e_handle_arrows(void)
{
   if (o_image)
     {
	double              ix, iy, iw, ih;

	evas_object_geometry_get(o_image, &ix, &iy, &iw, &ih);
	if (ix < 0)
	   e_fade_l_arrow_in(NULL);
	else
	   e_fade_l_arrow_out(NULL);
	if (iy < 0)
	   e_fade_t_arrow_in(NULL);
	else
	   e_fade_t_arrow_out(NULL);
	if (iw + ix > win_w)
	   e_fade_r_arrow_in(NULL);
	else
	   e_fade_r_arrow_out(NULL);
	if (ih + iy > win_h)
	   e_fade_b_arrow_in(NULL);
	else
	   e_fade_b_arrow_out(NULL);
     }
   else
     {
	e_fade_l_arrow_out(NULL);
	e_fade_r_arrow_out(NULL);
	e_fade_t_arrow_out(NULL);
	e_fade_b_arrow_out(NULL);
     }
}

void
e_handle_resize(void)
{
   int                 w, h;

   ecore_evas_geometry_get(ecore_evas, NULL, NULL, &win_w, &win_h);
   evas_object_move(o_bg, 0, 0);
   evas_object_resize(o_bg, win_w, win_h);
   evas_object_layer_set(o_bg, 0);

   evas_object_image_size_get(o_logo, &w, &h);
   evas_object_move(o_logo, (win_w - w) / 2, (win_h - h) / 2);
   evas_object_resize(o_logo, w, h);
   evas_object_layer_set(o_logo, 80);

   evas_object_image_size_get(o_panel, &w, NULL);
   if (panel_active == active_out || panel_active == active_force_out)
      evas_object_move(o_panel, -w, 0);
   evas_object_resize(o_panel, w, win_h);
   evas_object_layer_set(o_panel, 240);
   evas_object_layer_set(o_panel_arrow_u, 250);
   evas_object_layer_set(o_panel_arrow_d, 250);

   evas_object_move(o_showpanel, 0, 0);
   evas_object_resize(o_showpanel, 64, win_h);
   evas_object_layer_set(o_showpanel, 300);
   evas_object_repeat_events_set(o_showpanel, 1);
   evas_object_move(o_hidepanel, 128, 0);
   evas_object_resize(o_hidepanel, win_w - 128, win_h);
   evas_object_layer_set(o_hidepanel, 400);
   // make sure buttons aren't left hanging mid-window
   if (buttons_active == active_out || buttons_active == active_force_out) {
      evas_object_move(o_bt_prev, win_w + 0, 0);
      evas_object_move(o_bt_next, win_w + 32, 0);
      evas_object_move(o_bt_zoom_normal, win_w + 64, 0);
      evas_object_move(o_bt_zoom_in, win_w + 96, 0);
      evas_object_move(o_bt_zoom_out, win_w + 128, 0);
      evas_object_move(o_bt_expand, win_w + 160, 0);
      evas_object_move(o_bt_full, win_w + 192, 0);
      evas_object_move(o_bt_delete, win_w + 224, 0);
      evas_object_move(o_bt_close, win_w + 256, 0);
   }
   evas_object_move(o_showbuttons, win_w - 288, 0);
   evas_object_layer_set(o_showbuttons, 1500);
   evas_object_repeat_events_set(o_showbuttons, 1);
   evas_object_layer_set(o_bt_close, 1300);
   evas_object_layer_set(o_bt_delete, 1300);
   evas_object_layer_set(o_bt_expand, 1300);
   evas_object_layer_set(o_bt_full, 1300);
   evas_object_layer_set(o_bt_next, 1300);
   evas_object_layer_set(o_bt_prev, 1300);
   evas_object_layer_set(o_bt_zoom_in, 1300);
   evas_object_layer_set(o_bt_zoom_normal, 1300);
   evas_object_layer_set(o_bt_zoom_out, 1300);

   if (o_image)
     {
	int                 sx, sy, mx, my, pw, ph;

	sx = scroll_x;
	sy = scroll_y;
	evas_object_image_size_get(o_image, &w, &h);
	pw = w;
	ph = h;
	w = (int)((double)w / scale);
	h = (int)((double)h / scale);
	if (w > win_w)
	  {
	     if (sx > ((w - win_w) / 2))
		sx = ((w - win_w) / 2);
	     if (sx < -((w - win_w + 1) / 2))
		sx = -((w - win_w + 1) / 2);
	  }
	else
	   sx = 0;
	if (h > win_h)
	  {
	     if (sy > ((h - win_h) / 2))
		sy = ((h - win_h) / 2);
	     if (sy < -((h - win_h + 1) / 2))
		sy = -((h - win_h + 1) / 2);
	  }
	else
	   sy = 0;

	evas_object_move(o_image,
			 sx + ((win_w - w) / 2), sy + ((win_h - h) / 2));
	evas_object_image_fill_set(o_image, 0, 0, w, h);
	evas_object_resize(o_image, w, h);
	evas_object_layer_set(o_image, 100);

	scroll_sx = sx;
	scroll_sy = sy;

	if ((win_w <= w) && (win_h <= h) &&
	    (!evas_object_image_alpha_get(o_image)))
	   evas_object_hide(o_bg);
	else
	   evas_object_show(o_bg);
	evas_object_hide(o_logo);

	if (o_trash)
	  {
	     int                 dw, dh, r;

	     dh = (win_h / 4);
	     dw = (win_w / 4);
	     r = (dh + dw) / 2;
	     evas_object_image_fill_set(o_trash, 0, 0, r, r);
	     evas_object_move(o_trash, win_w - r, win_h - r);
	     evas_object_image_border_set(o_trash, 4, 4, 4, 4);
	     evas_object_resize(o_trash, r, r);
	     evas_object_layer_set(o_trash, 200);
	     evas_object_show(o_trash);
	     if (o_trash_can)
	       {
		  int                 cw, ch;

		  evas_object_image_size_get(o_trash_can, &cw, &ch);
		  evas_object_move(o_trash_can, win_w - (r / 2) - (cw / 2),
				   win_h - (r / 2) - (ch / 2));
		  evas_object_layer_set(o_trash_can, 199);
		  evas_object_show(o_trash_can);
	       }
	  }

	if (o_mini_image)
	  {
	     int                 ww, hh, sw, sh;

	     ww = 48;
	     hh = (48 * ph) / pw;
	     if (h > w)
	       {
		  hh = 48;
		  ww = (48 * pw) / ph;
	       }
	     evas_object_resize(o_mini_image, ww, hh);
	     evas_object_image_fill_set(o_mini_image, 0, 0, ww, hh);
	     evas_object_move(o_mini_image, win_w - ww - 3, win_h - hh - 3);
	     evas_object_image_border_set(o_mini_select, 4, 4, 4, 4);
	     sw = ww;
	     sh = hh;
	     if (win_w < w)
		sw = (win_w * ww) / w;
	     if (win_h < h)
		sh = (win_h * hh) / h;
	     mx = (-sx * ww) / pw;
	     my = (-sy * hh) / ph;
	     evas_object_resize(o_mini_select, sw + 6, sh + 6);
	     evas_object_image_fill_set(o_mini_select, 0, 0, sw + 6, sh + 6);
	     evas_object_move(o_mini_select,
			      win_w - ww - 3 - 3 + ((ww - sw) / 2) + mx,
			      win_h - hh - 3 - 3 + ((hh - sh) / 2) + my);
	     evas_object_layer_set(o_mini_image, 110);
	     evas_object_layer_set(o_mini_select, 111);
	     evas_object_show(o_mini_image);
	     evas_object_show(o_mini_select);
	  }
     }
   else
     {
       enum active_state command = active_in;
	evas_object_show(o_bg);
	evas_object_show(o_logo);
	e_fade_logo(&command);
     }
   evas_object_image_size_get(o_arrow_l, &w, &h);
   evas_object_move(o_arrow_l, 0, (win_h - h) / 2);
   evas_object_image_size_get(o_arrow_r, &w, &h);
   evas_object_move(o_arrow_r, win_w - w, (win_h - h) / 2);
   evas_object_image_size_get(o_arrow_t, &w, &h);
   evas_object_move(o_arrow_t, (win_w - w) / 2, 0);
   evas_object_image_size_get(o_arrow_b, &w, &h);
   evas_object_move(o_arrow_b, (win_w - w) / 2, win_h - h);
   evas_object_layer_set(o_arrow_l, 230);
   evas_object_layer_set(o_arrow_r, 230);
   evas_object_layer_set(o_arrow_t, 230);
   evas_object_layer_set(o_arrow_b, 230);

   e_handle_arrows();
   e_scroll_list(NULL);
}
/* XXX
void
e_handle_dnd(void)
{
   int                 i;
   char               *dnd_item;

   for (i = 0; i < dnd_num_files; i++)
     {
	dnd_item = strstr(dnd_files[i], "file:");
	dnd_item = strchr(dnd_item, '/');

	image_add_from_dnd(dnd_item);
     }
   return;
}
*/
