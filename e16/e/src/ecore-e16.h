#ifdef USE_ECORE_X
#include <Ecore_X.h>

#else

#define Ecore_X_GC       GC
#define Ecore_X_Drawable Drawable
#define Ecore_X_Window   Window
#define Ecore_X_Pixmap   Pixmap
#define Ecore_X_Atom     Atom
#define Ecore_X_Time     Time

#define _ecore_x_disp disp

#define ecore_x_init(dstr) \
	disp = XOpenDisplay(dstr)
#define ecore_x_shutdown() \
	XCloseDisplay(disp)
#define ecore_x_display_get() \
	disp

#define ecore_x_sync() \
	XSync(disp, False)

#define ecore_x_window_move(win, x, y) \
	XMoveWindow(disp, win, x, y)
#define ecore_x_window_resize(win, w, h) \
	XResizeWindow(disp, win, w, h)
#define ecore_x_window_move_resize(win, x, y, w, h) \
	XMoveResizeWindow(disp, win, x, y, w, h)

#define ecore_x_pixmap_new(draw, w, h, dep) \
	XCreatePixmap(disp, draw, w, h, dep)
#define ecore_x_pixmap_del(pmap) \
	XFreePixmap(disp, pmap)

#define ecore_x_gc_new(draw) \
	XCreateGC(disp, draw, 0, NULL);
#define ecore_x_gc_del(gc) \
	XFreeGC(disp, gc)

void                ecore_x_grab(void);
void                ecore_x_ungrab(void);

int                 ecore_x_client_message32_send(Ecore_X_Window win,
						  Ecore_X_Atom type,
						  long mask,
						  long d0, long d1, long d2,
						  long d3, long d4);

void                ecore_x_window_prop_card32_set(Ecore_X_Window win,
						   Ecore_X_Atom atom,
						   unsigned int *val,
						   unsigned int num);
int                 ecore_x_window_prop_card32_get(Ecore_X_Window win,
						   Ecore_X_Atom atom,
						   unsigned int *val,
						   unsigned int len);

void                ecore_x_icccm_delete_window_send(Ecore_X_Window win,
						     Ecore_X_Time ts);
void                ecore_x_icccm_take_focus_send(Ecore_X_Window win,
						  Ecore_X_Time ts);

/* Misc. */
extern Atom         ECORE_X_ATOM_UTF8_STRING;

/* ICCCM */
extern Atom         ECORE_X_ATOM_WM_STATE;
extern Atom         ECORE_X_ATOM_WM_PROTOCOLS;
extern Atom         ECORE_X_ATOM_WM_DELETE_WINDOW;
extern Atom         ECORE_X_ATOM_WM_TAKE_FOCUS;

#if 0
extern Atom         ECORE_X_ATOM_WM_SAVE_YOURSELF;
#endif

void                ecore_x_icccm_init(void);

/* NETWM (EWMH) */
extern Atom         ECORE_X_ATOM_NET_SUPPORTED;
extern Atom         ECORE_X_ATOM_NET_SUPPORTING_WM_CHECK;

extern Atom         ECORE_X_ATOM_NET_NUMBER_OF_DESKTOPS;
extern Atom         ECORE_X_ATOM_NET_VIRTUAL_ROOTS;
extern Atom         ECORE_X_ATOM_NET_DESKTOP_NAMES;
extern Atom         ECORE_X_ATOM_NET_DESKTOP_GEOMETRY;
extern Atom         ECORE_X_ATOM_NET_WORKAREA;
extern Atom         ECORE_X_ATOM_NET_CURRENT_DESKTOP;
extern Atom         ECORE_X_ATOM_NET_DESKTOP_VIEWPORT;
extern Atom         ECORE_X_ATOM_NET_SHOWING_DESKTOP;

extern Atom         ECORE_X_ATOM_NET_SHOWING_DESKTOP;

extern Atom         ECORE_X_ATOM_NET_WM_NAME;

extern Atom         ECORE_X_ATOM_NET_WM_WINDOW_OPACITY;

void                ecore_x_netwm_init(void);

void                ecore_x_netwm_wm_identify(Ecore_X_Window root,
					      Ecore_X_Window check,
					      const char *wm_name);

void                ecore_x_netwm_desk_count_set(Ecore_X_Window root,
						 unsigned int n_desks);
void                ecore_x_netwm_desk_roots_set(Ecore_X_Window root,
						 unsigned int n_desks,
						 Ecore_X_Window * vroots);
void                ecore_x_netwm_desk_names_set(Ecore_X_Window root,
						 unsigned int n_desks,
						 const char **names);
void                ecore_x_netwm_desk_size_set(Ecore_X_Window root,
						unsigned int width,
						unsigned int height);
void                ecore_x_netwm_desk_workareas_set(Ecore_X_Window root,
						     unsigned int n_desks,
						     unsigned int *areas);
void                ecore_x_netwm_desk_current_set(Ecore_X_Window root,
						   unsigned int desk);
void                ecore_x_netwm_desk_viewports_set(Ecore_X_Window root,
						     unsigned int n_desks,
						     unsigned int *origins);
void                ecore_x_netwm_showing_desktop_set(Ecore_X_Window root,
						      int on);

void                ecore_x_netwm_client_list_set(Ecore_X_Window root,
						  unsigned int n_clients,
						  Ecore_X_Window * p_clients);
void                ecore_x_netwm_client_list_stacking_set(Ecore_X_Window root,
							   unsigned int
							   n_clients,
							   Ecore_X_Window *
							   p_clients);
void                ecore_x_netwm_client_active_set(Ecore_X_Window root,
						    Ecore_X_Window win);
void                ecore_x_netwm_name_set(Ecore_X_Window win,
					   const char *name);
char               *ecore_x_netwm_name_get(Ecore_X_Window win);
void                ecore_x_netwm_icon_name_set(Ecore_X_Window win,
						const char *name);
char               *ecore_x_netwm_icon_name_get(Ecore_X_Window win);
void                ecore_x_netwm_visible_name_set(Ecore_X_Window win,
						   const char *name);
char               *ecore_x_netwm_visible_name_get(Ecore_X_Window win);
void                ecore_x_netwm_visible_icon_name_set(Ecore_X_Window win,
							const char *name);
char               *ecore_x_netwm_visible_icon_name_get(Ecore_X_Window win);

void                ecore_x_netwm_desktop_set(Ecore_X_Window win,
					      unsigned int desk);
int                 ecore_x_netwm_desktop_get(Ecore_X_Window win,
					      unsigned int *desk);
void                ecore_x_netwm_opacity_set(Ecore_X_Window win,
					      unsigned int opacity);
int                 ecore_x_netwm_opacity_get(Ecore_X_Window win,
					      unsigned int *opacity);

#endif

void                ecore_x_icccm_state_set_iconic(Ecore_X_Window win);
void                ecore_x_icccm_state_set_normal(Ecore_X_Window win);
void                ecore_x_icccm_state_set_withdrawn(Ecore_X_Window win);
