#ifndef __EVENT_H__
#define __EVENT_H__

/* callbacks for evas handling */
/* when the event queue goes idle call this */
int                e_idle(void *data);

/* when the window gets exposed call this */
int                e_window_damage(void* data, int ev_type, Ecore_Event *ev);

/* when the mouse moves in the window call this */
int                e_mouse_move(void* data, int ev_type, Ecore_Event *ev);

/* when a mouse button goes down in the window call this */
int                e_mouse_down(void* data, int ev_type, Ecore_Event *ev);

/* when a mouse button is released in the window call this */
int                e_mouse_up(void* data, int ev_type, Ecore_Event *ev);

void		   e_window_resize(Ecore_Evas *ee);
/* when the window gets moved or resized */
void		   e_window_delete(Ecore_Evas *ee);
/* when they click the little X in the corner */
//int                e_window_configure(void* data, int ev_type, Ecore_Event *ev);

/* when a key gets pressed */
void               e_key_down(void* data, Evas *e, Evas_Object *ev, void *event_info);
void               e_key_up(void* data, Evas *e, Evas_Object *ev, void *event_info);
//int                e_property(void* data, int ev_type, Ecore_Event *ev);

#endif /* __EVENT_H__ */
