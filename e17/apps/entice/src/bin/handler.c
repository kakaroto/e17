#include "entice.h"

void e_handle_arrows(void)
{
  if (o_image)
    {
      double ix, iy, iw, ih;
	
      evas_get_geometry(evas, o_image, &ix, &iy, &iw, &ih);	
      if (ix < 0) e_fade_l_arrow_in(0, NULL);
      else e_fade_l_arrow_out(0, NULL);
      if (iy < 0) e_fade_t_arrow_in(0, NULL);
      else e_fade_t_arrow_out(0, NULL);
      if (iw + ix > win_w) e_fade_r_arrow_in(0, NULL);
      else e_fade_r_arrow_out(0, NULL);
      if (ih + iy > win_h) e_fade_b_arrow_in(0, NULL);
      else e_fade_b_arrow_out(0, NULL);
    }
  else
    {
      e_fade_l_arrow_out(0, NULL);
      e_fade_r_arrow_out(0, NULL);
      e_fade_t_arrow_out(0, NULL);
      e_fade_b_arrow_out(0, NULL);
    }
}

void e_handle_resize(void)
{
  int w, h;
   
  evas_move(evas, o_bg, 0, 0);
  evas_resize(evas, o_bg, win_w, win_h);
  evas_set_layer(evas, o_bg, 0);
   
  evas_get_image_size(evas, o_logo, &w, &h);
  evas_move(evas, o_logo, (win_w - w) / 2, (win_h - h) / 2);
  evas_resize(evas, o_logo, w, h);
  evas_set_layer(evas, o_logo, 100);

  evas_get_image_size(evas, o_panel, &w, NULL);
  if (!panel_active)
    evas_move(evas, o_panel, -w, 0);
  evas_resize(evas, o_panel, w, win_h);
  evas_set_layer(evas, o_panel, 200);
  evas_set_layer(evas, o_panel_arrow_u, 250);
  evas_set_layer(evas, o_panel_arrow_d, 250);
   
  evas_move(evas, o_showpanel, 0, 0);
  evas_resize(evas, o_showpanel, 64, win_h);
  if (panel_active)
    evas_set_layer(evas, o_showpanel, 180);
  else
    evas_set_layer(evas, o_showpanel, 1000);
  {   
    double px = 0;
	
    if (buttons_active)
      px = win_w - 256;	
    else
      px = win_w;
    evas_move(evas, o_bt_prev, px + 0, 0);
    evas_move(evas, o_bt_next, px + 32, 0);
    evas_move(evas, o_bt_zoom_normal, px + 64, 0);
    evas_move(evas, o_bt_zoom_in, px + 96, 0);
    evas_move(evas, o_bt_zoom_out, px + 128, 0);
    evas_move(evas, o_bt_expand, px + 160, 0);
    evas_move(evas, o_bt_full, px + 192, 0);
    evas_move(evas, o_bt_close, px + 224, 0);
  }
  evas_move(evas, o_showbuttons, win_w - 256, 0);
  evas_resize(evas, o_showbuttons, 256, 32);
  evas_set_layer(evas, o_showbuttons, 1100);
  evas_set_layer(evas, o_bt_close, 1300);
  evas_set_layer(evas, o_bt_expand, 1300);
  evas_set_layer(evas, o_bt_full, 1300);
  evas_set_layer(evas, o_bt_next, 1300);
  evas_set_layer(evas, o_bt_prev, 1300);
  evas_set_layer(evas, o_bt_zoom_in, 1300);
  evas_set_layer(evas, o_bt_zoom_normal, 1300);
  evas_set_layer(evas, o_bt_zoom_out, 1300);
   
  evas_move(evas, o_hidepanel, 128, 0);
  evas_resize(evas, o_hidepanel, win_w - 128, win_h);
  evas_set_layer(evas, o_hidepanel, 1000);
   
  if (o_image)
    {
      int sx, sy, mx, my, pw, ph;
	
      sx = scroll_x;
      sy = scroll_y;
      evas_get_image_size(evas, o_image, &w, &h);
      pw = w;
      ph = h;
      w = (int)((double)w / scale);
      h = (int)((double)h / scale);
      if (w > win_w)
	{
	  if (sx > ((w - win_w) / 2)) sx = ((w - win_w) / 2);
	  if (sx < -((w - win_w + 1) / 2)) sx = -((w - win_w + 1) / 2);
	}
      else sx = 0;
      if (h > win_h)
	{
	  if (sy > ((h - win_h) / 2)) sy = ((h - win_h) / 2);
	  if (sy < -((h - win_h + 1) / 2)) sy = -((h - win_h + 1) / 2);
	}
      else sy = 0;
      if (o_trash)
      {
	  int dw, dh, r;

	  dh = (win_h / 4);
	  dw = (win_w / 4);
	  r = (dh + dw) / 2;
	  evas_set_image_fill(evas, o_trash, 0, 0, r, r);
	  evas_move(evas, o_trash, win_w - r, win_h - r);
	  evas_set_image_border(evas, o_trash, 4, 4, 4, 4);
	  evas_resize(evas, o_trash, r, r);
	  evas_set_layer(evas, o_trash, 200);
	  evas_show(evas, o_trash);
      }
      
      if (o_mini_image)
	{
	  int ww, hh, sw, sh;
	     
	  ww = 48;
	  hh = (48 * ph) / pw;
	  if (h > w)
	    {
	      hh = 48;
	      ww = (48 * pw) / ph;
	    }
	  evas_resize(evas, o_mini_image, ww, hh);	     
	  evas_set_image_fill(evas, o_mini_image, 0, 0, ww, hh);
	  evas_move(evas, o_mini_image, win_w - ww - 3, win_h - hh - 3);
	  evas_set_image_border(evas, o_mini_select, 4, 4, 4, 4);
	  sw = ww;
	  sh = hh;
	  if (win_w < w) sw = (win_w * ww) / w;
	  if (win_h < h) sh = (win_h * hh) / h;
	  mx = (-sx * ww) / pw;
	  my = (-sy * hh) / ph;
	  evas_resize(evas, o_mini_select, sw + 6, sh + 6);
	  evas_set_image_fill(evas, o_mini_select, 0, 0, sw + 6, sh + 6);
	  evas_move(evas, o_mini_select, 
		    win_w - ww - 3 - 3 + ((ww - sw) / 2) + mx,
		    win_h - hh - 3 - 3 + ((hh - sh) / 2) + my);
	  evas_set_layer(evas, o_mini_image, 110);
	  evas_set_layer(evas, o_mini_select, 111);
	  evas_show(evas, o_mini_image);
	  evas_show(evas, o_mini_select);
	}
      evas_move(evas, o_image, 
		sx + ((win_w - w) / 2), 
		sy + ((win_h - h) / 2));
      evas_set_image_fill(evas, o_image, 0, 0, w, h);
      evas_resize(evas, o_image, w, h);
      evas_set_layer(evas, o_image, 100);	
	
      scroll_sx = sx;
      scroll_sy = sy;
	
      if ((win_w <= w) && 
	  (win_h <= h) && 
	  (!evas_get_image_alpha(evas, o_image)))
	evas_hide(evas, o_bg);
      else
	evas_show(evas, o_bg);	
      evas_hide(evas, o_logo);
    }
  else
    {
      evas_show(evas, o_bg);
      evas_show(evas, o_logo);
      e_fade_logo_in(0, NULL);
    }
  evas_get_image_size(evas, o_arrow_l, &w, &h);
  evas_move(evas, o_arrow_l, 0, (win_h - h) / 2);
  evas_get_image_size(evas, o_arrow_r, &w, &h);
  evas_move(evas, o_arrow_r, win_w - w, (win_h - h) / 2);
  evas_get_image_size(evas, o_arrow_t, &w, &h);
  evas_move(evas, o_arrow_t, (win_w - w) / 2, 0);
  evas_get_image_size(evas, o_arrow_b, &w, &h);
  evas_move(evas, o_arrow_b, (win_w - w) / 2, win_h - h);
  evas_set_layer(evas, o_arrow_l, 230);
  evas_set_layer(evas, o_arrow_r, 230);
  evas_set_layer(evas, o_arrow_t, 230);
  evas_set_layer(evas, o_arrow_b, 230);

  e_handle_arrows();
  e_scroll_list(0, NULL);
}

void e_handle_dnd(void)
{
    DIR *d;
    int i;
    char *dnd_item;

    for (i = 0; i < dnd_num_files; i++)
	{
	    dnd_item = strstr(dnd_files[i], "file:" );
	    dnd_item = strchr(dnd_item, '/' ); 

	    image_add_from_dnd(dnd_item);	   
	}
    return;
}
