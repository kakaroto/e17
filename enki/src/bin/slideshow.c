// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "main.h"

#define TRANSFORMATION_DURATION 1.0

static Evas_Object *win = NULL;
static Evas_Object *notify, *notify_right;// *notify_left;
static Evas_Object *slideshow, *bt_start, *bt_stop, *sl, *rect;
static Elm_Slideshow_Item_Class itc;
static Evas_Object * _get(void *data, Evas_Object *obj);
static Enlil_Album *galbum = NULL;
static Enlil_Root *groot = NULL;
static Ecore_Animator *animator = NULL;
static double start;
static Enlil_Trans_Job_Type transformation;
static Eina_Bool state;


   static void
_key_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Up *ev = (Evas_Event_Key_Up*) event_info;

   //printf("%s\n", ev->key);
   if(!strcmp(ev->key, "Right") || !strcmp(ev->key, "Up"))
     elm_slideshow_next(slideshow);
   else if(!strcmp(ev->key, "Left") || !strcmp(ev->key, "Down"))
     elm_slideshow_previous(slideshow);
   else if(!strcmp(ev->key, "Escape"))
     slideshow_hide();
   else if(!strcmp(ev->key, "Home"))
     {
	const Eina_List *l = elm_slideshow_items_get(slideshow);
	elm_slideshow_show(eina_list_data_get(l));
     }
   else if(!strcmp(ev->key, "End"))
     {
	const Eina_List *l = elm_slideshow_items_get(slideshow);
	elm_slideshow_show(eina_list_data_get(eina_list_last(l)));
     }
}


   static void
_slideshow_move_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   int x,y,w,h;

   evas_object_geometry_get(slideshow,&x,&y,&w,&h);
   evas_object_resize(rect,w,h);
   evas_object_move(rect,x,y);
}

   static void
_notify_show(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   evas_object_show(data);
   evas_object_show(notify_right);
   //evas_object_show(notify_left);

   elm_notify_timer_init(data);
   elm_notify_timer_init(notify_right);
   //elm_notify_timer_init(notify_left);
}

   static void
_next(void *data, Evas_Object *obj, void *event_info)
{
   elm_slideshow_next(data);
}

   static void
_previous(void *data, Evas_Object *obj, void *event_info)
{
   elm_slideshow_previous(data);
}

   static void
_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   elm_notify_timeout_set(data, 0);
   elm_notify_timeout_set(notify_right, 0);
   //elm_notify_timeout_set(notify_left, 0);
}


   static void
_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   elm_notify_timeout_set(data, 3);
   elm_notify_timeout_set(notify_right, 3);
   //elm_notify_timeout_set(notify_left, 3);
}

   static void
_hv_select(void *data, Evas_Object *obj, void *event_info)
{
   elm_slideshow_transition_set(slideshow, data);
   elm_hoversel_label_set(obj, data);
}

   static void
_start(void *data, Evas_Object *obj, void *event_info)
{
   elm_slideshow_timeout_set(slideshow, (int)elm_spinner_value_get(data));

   elm_object_disabled_set(bt_start, 1);
   elm_object_disabled_set(bt_stop, 0);
}

   static void
_stop(void *data, Evas_Object *obj, void *event_info)
{
   elm_slideshow_timeout_set(slideshow, 0);
   elm_object_disabled_set(bt_start, 0);
   elm_object_disabled_set(bt_stop, 1);
}

   static void
_spin(void *data, Evas_Object *obj, void *event_info)
{
   if(elm_slideshow_timeout_get(slideshow) > 0)
     elm_slideshow_timeout_set(slideshow, (int)elm_spinner_value_get(data));
}

   static void
_close(void *data, Evas_Object *obj, void *event_info)
{
   slideshow_hide();
}

   static Evas_Object *
_get(void *data, Evas_Object *obj)
{
   char buf[PATH_MAX];

   snprintf(buf,PATH_MAX, "%s/%s", enlil_photo_path_get(data), enlil_photo_file_name_get(data));
   //Evas_Object *o = photo_object_add(slideshow);
   //photo_object_theme_file_set(o, THEME, "photo/slideshow");
   //photo_object_file_set(o, buf, NULL);

   Evas_Object *o = elm_photocam_add(obj);
   elm_photocam_zoom_mode_set(o,ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT);
   elm_photocam_file_set(o, buf);


   return o;
}

   static void
_fit(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *o;
   Elm_Slideshow_Item *item;

   item = elm_slideshow_item_current_get(slideshow);
   if(item)
     {
	o = elm_slideshow_item_object_get(item);
	elm_photocam_zoom_mode_set(o, ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT);
     }
}


   static void
_fill(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *o;
   Elm_Slideshow_Item *item;

   item = elm_slideshow_item_current_get(slideshow);
   if(item)
     {
	o = elm_slideshow_item_object_get(item);
	elm_photocam_zoom_mode_set(o, ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL);
     }
}

   static void
_1_1(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *o;
   Elm_Slideshow_Item *item;

   item = elm_slideshow_item_current_get(slideshow);
   if(item)
     {
	o = elm_slideshow_item_object_get(item);
	elm_photocam_zoom_mode_set(o, ELM_PHOTOCAM_ZOOM_MODE_MANUAL);
	elm_photocam_zoom_set(o, 1);
     }
}

   static void
_slider_photocam_zoom_cb(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *photocam;
   Elm_Slideshow_Item *item;
   int zoom = 1;
   int i;
   int val = elm_slider_value_get(sl);

   item = elm_slideshow_item_current_get(slideshow);
   if(!item) return ;
   photocam = elm_slideshow_item_object_get(item);

   for(i=0; i<val - 1; i++)
     zoom *= 2;

   elm_photocam_zoom_mode_set(photocam, ELM_PHOTOCAM_ZOOM_MODE_MANUAL);
   elm_photocam_zoom_set(photocam, zoom);
}


static void _inwin_del_cb(void *data)
{
   eina_list_free(data);
}

   static void
_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elm_Slideshow_Item *item;

   item = elm_slideshow_item_current_get(slideshow);
   if(item)
     {
	Enlil_Photo *photo = elm_slideshow_item_data_get(item);
	Eina_List *l = eina_list_append(NULL, photo);
	inwin_photo_delete_new(win, _inwin_del_cb, l, l);
     }
}

   /* TOTO: finish this code a day
static int _transformation(void *data)
{
   Elm_Slideshow_Item *item = elm_slideshow_item_current_get(slideshow);
   if(item)
     {
	Evas_Coord x, y, w, h;
	double p, deg = 0.0;
	double t;
	Evas_Map *m; 
	Evas_Object *o = elm_slideshow_item_object_get(item);
        evas_object_geometry_get(o, &x, &y, &w, &h);

	if (!animator) return 0;

	t = ecore_loop_time_get() - start;
	t = t / TRANSFORMATION_DURATION;
	if (t > 1.0) t = 1.0;

	p = 1.0 - t;
	p = 1.0 - (p * p);

	switch(transformation)
	  {
	   case Enlil_TRANS_ROTATE_90:
	      deg = 90.0 * p + state * 90;
	   default :
	      deg = - ((3 - state) * 90.0) - (90.0 * p);
	  }

	printf("%f\n", deg);
	m = evas_map_new(4);

	evas_map_util_points_populate_from_geometry(m, x, y, w, h, 0);
	//evas_map_util_rotate(m, 90, x + (w / 2), y + (h / 2));
	evas_map_util_3d_rotate(m, 0.0, 0.0, deg, x + w / 2, y + h /2, 0);

	evas_object_map_enable_set(o, 1);
	evas_object_map_set(o, m);

	evas_map_free(m);

	if (t >= 1.0)
	  {
	     animator = NULL;
	     state = !state;
	     return 0;
	  }

	return 1;
     }

   return 0;
}

   static void
_rotate_90_cb(void *data, Evas_Object *obj, void *event_info)
{
   transformation = Enlil_TRANS_ROTATE_90;
   if(animator)
     ecore_animator_del(animator);

   start = ecore_loop_time_get();
   animator = ecore_animator_add(_transformation, NULL);
}
*/
   static void
_photocam_mouse_wheel_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Object *photocam;
   Elm_Slideshow_Item *item;
   Evas_Event_Mouse_Wheel *ev = (Evas_Event_Mouse_Wheel*) event_info;
   int zoom;
   double val;
   //unset the mouse wheel
   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;

   item = elm_slideshow_item_current_get(slideshow);
   if(!item) return ;
   photocam = elm_slideshow_item_object_get(item);

   zoom = elm_photocam_zoom_get(photocam);
   if (ev->z>0 && zoom == 1) return;

   if (ev->z > 0)
     zoom /= 2;
   else
     zoom *= 2;

   val = 1;
   int _zoom = zoom;
   while(_zoom>1)
     {
	_zoom /= 2;
	val++;
     }

   if(val>10) return;

   elm_photocam_zoom_mode_set(photocam, ELM_PHOTOCAM_ZOOM_MODE_MANUAL);
   if (zoom >= 1) elm_photocam_zoom_set(photocam, zoom);

   elm_slider_value_set(sl, val);
}

   void
_init_slideshow()
{
   Evas_Object *bg, *bx, *bt, *hv, *spin, *icon, *sp;
   const Eina_List *l;
   const char *transition;

   if(win)
     return;

   itc.func.get = _get;
   itc.func.del = NULL;

   win = elm_win_add(NULL, "Slideshow", ELM_WIN_BASIC);
   elm_win_title_set(win, "Slideshow");

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0);
   evas_object_color_set(bg, 0, 0, 0, 255);
   evas_object_show(bg);

   slideshow = elm_slideshow_add(win);
   elm_slideshow_loop_set(slideshow, 1);
   elm_win_resize_object_add(win, slideshow);
   evas_object_size_hint_weight_set(slideshow, 1.0, 1.0);
   evas_object_show(slideshow);

   evas_object_event_callback_add(slideshow, EVAS_CALLBACK_RESIZE, _slideshow_move_resize_cb, NULL);
   evas_object_event_callback_add(slideshow, EVAS_CALLBACK_MOVE, _slideshow_move_resize_cb, NULL);

   //rectangle on top of the slideshow which retrieves the mouse wheel
   rect = evas_object_rectangle_add(evas_object_evas_get(win));
   evas_object_color_set(rect, 0, 0, 0, 0);
   evas_object_repeat_events_set(rect,1);
   evas_object_show(rect);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_WHEEL, _photocam_mouse_wheel_cb, NULL);
   evas_object_raise(rect);


   notify = elm_notify_add(win);
   elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_BOTTOM);
   elm_win_resize_object_add(win, notify);
   elm_notify_timeout_set(notify, 3);

   bx = elm_box_add(win);
   elm_box_horizontal_set(bx, 1);
   elm_notify_content_set(notify, bx);
   evas_object_show(bx);

   evas_object_event_callback_add(bx, EVAS_CALLBACK_MOUSE_IN, _mouse_in, notify);
   evas_object_event_callback_add(bx, EVAS_CALLBACK_MOUSE_OUT, _mouse_out, notify);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Previous");
   evas_object_smart_callback_add(bt, "clicked", _previous, slideshow);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Next");
   evas_object_smart_callback_add(bt, "clicked", _next, slideshow);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   sp = elm_separator_add(win);
   elm_box_pack_end(bx, sp);
   evas_object_show(sp);

   hv = elm_hoversel_add(win);
   elm_box_pack_end(bx, hv);
   elm_hoversel_hover_parent_set(hv, win);
   EINA_LIST_FOREACH(elm_slideshow_transitions_get(slideshow), l, transition)
      elm_hoversel_item_add(hv, transition, NULL, 0, _hv_select, transition);
   elm_hoversel_label_set(hv, eina_list_data_get(elm_slideshow_transitions_get(slideshow)));
   evas_object_show(hv);

   sp = elm_separator_add(win);
   elm_box_pack_end(bx, sp);
   evas_object_show(sp);

   spin = elm_spinner_add(win);
   elm_spinner_label_format_set(spin, "%2.0f secs.");
   evas_object_smart_callback_add(spin, "changed", _spin, spin);
   elm_spinner_step_set(spin, 1);
   elm_spinner_min_max_set(spin, 1, 30);
   elm_spinner_value_set(spin, 6);
   elm_box_pack_end(bx, spin);
   evas_object_show(spin);

   bt = elm_button_add(win);
   bt_start = bt;
   elm_button_label_set(bt, "Start");
   evas_object_smart_callback_add(bt, "clicked", _start, spin);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   bt_stop = bt;
   elm_button_label_set(bt, "Stop");
   evas_object_smart_callback_add(bt, "clicked", _stop, spin);
   elm_box_pack_end(bx, bt);
   elm_object_disabled_set(bt, 1);
   evas_object_show(bt);

   sp = elm_separator_add(win);
   elm_box_pack_end(bx, sp);
   evas_object_show(sp);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Close");
   evas_object_smart_callback_add(bt, "clicked", _close, NULL);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   notify_right = elm_notify_add(win);
   elm_notify_orient_set(notify_right, ELM_NOTIFY_ORIENT_RIGHT);
   elm_win_resize_object_add(win, notify_right);
   elm_notify_timeout_set(notify_right, 3);

   bx = elm_box_add(win);
   elm_box_horizontal_set(bx, 0);
   elm_notify_content_set(notify_right, bx);
   evas_object_show(bx);

   evas_object_event_callback_add(bx, EVAS_CALLBACK_MOUSE_IN, _mouse_in, notify);
   evas_object_event_callback_add(bx, EVAS_CALLBACK_MOUSE_OUT, _mouse_out, notify);

   icon = elm_icon_add(win);
   elm_icon_file_set(icon, THEME, "icons/1_1");

   bt = elm_button_add(win);
   elm_button_label_set(bt, "1:1");
   evas_object_size_hint_align_set(bt, -1, -1);
   elm_button_icon_set(bt, icon);
   evas_object_smart_callback_add(bt, "clicked", _1_1, slideshow);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   icon = elm_icon_add(win);
   elm_icon_file_set(icon, THEME, "icons/fit");

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Fit");
   evas_object_size_hint_align_set(bt, -1, -1);
   elm_button_icon_set(bt, icon);
   evas_object_smart_callback_add(bt, "clicked", _fit, slideshow);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   icon = elm_icon_add(win);
   elm_icon_file_set(icon, THEME, "icons/fill");

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Fill");
   evas_object_size_hint_align_set(bt, -1, -1);
   elm_button_icon_set(bt, icon);
   evas_object_smart_callback_add(bt, "clicked", _fill, slideshow);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   sp = elm_separator_add(win);
   elm_separator_horizontal_set(sp, 1);
   elm_box_pack_end(bx, sp);
   evas_object_show(sp);

   sl = elm_slider_add(win);
   evas_object_size_hint_weight_set(sl, 1.0, 0.0);
   evas_object_size_hint_align_set(sl, -1.0, -1.0);
   elm_slider_label_set(sl, "Zoom");
   elm_slider_indicator_format_set(sl, "%3.0f");
   elm_slider_min_max_set(sl, 1, 10);
   elm_slider_value_set(sl, 50);
   elm_slider_horizontal_set(sl, 0);
   elm_slider_span_size_set(sl, 150);
   elm_slider_unit_format_set(sl, "%4.0f");
   evas_object_smart_callback_add(sl, "changed", _slider_photocam_zoom_cb, NULL);
   evas_object_show(sl);
   elm_box_pack_end(bx, sl);


   /*notify_left = elm_notify_add(win);
   elm_notify_orient_set(notify_left, ELM_NOTIFY_ORIENT_LEFT);
   elm_win_resize_object_add(win, notify_left);
   elm_notify_timeout_set(notify_left, 3);

   bx = elm_box_add(win);
   elm_box_horizontal_set(bx, 0);
   elm_notify_content_set(notify_left, bx);
   evas_object_show(bx);

   evas_object_event_callback_add(bx, EVAS_CALLBACK_MOUSE_IN, _mouse_in, notify);
   evas_object_event_callback_add(bx, EVAS_CALLBACK_MOUSE_OUT, _mouse_out, notify);

   icon = elm_icon_add(win);
   elm_icon_file_set(icon, THEME, "icons/1_1");

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Delete");
   evas_object_size_hint_align_set(bt, -1, -1);
   elm_button_icon_set(bt, icon);
   evas_object_smart_callback_add(bt, "clicked", _delete_cb, slideshow);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   
   icon = elm_icon_add(win);
   elm_icon_file_set(icon, THEME, "icons/rotate/90");

   bt = elm_button_add(win);
   elm_button_label_set(bt, "90°");
   elm_button_icon_set(bt, icon);
   evas_object_size_hint_align_set(bt, -1, -1);
   evas_object_smart_callback_add(bt, "clicked", _rotate_90_cb, slideshow);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   */




   //evas_object_smart_callback_add(slideshow, "clicked", _next, slideshow);
   evas_object_event_callback_add(slideshow, EVAS_CALLBACK_MOUSE_MOVE, _notify_show, notify);
   evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_UP, _key_up_cb, NULL);
   evas_object_focus_set(win, 1);
}


void slideshow_show()
{
   _init_slideshow();

   elm_win_fullscreen_set(win, 1);
   //evas_object_resize(win, 400,400);
   evas_object_show(win);
}


void slideshow_album_add(Enlil_Album *album, Enlil_Photo *_photo)
{
   Eina_List *l;
   Enlil_Photo *photo;
   Elm_Slideshow_Item *item = NULL;

   _init_slideshow();

   galbum = album;
   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
     {
	if(enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_PHOTO)
	  {
	     Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
	     photo_data->slideshow_item = elm_slideshow_item_add(slideshow, &itc, photo);

	     if(photo == _photo)
	       item = photo_data->slideshow_item;
	  }
     }
   if(item)
     elm_slideshow_show(item);
}

void slideshow_root_add(Enlil_Root *root, Enlil_Photo *_photo)
{
   Eina_List *l, *l2;
   Enlil_Photo *photo;
   Enlil_Album *album;
   Elm_Slideshow_Item *item = NULL;

   _init_slideshow();

   groot = root;
   EINA_LIST_FOREACH(enlil_root_albums_get(root), l, album)
     {
	EINA_LIST_FOREACH(enlil_album_photos_get(album), l2, photo)
	  {
	     if(enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_PHOTO)
	       {
		  Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
		  photo_data->slideshow_item = elm_slideshow_item_add(slideshow, &itc, photo);
		  if(photo == _photo)
		    item = photo_data->slideshow_item;
	       }
	  }
     }

   if(item)
     elm_slideshow_show(item);
}

void slideshow_clear()
{
   Eina_List *l, *l2;
   Enlil_Photo *photo;
   Enlil_Album *album;

   _init_slideshow();

   elm_slideshow_clear(slideshow);

   if(galbum)
     {
	EINA_LIST_FOREACH(enlil_album_photos_get(galbum), l, photo)
	  {
	     Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
	     photo_data->slideshow_item = NULL;
	  }
	galbum = NULL;
     }

   if(groot)
     {
	EINA_LIST_FOREACH(enlil_root_albums_get(groot), l, album)
	  {
	     EINA_LIST_FOREACH(enlil_album_photos_get(album), l2, photo)
	       {
		  Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
		  photo_data->slideshow_item = NULL;
	       }
	  }
	groot = NULL;
     }
}

void slideshow_hide()
{
   _init_slideshow();
   evas_object_hide(win);
}

