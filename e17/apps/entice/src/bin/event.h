#ifndef __EVENT_H__
#define __EVENT_H__

/* callbacks for evas handling */
/* when the event queue goes idle call this */
void e_idle(void *data);
/* when the window gets exposed call this */
void e_window_expose(Ecore_Event * ev);
/* when the mouse moves in the window call this */
void e_mouse_move(Ecore_Event * ev);
/* when a mouse button goes down in the window call this */
void e_mouse_down(Ecore_Event * ev);
/* when a mouse button is released in the window call this */
void e_mouse_up(Ecore_Event * ev);
/* when the window gets moved or resized */
void e_window_configure(Ecore_Event * ev);
/* when a key gets pressed */
void e_key_down(Ecore_Event * ev);
void e_property(Ecore_Event * ev);

#endif /* __EVENT_H__ */
