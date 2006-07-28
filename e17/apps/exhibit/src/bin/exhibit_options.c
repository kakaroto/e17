#include "exhibit.h"

Ex_Options *
_ex_options_new()
{
   Ex_Options *o;
   
   o = calloc(1, sizeof(Ex_Options));
   o = _ex_options_default(o);
   
   return o;
}

Ex_Options * 
_ex_options_default(Ex_Options *o) 
{
   o->app1 =     NULL;
   o->app2 =     NULL;
   o->app3 =     NULL;
   o->app4 =     NULL;
   o->fav_path = NULL;   
   o->blur_thresh      = EX_DEFAULT_BLUR_THRESH;
   o->sharpen_thresh   = EX_DEFAULT_SHARPEN_THRESH;
   o->brighten_thresh  = EX_DEFAULT_BRIGHTEN_THRESH;
   o->slide_interval   = EX_DEFAULT_SLIDE_INTERVAL;
   o->comments_visible = EX_DEFAULT_COMMENTS_VISIBLE;
   o->im_view          = EX_IMAGE_ONE_TO_ONE;

   return o;
}

void
_ex_options_free(Exhibit *e)
{
   E_FREE(e->options->app1);
   E_FREE(e->options->app2);
   E_FREE(e->options->app3);
   E_FREE(e->options->app4);
   E_FREE(e->options->fav_path);
   E_FREE(e->options);
}

void
_ex_options_save(Exhibit *e)
{
   /*  TODO
    *  Save options to .eet */   
}

Ex_Options *
_ex_options_load(Ex_Options *o)
{

   /*  TODO
    *  Load options from .eet */
   
   return o;
}

void
_ex_options_window_show(Exhibit *e)
{
   Etk_Widget *win;
   Etk_Widget *vbox, *vbox2;
   Etk_Widget *btn, *btn2;
   Etk_Widget *frame;
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Exhibit options");
   etk_window_resize(ETK_WINDOW(win), 200, 150);
   etk_container_border_width_set(ETK_CONTAINER(win), 10);
   etk_signal_connect("delete_event", ETK_OBJECT(win),
		      ETK_CALLBACK(_ex_options_window_delete_cb), win);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   frame = etk_frame_new("Choose standard view");
   etk_box_pack_start(ETK_BOX(vbox), frame, ETK_TRUE, ETK_FALSE, 0);
   
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);
   
   btn = etk_radio_button_new_with_label("Zoom 1:1", NULL);
   etk_box_pack_start(ETK_BOX(vbox2), btn, ETK_TRUE, ETK_FALSE, 0);
   
   btn2 = etk_radio_button_new_with_label_from_widget("Fit to window", 
						      ETK_RADIO_BUTTON(btn));
   etk_box_pack_start(ETK_BOX(vbox2), btn2, ETK_TRUE, ETK_FALSE, 0);
      
   etk_widget_show_all(ETK_WIDGET(win));
}

Etk_Bool 
_ex_options_window_delete_cb(void *data)
{
   etk_object_destroy(ETK_OBJECT(data));
  return ETK_TRUE;
}
