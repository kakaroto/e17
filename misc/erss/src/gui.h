void erss_mouse_click_item (void *data, Evas_Object *o, 
			    const char *sig, const char *src);
void erss_mouse_in_cursor_change (void *data, Evas *e, Evas_Object *obj,
				  void *event_info);
void erss_mouse_out_cursor_change (void *data, Evas *e, Evas_Object *obj,
				   void *event_info);

void erss_window_resize (Ecore_Evas *);

int  erss_set_time (void *);

int  erss_gui_init (char *);
int  erss_gui_exit (void);
