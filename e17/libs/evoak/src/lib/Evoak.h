#ifndef _EVOAK_H
#define _EVOAK_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/

   enum _Evoak_Callback_Type
     {
	EVOAK_CALLBACK_MOUSE_IN, /**< Mouse In Event */
	  EVOAK_CALLBACK_MOUSE_OUT, /**< Mouse Out Event */
	  EVOAK_CALLBACK_MOUSE_DOWN, /**< Mouse Button Down Event */
	  EVOAK_CALLBACK_MOUSE_UP, /**< Mouse Button Up Event */
	  EVOAK_CALLBACK_MOUSE_MOVE, /**< Mouse Move Event */
	  EVOAK_CALLBACK_MOUSE_WHEEL, /**< Mouse Wheel Event */
	  EVOAK_CALLBACK_FREE, /**< Object Being Freed */
	  EVOAK_CALLBACK_KEY_DOWN, /**< Key Press Event */
	  EVOAK_CALLBACK_KEY_UP, /**< Key Release Event */
	  /* FIXME: these events are not implemented yet */
	  EVOAK_CALLBACK_FOCUS_IN, /**< Focus In Event */
	  EVOAK_CALLBACK_FOCUS_OUT, /**< Focus Out Event */
	  EVOAK_CALLBACK_SHOW, /**< Show Event */
	  EVOAK_CALLBACK_HIDE, /**< Hide Event */
	  EVOAK_CALLBACK_MOVE, /**< Move Event */
	  EVOAK_CALLBACK_RESIZE, /**< Resize Event */
	  EVOAK_CALLBACK_RESTACK /**< Restack Event */
     };
   enum _Evoak_Modifier
     {
	EVOAK_MODIFIER_SHIFT = (1 << 0),
	EVOAK_MODIFIER_CTRL  = (1 << 1),
	EVOAK_MODIFIER_ALT   = (1 << 2),
	EVOAK_MODIFIER_SUPER = (1 << 3),
	EVOAK_MODIFIER_HYPER = (1 << 4)
     };
   enum _Evoak_Lock
     {
	EVOAK_LOCK_CAPS   = (1 << 0),
	EVOAK_LOCK_SCROLL = (1 << 1),
	EVOAK_LOCK_NUM    = (1 << 2),
	EVOAK_LOCK_SHIFT  = (1 << 3)
     };
   typedef enum _Evoak_Callback_Type Evoak_Callback_Type; /**< The type of event to trigger the callback */
   typedef enum _Evoak_Modifier Evoak_Modifier;
   typedef enum _Evoak_Lock Evoak_Lock;
   
   typedef int Evoak_Coord;
   typedef int Evoak_Font_Size;
   typedef int Evoak_Bool;
   typedef double Evoak_Angle;
   
   typedef struct _Evoak Evoak;
   typedef struct _Evoak_Object Evoak_Object;
   
   typedef struct _Evoak_List Evoak_List;
   
   struct _Evoak_List
     {
	void       *data; /**< Pointer to list element payload */
        Evoak_List *next; /**< Next member in the list */
        Evoak_List *prev; /**< Previous member in the list */
     };
   
   /* callback event info structs */
   typedef struct _Evoak_Event_Mouse_Move Evoak_Event_Mouse_Move;
   typedef struct _Evoak_Event_Mouse_In Evoak_Event_Mouse_In;
   typedef struct _Evoak_Event_Mouse_Out Evoak_Event_Mouse_Out;
   typedef struct _Evoak_Event_Mouse_Down Evoak_Event_Mouse_Down;
   typedef struct _Evoak_Event_Mouse_Up Evoak_Event_Mouse_Up;
   typedef struct _Evoak_Event_Mouse_Wheel Evoak_Event_Mouse_Wheel;
   typedef struct _Evoak_Event_Key_Down Evoak_Event_Key_Down;
   typedef struct _Evoak_Event_Key_Up Evoak_Event_Key_Up;
   
   struct _Evoak_Event_Mouse_Move
     {
	struct {
	   Evoak_Coord x, y;
	} cur, prev;
	int buttons;
	Evoak_Modifier modmask;
	Evoak_Lock lockmask;
     };
   
   struct _Evoak_Event_Mouse_In
     {
	Evoak_Coord x, y;
	int buttons;
	Evoak_Modifier modmask;
	Evoak_Lock lockmask;
     };
   
   struct _Evoak_Event_Mouse_Out
     {
	Evoak_Coord x, y;
	int buttons;
	Evoak_Modifier modmask;
	Evoak_Lock lockmask;
     };
   
   struct _Evoak_Event_Mouse_Down
     {
	Evoak_Coord x, y;
	int button;
	Evoak_Modifier modmask;
	Evoak_Lock lockmask;
     };
   
   struct _Evoak_Event_Mouse_Up
     {
	Evoak_Coord x, y;
	int button;
	Evoak_Modifier modmask;
	Evoak_Lock lockmask;
     };
   
   struct _Evoak_Event_Mouse_Wheel
     {
	int dir;
	int z;
	Evoak_Coord x, y;
	Evoak_Modifier modmask;
	Evoak_Lock lockmask;
     };
   
   struct _Evoak_Event_Key_Down
     {
	char *keyname;
	char *string;
	Evoak_Modifier modmask;
	Evoak_Lock lockmask;
     };
   
   struct _Evoak_Event_Key_Up
     {
	char *keyname;
	char *string;
	Evoak_Modifier modmask;
	Evoak_Lock lockmask;
     };
   
   /* events a client may recieve */
   extern int EVOAK_EVENT_CANVAS_INFO;
   typedef struct
     {
	Evoak *evoak;
	Evoak_Coord w, h;
	int rotation;
     } Evoak_Event_Canvas_Info;
   
   extern int EVOAK_EVENT_SYNC_REPLY;
   typedef struct
     {
	Evoak *evoak;
	int id;
     } Evoak_Event_Sync_Reply;
   extern int EVOAK_EVENT_DISCONNECT;
   typedef struct
     {
	Evoak *evoak;
     } Evoak_Event_Disconnect;
   
   int           evoak_init                          (void);
   int           evoak_shutdown                      (void);
   Evoak        *evoak_connect                       (char *server, char *client_name, char *client_class);
   void          evoak_disconnect                    (Evoak *ev);
   
   void          evoak_output_size_get               (Evoak *ev, Evoak_Coord *w, Evoak_Coord *h);
   
   void          evoak_sync                          (Evoak *ev, int id);
   void          evoak_freeze                        (Evoak *ev);
   void          evoak_thaw                          (Evoak *ev);

   void          evoak_object_del                    (Evoak_Object *o);
   const char   *evoak_object_type_get               (Evoak_Object *o);
   
   void          evoak_object_move                   (Evoak_Object *o, Evoak_Coord x, Evoak_Coord y);
   void          evoak_object_resize                 (Evoak_Object *o, Evoak_Coord w, Evoak_Coord h);
   void          evoak_object_geometry_get           (Evoak_Object *o, Evoak_Coord *x, Evoak_Coord *y, Evoak_Coord *w, Evoak_Coord *h);

   
   void          evoak_object_show                   (Evoak_Object *o);
   void          evoak_object_hide                   (Evoak_Object *o);
   Evoak_Bool    evoak_object_visible_get            (Evoak_Object *o);
   
   void              evoak_object_clip_set           (Evoak_Object *o, Evoak_Object *oc);
   Evoak_Object     *evoak_object_clip_get           (Evoak_Object *o);
   void              evoak_object_clip_unset         (Evoak_Object *o, Evoak_Object *oc);
   const Evoak_List *evoak_object_clipees_get        (Evoak_Object *o);
   
   void          evoak_object_color_set              (Evoak_Object *o, int r, int g, int b, int a);
   void          evoak_object_color_get              (Evoak_Object *o, int *r, int *g, int *b, int *a);
   
   void          evoak_object_layer_set              (Evoak_Object *o, int l);
   int           evoak_object_layer_get              (Evoak_Object *o);
   
   void          evoak_object_raise                  (Evoak_Object *o);
   void          evoak_object_lower                  (Evoak_Object *o);
   void          evoak_object_stack_above            (Evoak_Object *o, Evoak_Object *above);
   void          evoak_object_stack_below            (Evoak_Object *o, Evoak_Object *below);
   Evoak_Object *evoak_object_above_get              (Evoak_Object *o);
   Evoak_Object *evoak_object_below_get              (Evoak_Object *o);
   Evoak_Object *evoak_object_bottom_get             (Evoak *ev);
   Evoak_Object *evoak_object_top_get                (Evoak *ev);

   void          evoak_object_data_set               (Evoak_Object *o, const char *key, const void *data);
   void         *evoak_object_data_get               (Evoak_Object *o, const char *key);
   void         *evoak_object_data_del               (Evoak_Object *o, const char *key);
   
   void          evoak_object_name_set               (Evoak_Object *o, const char *name);
   const char   *evoak_object_name_get               (Evoak_Object *o);
   Evoak_Object *evoak_object_name_find              (Evoak *ev, const char *name);
   
   Evoak        *evoak_object_evoak_get              (Evoak_Object *o);
   
   void          evoak_object_pass_events_set        (Evoak_Object *o, Evoak_Bool onoff);
   Evoak_Bool    evoak_object_pass_events_get        (Evoak_Object *o);
   void          evoak_object_repeat_events_set      (Evoak_Object *o, Evoak_Bool onoff);
   Evoak_Bool    evoak_object_repeat_events_get      (Evoak_Object *o);
   
   void          evoak_object_event_callback_add     (Evoak_Object *o, Evoak_Callback_Type type, void (*func) (void *data, Evoak *e, Evoak_Object *obj, void *event_info), const void *data);
   void         *evoak_object_event_callback_del     (Evoak_Object *o, Evoak_Callback_Type type, void (*func) (void *data, Evoak *e, Evoak_Object *obj, void *event_info));

   /* this is app LOCAL only - if this client has the focus, which objects */
   /* get key press events. currently all apps have the focus :) */
   void          evoak_object_focus_set              (Evoak_Object *o, Evoak_Bool onoff);
   Evoak_Bool    evoak_object_focus_get              (Evoak_Object *o);
   
   Evoak_Object      *evoak_object_rectangle_add          (Evoak *ev);
   
   Evoak_Object      *evoak_object_image_add              (Evoak *ev);
   void               evoak_object_image_file_set         (Evoak_Object *o, const char *file, const char *key);
   void               evoak_object_image_file_get         (Evoak_Object *o, const char **file, const char **key);
/* int                evoak_object_image_load_error_get   (Evoak_Object *o); */
   
   void               evoak_object_image_fill_set         (Evoak_Object *o, Evoak_Coord x, Evoak_Coord y, Evoak_Coord w, Evoak_Coord h);
   void               evoak_object_image_fill_get         (Evoak_Object *o, Evoak_Coord *x, Evoak_Coord *y, Evoak_Coord *w, Evoak_Coord *h);
   void               evoak_object_image_border_set       (Evoak_Object *o, int l, int r, int t, int b);
   void               evoak_object_image_border_get       (Evoak_Object *o, int *l, int *r, int *t, int *b);
   void               evoak_object_image_smooth_scale_set (Evoak_Object *o, Evoak_Bool onoff);
   Evoak_Bool         evoak_object_image_smooth_scale_get (Evoak_Object *o);
   void               evoak_object_image_size_set         (Evoak_Object *o, int w, int h);
   void               evoak_object_image_size_get         (Evoak_Object *o, int *w, int *h);
   void               evoak_object_image_alpha_set        (Evoak_Object *o, Evoak_Bool onoff);
   Evoak_Bool         evoak_object_image_alpha_get        (Evoak_Object *o);
   /* add dummy calls for data get/set etc. */
   
   Evoak_Object      *evoak_object_line_add               (Evoak *ev);
   void               evoak_object_line_xy_set            (Evoak_Object *o, Evoak_Coord x1, Evoak_Coord y1, Evoak_Coord x2, Evoak_Coord y2);
   void               evoak_object_line_xy_get            (Evoak_Object *o, Evoak_Coord *x1, Evoak_Coord *y1, Evoak_Coord *x2, Evoak_Coord *y2);
   
   Evoak_Object      *evoak_object_polygon_add            (Evoak *ev);
   void               evoak_object_polygon_point_add      (Evoak_Object *o, Evoak_Coord x, Evoak_Coord y);
   void               evoak_object_polygon_points_clear   (Evoak_Object *o);

   Evoak_Object      *evoak_object_gradient_add           (Evoak *ev);
   void               evoak_object_gradient_color_add     (Evoak_Object *o, int r, int g, int b, int a, int distance);
   void               evoak_object_gradient_colors_clear  (Evoak_Object *o);
   void               evoak_object_gradient_angle_set     (Evoak_Object *o, Evoak_Angle angle);
   Evoak_Angle        evoak_object_gradient_angle_get     (Evoak_Object *o);

   Evoak_Object      *evoak_object_text_add               (Evoak *ev);
   void               evoak_object_text_font_source_set   (Evoak_Object *o, const char *source);
   const char        *evoak_object_text_font_source_get   (Evoak_Object *o);
   void               evoak_object_text_font_set          (Evoak_Object *o, const char *font, Evoak_Font_Size size);
   void               evoak_object_text_font_get          (Evoak_Object *o, const char **font, Evoak_Font_Size *size);
   void               evoak_object_text_text_set          (Evoak_Object *o, const char *text);
   const char        *evoak_object_text_text_get          (Evoak_Object *o);
   Evoak_Coord        evoak_object_text_ascent_get        (Evoak_Object *o);
   Evoak_Coord        evoak_object_text_descent_get       (Evoak_Object *o);
   Evoak_Coord        evoak_object_text_max_ascent_get    (Evoak_Object *o);
   Evoak_Coord        evoak_object_text_max_descent_get   (Evoak_Object *o);
   Evoak_Coord        evoak_object_text_horiz_advance_get (Evoak_Object *o);
   Evoak_Coord        evoak_object_text_vert_advance_get  (Evoak_Object *o);
   Evoak_Coord        evoak_object_text_inset_get         (Evoak_Object *o);
   int                evoak_object_text_char_pos_get      (Evoak_Object *o, int pos, Evoak_Coord *cx, Evoak_Coord *cy, Evoak_Coord *cw, Evoak_Coord *ch);
   int                evoak_object_text_char_coords_get   (Evoak_Object *o, Evoak_Coord x, Evoak_Coord y, Evoak_Coord *cx, Evoak_Coord *cy, Evoak_Coord *cw, Evoak_Coord *ch);

   void               evoak_font_path_clear                (Evoak *ev);
   void               evoak_font_path_append               (Evoak *ev, const char *path);
   void               evoak_font_path_prepend              (Evoak *ev, const char *path);
   const Evoak_List  *evoak_font_path_list                 (Evoak *ev);
   
   int                evoak_string_char_next_get           (const char *str, int pos, int *decoded);
   int                evoak_string_char_prev_get           (const char *str, int pos, int *decoded);
   
   Evoak_Object      *evoak_object_edje_add                        (Evoak *ev);
   int                evoak_object_edje_file_set                   (Evoak_Object *o, const char *file, const char *part);
   void               evoak_object_edje_file_get                   (Evoak_Object *o, const char **file, const char **part);
/* int                evoak_object_edje_load_error_get             (Evoak_Object *o);*/
   const char        *evoak_object_edje_data_get                   (Evoak_Object *o, const char *key);
   void               evoak_object_edje_signal_callback_add        (Evoak_Object *o, const char *emission, const char *source, void (*func) (void *data, Evoak_Object *o, const char *emission, const char *source), void *data);
   void              *evoak_object_edje_signal_callback_del        (Evoak_Object *o, const char *emission, const char *source, void (*func) (void *data, Evoak_Object *o, const char *emission, const char *source));
   void               evoak_object_edje_signal_emit                (Evoak_Object *o, const char *emission, const char *source);
   int                evoak_object_edje_part_exists                (Evoak_Object *o, const char *part);
   void               evoak_object_edje_part_swallow               (Evoak_Object *o, const char *part, Evoak_Object *o_swallow);
   void               evoak_object_edje_part_unswallow             (Evoak_Object *o, Evoak_Object *o_swallow);
   Evoak_Object      *evoak_object_edje_part_swallow_get           (Evoak_Object *o, const char *part);
   /* FIXME: implement edje callbacks for when text is changed */
   void               evoak_object_edje_part_text_set              (Evoak_Object *o, const char *part, const char *text);
   const char        *evoak_object_edje_part_text_get              (Evoak_Object *o, const char *part);
   
   /* FIXME: oh god this is going to be FUN! */
/*   
   void               evoak_object_edje_color_class_set            (Evoak_Object *o, const char *color_class, int r, int g, int b, int a, int r2, int g2, int b2, int a2, int r3, int g3, int b3, int a3);
   void               evoak_object_edje_text_class_set             (Evoak_Object *o, const char *text_class, const char *font, Evoak_Font_Size size);
   void               evoak_object_edje_size_min_get               (Evoak_Object *o, Evoak_Coord *minw, Evoak_Coord *minh);
   void               evoak_object_edje_size_max_get               (Evoak_Object *o, Evoak_Coord *maxw, Evoak_Coord *maxh);
 */
   /* FIXME: this will need to implement the entire edje calulation engine client-side - i think not. */
/*   
   void               evoak_object_edje_size_min_calc              (Evoak_Object *o, Evoak_Coord *minw, Evoak_Coord *minh);
 */
   /* FIXME: will need more protocol here and have ways of monitoring drag val */
/*   
   const char        *evoak_object_edje_part_state_get             (Evoak_Object *o, const char *part, double *val_ret);
   int                evoak_object_edje_part_drag_dir_get          (Evoak_Object *o, const char *part);
   void               evoak_object_edje_part_drag_value_set        (Evoak_Object *o, const char *part, double dx, double dy);
   void               evoak_object_edje_part_drag_value_get        (Evoak_Object *o, const char *part, double *dx, double *dy);
   void               evoak_object_edje_part_drag_size_set         (Evoak_Object *o, const char *part, double dw, double dh);
   void               evoak_object_edje_part_drag_size_get         (Evoak_Object *o, const char *part, double *dw, double *dh);
   void               evoak_object_edje_part_drag_step_set         (Evoak_Object *o, const char *part, double dx, double dy);
   void               evoak_object_edje_part_drag_step_get         (Evoak_Object *o, const char *part, double *dx, double *dy);
   void               evoak_object_edje_part_drag_page_set         (Evoak_Object *o, const char *part, double dx, double dy);
   void               evoak_object_edje_part_drag_page_get         (Evoak_Object *o, const char *part, double *dx, double *dy);
   void               evoak_object_edje_part_drag_step             (Evoak_Object *o, const char *part, double dx, double dy);
   void               evoak_object_edje_part_drag_page             (Evoak_Object *o, const char *part, double dx, double dy);
 */
   /* FIXME: general policy color and text classes - easily done. more protocol needed */
/*   
   void               edje_color_class_set(const char *color_class, int r, int g, int b, int a, int r2, int g2, int b2, int a2, int r3, int g3, int b3, int a3);
   void               edje_text_class_set(const char *text_class, const char *font, Evas_Font_Size size);
*/
   /* FIXME: this should really be general min/max size hints on ALL objects, not just edje. implement both set AND get */
/*   
   void               edje_extern_object_min_size_set (Evas_Object *o, Evas_Coord minw, Evas_Coord minh);
   void               edje_extern_object_max_size_set (Evas_Object *o, Evas_Coord maxw, Evas_Coord maxh);
*/ 

   /* probably wont implement this... but who knows */
/*   
   void               evoak_object_edje_play_set                   (Evoak_Object *o, int play);
   int                evoak_object_edje_play_get                   (Evoak_Object *o);
   void               evoak_object_edje_animation_set              (Evoak_Object *o, int on);
   int                evoak_object_edje_animation_get              (Evoak_Object *o);
   // definitely dont need to implement this
   //int                evoak_object_edje_freeze                     (Evoak_Object *o);
   //int                evoak_object_edje_thaw                       (Evoak_Object *o);
 * // very unlikely to do this one
   void               evoak_object_edje_part_geometry_get          (Evoak_Object *o, const char *part, Evoak_Coord *x, Evoak_Coord *y, Evoak_Coord *w, Evoak_Coord *h);
*/ 
   /*
    * FIXME: add object min/max size setting and protocol to send to server
    * FIXME: add load_error support for images
    * FIXME: add client add/del events, and selecting for them
    * FIXME: add "delete a client" api
    * FIXME: add client focus api (set focus, gain/lose focus events)
    * FIXME: add client action requests (hide, show, lower, raise, go to layer)
    * FIXME: add object groups for move/resize/stack/del
    * FIXME: add manager clips/group objects
    * FIXME: add smart objects
    * FIXME: add input method support
    * FIXME: add edje objects
    * FIXME: add image data upload/change
    * FIXME: add shm transport
    * FIXME: add image/font/edje etc. file uploads & server cache
    * FIXME: add font listing api
    * FIXME: add unfocus all
    * FIXME: add focused object(s) get
    * FIXME: add key grabs that work regardless of object or client focus
    * Evoak_Bool         evoak_object_key_grab               (Evoak_Object *o, const char *keyname, Evoak_Modifier modifiers, Evoak_Modifier not_modifiers, Evoak_Bool exclusive);
    * void               evoak_object_key_ungrab             (Evoak_Object *o, const char *keyname, Evoak_Modifier modifiers, Evoak_Modifier not_modifiers);
    * FIXME: add server-side smart objects via embryo methods for their contents
    */
   
/***************************************************************************/
#ifdef __cplusplus
}
#endif

#endif
