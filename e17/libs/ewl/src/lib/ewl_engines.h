#ifndef EWL_ENGINES_H
#define EWL_ENGINES_H

#include <Ewl.h>

#define EWL_ENGINE(engine) ((Ewl_Engine *)engine)
typedef struct Ewl_Engine Ewl_Engine;

typedef struct Ewl_Engine_Info Ewl_Engine_Info;
struct Ewl_Engine_Info
{
	int  (*init)(Ewl_Engine *engine);	/**< Initialize the engine */
	void (*shutdown)(Ewl_Engine *engine);	/**< Shutdown the engine */

	void (*window_new)(Ewl_Window *win);	/**< Create the window */
	void (*window_destroy)(Ewl_Window *win);/**< Destroy the window */

	void (*window_move)(Ewl_Window *win);	/**< Move the window to 
							the given location */
	void (*window_resize)(Ewl_Window *win);	/**< Resize the window */
	void (*window_min_max_size_set)(Ewl_Window *win); /**< Set the min 
							       and max sizes */
	void (*window_show)(Ewl_Window *win);	/**< Show the window */
	void (*window_hide)(Ewl_Window *win);	/**< Hide the window */

	void (*window_title_set)(Ewl_Window *win); /**< Set the window title */
	void (*window_name_class_set)(Ewl_Window *win);	/**< Set the window 
								name/class */

	void (*window_borderless_set)(Ewl_Window *win);	/**< Set the borderless 
							 state of the window */
	void (*window_dialog_set)(Ewl_Window *win);	/**< Set the dialog 
						       setting of the window */
	void (*window_fullscreen_set)(Ewl_Window *win);	/**< Set the fullscreen 
							setting of the window */
	void (*window_transient_for)(Ewl_Window *win);	/**< Set the window 
								transient */
	void (*window_modal_set)(Ewl_Window *win); /**< Set the window modal */

	void (*window_raise)(Ewl_Window *win);	/**< Raise the window */
	void (*window_lower)(Ewl_Window *win);	/**< Lower the window */

	int  (*keyboard_grab)(Ewl_Window *win);	/**< Set the keyboard grab */
	void  (*keyboard_ungrab)(Ewl_Window *win); /**< Set the keyboard 
							ungrab */
	int  (*pointer_grab)(Ewl_Window *win);	/**< Set the pointer grab */
	void  (*pointer_ungrab)(Ewl_Window *win); /**< Set the pointer 
							ungrab */

	void (*window_selection_text_set)(Ewl_Window *win, 
					const char *txt); /**< Set the 
							   selection text */
	void (*window_geometry_get)(Ewl_Window *win, 
				int *width, int *height); /**< Get the window 
								geometry */
	void (*window_dnd_aware_set)(Ewl_Window *win);	 /**< Set the window 
								dnd aware */

	void (*canvas_setup)(Ewl_Window *win, int debug); /**< Setup the 
						         render canvas */
};

struct Ewl_Engine
{
	void *handle;			/**< The dlsym handle */
	char *name;			/**< The name of the engine */

	Ecore_DList *dependancies;	/**< The engines this one depends on */
	Ewl_Engine_Info *functions;	/**< The functions used by ewl, 
						suppled by the engine */
};

int 		 ewl_engines_init(void);
void 		 ewl_engines_shutdown(void);

Ecore_List	*ewl_engine_names_get(void);

Ewl_Engine	*ewl_engine_new(const char *name);

void		 ewl_engine_event_handlers_init(void);
void		 ewl_engine_event_handlers_shutdown(void);

void		 ewl_engine_engine_shutdown(Ewl_Window *win);

void		 ewl_engine_window_new(Ewl_Window *win);
void		 ewl_engine_window_destroy(Ewl_Window *win);

void		 ewl_engine_window_move(Ewl_Window *win);
void		 ewl_engine_window_resize(Ewl_Window *win);
void		 ewl_engine_window_min_max_size_set(Ewl_Window *win);
void		 ewl_engine_window_show(Ewl_Window *win);
void		 ewl_engine_window_hide(Ewl_Window *win);

void		 ewl_engine_window_title_set(Ewl_Window *win);
void		 ewl_engine_window_name_class_set(Ewl_Window *win);

void		 ewl_engine_window_borderless_set(Ewl_Window *win);
void		 ewl_engine_window_dialog_set(Ewl_Window *win);
void		 ewl_engine_window_fullscreen_set(Ewl_Window *win);
void		 ewl_engine_window_transient_for(Ewl_Window *win);
void		 ewl_engine_window_modal_set(Ewl_Window *win);

void		 ewl_engine_window_raise(Ewl_Window *win);
void		 ewl_engine_window_lower(Ewl_Window *win);

int		 ewl_engine_keyboard_grab(Ewl_Window *win);
void		 ewl_engine_keyboard_ungrab(Ewl_Window *win);
int 		 ewl_engine_pointer_grab(Ewl_Window *win);
void 		 ewl_engine_pointer_ungrab(Ewl_Window *win);

void		 ewl_engine_window_selection_text_set(Ewl_Window *win, 
							const char *txt);
void		 ewl_engine_window_geometry_get(Ewl_Window *win, int root,
						int *width, int *height);
void		 ewl_engine_window_dnd_aware_set(Ewl_Window *win);

void		 ewl_engine_canvas_setup(Ewl_Window *win, int debug);

#endif

