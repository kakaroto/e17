#include "entice.h"

void e_scroll_list(int v, void *data)
{
  int dd, d, fp;
      
  fp = (int)focus_pos - (win_h / 2);
  dd = d = fp + icon_y;
  if (dd < 0) dd = -dd;
  icon_y -= (d / 4);
  e_fix_icons();
  if (dd > 3)
    ecore_add_event_timer("e_scroll_list()", 0.05, e_scroll_list, v + 1, NULL);
}

void e_list_item_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  Image *im;
  Evas_List l = _data;
  int iw, ih;
  double x, y, w, h;
  im = l->data;

  if (icon_drag)
      evas_del_object(evas, icon_drag);
  
  if (im->thumb && !icon_drag) 
      icon_drag = evas_add_image_from_file(evas, im->thumb);
  else
      icon_drag = NULL;

  evas_get_image_size(evas, icon_drag, &iw, &ih);
  evas_get_geometry(evas, im->o_thumb, &x, &y, &w, &h);
  evas_set_layer(evas, icon_drag, 300);

  evas_move(evas, icon_drag, ((x + (w / 2)) - (iw / 2)), ((y + (h / 2)) - (ih / 2)));
  evas_hide(evas, icon_drag);
}

void e_list_item_drag(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  Image *im;
  Evas_List l = _data;
  int iw, ih;
  double x, y, w, h;

  im = l->data;

  if (icon_drag) {
      if (!dragging) {
	  dragging = 1;
	  e_list_item_zoom(-1 , _data);
	  e_fade_trash_in(0, (void *)1); }

      evas_get_image_size(evas, icon_drag, &iw, &ih);
      evas_show(evas, icon_drag);
      evas_move(evas, icon_drag, (_x - (iw / 2)), (_y - (ih / 2))); 
      evas_get_geometry(evas, o_trash, &x, &y, &w, &h);
      if ((_x > (int)x) && (_y > (int)y))
	  evas_set_color(evas, icon_drag, 200, 200, 200, 100);
      else
	  evas_set_color(evas, icon_drag, 255, 255, 255, 255);
     }
}

void e_list_item_select(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  Image *im;
  Evas_List l = _data;

  im = (Image *)(l->data);
 
  if (dragging) {
      e_fade_trash_out(0, NULL); 
      dragging = 0; }

  if (icon_drag) {
      evas_del_object(evas, icon_drag);
      icon_drag = NULL; 
}
       
  
  
  /* CS */
  if( 0 )
  /* if( e_file_is_dir(im->file) ) */
    {
      printf("%s\n",im->file);
      image_destroy_list();
      image_create_list_dir(im->file);
      image_create_thumbnails();
      icon_x = -100;
      e_fix_icons();
    }
  else
    {
	double x, y, w, h;
	evas_get_geometry(evas, o_trash, &x, &y, &w, &h);
	if ((_x > (int)x) && (_y > (int)y)) {
	    e_list_item_zoom(-1 , _data);
	    image_delete(im);
	    e_display_current_image(); }
	else {
	    current_image = _data;
	    e_display_current_image(); }
    }
}

void e_list_item_zoom(int v, void *data)
{
  Evas_List l;
  Image *im;
  static Evas_Object zo = NULL;
  static double t_start = 0.0;
  double t;
   
  l = data;
  im = l->data;
  if (v == -1)
    {
      evas_del_object(evas, zo);
      zo = NULL;
      return;
    }
  if (v == 0)
    {
      t_start = ecore_get_time();
      if ((im->thumb) && (im->file))
	{
	  zo = evas_add_image_from_file(evas, im->thumb);
	}
    }
  t = (ecore_get_time() - t_start) * 2;
  if (t > 1.0) t = 1.0;
  if (zo)
    {
      double x, y, w, h, nw, nh;
      int iw, ih;
	
      evas_get_image_size(evas, im->o_thumb, &iw, &ih);
      evas_get_geometry(evas, im->o_thumb, &x, &y, &w, &h);
      evas_set_layer(evas, zo, 300);
      nw = (double)iw * t;
      nh = (double)ih * t;
      evas_move(evas, zo, x + w + (t * 32), y + (h - nh) / 2);
      evas_resize(evas, zo, nw, nh);
      evas_set_image_fill(evas, zo, 0, 0, nw, nh);
      evas_set_color(evas, zo, 255, 255, 255, (int)(t * 255));
      evas_show(evas, zo);
    }
   
  if (t  < 1.0)
    ecore_add_event_timer("e_list_item_zoom()", 0.02, e_list_item_zoom, v + 1, data);
}

void e_list_item_in(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  Evas_List l=_data;
  Image *im=l->data;

  if( e_file_is_dir(im->file) )
    {
      txt_info[0][0]='\0';
      txt_info[1][0]='\0';
      sprintf(txt_info[0],"Directory: %s",im->file);
      e_fade_info_in(0,NULL);
    }
  else
    e_list_item_zoom(0, _data);
}

void e_list_item_out(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  Evas_List l=_data;
  Image *im=l->data;

  if( e_file_is_dir(im->file) )
    {
      e_fade_info_out(0,NULL);
    }
  else
    e_list_item_zoom(-1 , _data);
}

void e_list_scroll_up_timer(int v, void *data)
{
  int vv;
   
  vv = v / 5;
  if (vv > 15) vv = 15;
  icon_y -= 1 + vv;
  e_fix_icons();
  ecore_add_event_timer("e_list_scroll_up_timer()", 0.02, e_list_scroll_up_timer, v + 1, NULL);
}

void e_list_scroll_down_timer(int v, void *data)
{
  int vv;
   
  vv = v / 5;
  if (vv > 15) vv = 15;
  icon_y += 1 + vv;
  e_fix_icons();
  ecore_add_event_timer("e_list_scroll_down_timer()", 0.02, e_list_scroll_down_timer, v + 1, NULL);
}

void list_scroll_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  e_list_scroll_down_timer(0, NULL);
  evas_set_image_file(_e, _o, IM"list_arrow_u_2.png");
}

void list_scroll_up_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  ecore_del_event_timer("e_list_scroll_down_timer()");
  evas_set_image_file(_e, _o, IM"list_arrow_u_1.png");
}

void list_scroll_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  e_list_scroll_up_timer(0, NULL);
  evas_set_image_file(_e, _o, IM"list_arrow_d_2.png");
}

void list_scroll_down_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  ecore_del_event_timer("e_list_scroll_up_timer()");
  evas_set_image_file(_e, _o, IM"list_arrow_d_1.png");
}
