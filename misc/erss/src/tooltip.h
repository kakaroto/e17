typedef struct _erss_tooltip Erss_Tooltip;

struct _erss_tooltip {
   Evas *evas;
   Ecore_Evas *ee;
   Ecore_X_Window win;
   Evas_Object *bg;
   Evas_Object *etox;

   int x;
   int y;

   Ecore_Timer *timer;
};

void erss_window_move_tooltip (Ecore_Evas * ee);

Erss_Tooltip *erss_tooltip_new (char *description);
void erss_tooltip_hide (Erss_Tooltip *tt);
void erss_tooltip_show (Erss_Tooltip *tt);

void erss_tooltip_mouse_in (void *data, Evas *e, 
		Evas_Object *obj, void *event_info);
void erss_tooltip_mouse_out (void *data, Evas *e, 
		Evas_Object *obj, void *event_info);

