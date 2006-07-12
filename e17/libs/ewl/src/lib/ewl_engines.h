#ifndef EWL_ENGINES_H
#define EWL_ENGINES_H

#include <Ewl.h>

enum Ewl_Engine_Hook
{
	EWL_ENGINE_INIT,	/**< Initialize the engine */
	EWL_ENGINE_SHUTDOWN,	/**< Shutdown the engine */

	EWL_ENGINE_WINDOW_NEW,	/**< Create the window */
	EWL_ENGINE_WINDOW_DESTROY,/**< Destroy the window */

	EWL_ENGINE_WINDOW_MOVE,	/**< Move the window to 
							the given location */
	EWL_ENGINE_WINDOW_RESIZE,	/**< Resize the window */
	EWL_ENGINE_WINDOW_MIN_MAX_SIZE_SET, /**< Set the min and max sizes */
	EWL_ENGINE_WINDOW_SHOW,	/**< Show the window */
	EWL_ENGINE_WINDOW_HIDE,	/**< Hide the window */

	EWL_ENGINE_WINDOW_TITLE_SET, /**< Set the window title */
	EWL_ENGINE_WINDOW_NAME_CLASS_SET,	/**< Set the window name/class */

	EWL_ENGINE_WINDOW_BORDERLESS_SET,	/**< Set the borderless 
							 state of the window */
	EWL_ENGINE_WINDOW_DIALOG_SET,	/**< Set the dialog 
						       setting of the window */
	EWL_ENGINE_WINDOW_FULLSCREEN_SET,	/**< Set the fullscreen 
							setting of the window */
	EWL_ENGINE_WINDOW_TRANSIENT_FOR,	/**< Set the window transient */
	EWL_ENGINE_WINDOW_MODAL_SET, /**< Set the window modal */

	EWL_ENGINE_WINDOW_RAISE,	/**< Raise the window */
	EWL_ENGINE_WINDOW_LOWER,	/**< Lower the window */

	EWL_ENGINE_WINDOW_KEYBOARD_GRAB,	/**< Set the keyboard grab */
	EWL_ENGINE_WINDOW_KEYBOARD_UNGRAB,	/**< Set the keyboard ungrab */
	EWL_ENGINE_WINDOW_POINTER_GRAB,	/**< Set the pointer grab */
	EWL_ENGINE_WINDOW_POINTER_UNGRAB,	/**< Set the pointer ungrab */

	EWL_ENGINE_WINDOW_SELECTION_TEXT_SET, /**< Set the selection text */
	EWL_ENGINE_WINDOW_GEOMETRY_GET, /**< Get the window geometry */
	EWL_ENGINE_WINDOW_DND_AWARE_SET,	 /**< Set the window 
								dnd aware */
	EWL_ENGINE_CANVAS_SETUP, /**< Setup the render canvas */

	EWL_ENGINE_CANVAS_RENDER,
	EWL_ENGINE_CANVAS_FREEZE,
	EWL_ENGINE_CANVAS_THAW,

	EWL_ENGINE_THEME_FREEZE,
	EWL_ENGINE_THEME_THAW,

	EWL_ENGINE_THEME_DATA_GET,

	EWL_ENGINE_THEME_WIDGET_GROUP,

	EWL_ENGINE_THEME_OBJECT_ADD,
	EWL_ENGINE_THEME_OBJECT_DEL,
	EWL_ENGINE_THEME_OBJECT_MOVE,
	EWL_ENGINE_THEME_OBJECT_RESIZE,
	EWL_ENGINE_THEME_OBJECT_SHOW,
	EWL_ENGINE_THEME_OBJECT_HIDE,
	EWL_ENGINE_THEME_OBJECT_CLIP_SET,
	EWL_ENGINE_THEME_OBJECT_CLIP_UNSET,
	EWL_ENGINE_THEME_OBJECT_FILE_SET,
	EWL_ENGINE_THEME_OBJECT_LOAD_ERROR,
	EWL_ENGINE_THEME_OBJECT_MIN_SIZE_GET,
	EWL_ENGINE_THEME_OBJECT_MAX_SIZE_GET,
	EWL_ENGINE_THEME_OBJECT_SIGNAL_SEND,
	EWL_ENGINE_THEME_OBJECT_PART_TEXT_SET,
	EWL_ENGINE_THEME_OBJECT_COLOR_CLASS_SET,

	EWL_ENGINE_THEME_CLIP_ADD,
	EWL_ENGINE_THEME_CLIP_DEL,
	EWL_ENGINE_THEME_CLIP_CLIPEES_GET,
	EWL_ENGINE_THEME_CLIP_COLOR_SET,

	EWL_ENGINE_THEME_WIDGET_STACK_ADD,
	EWL_ENGINE_THEME_WIDGET_LAYER_UPDATE,
	EWL_ENGINE_MAX,
};

typedef enum Ewl_Engine_Hook Ewl_Engine_Hook;

#define EWL_ENGINE(engine) ((Ewl_Engine *)engine)
typedef struct Ewl_Engine Ewl_Engine;

typedef struct Ewl_Engine_Info Ewl_Engine_Info;
struct Ewl_Engine_Info
{
	void *engine_hooks[EWL_ENGINE_MAX];	/**< The engine hooks */
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

void            *ewl_engine_hook_get(Ewl_Embed *embed, Ewl_Engine_Hook type);
Ecore_List      *ewl_engine_hook_chain_get(Ewl_Embed *embed, Ewl_Engine_Hook type);

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
void		 ewl_engine_canvas_render(Ewl_Embed *embed);
void		 ewl_engine_canvas_freeze(Ewl_Embed *embed);
void		 ewl_engine_canvas_thaw(Ewl_Embed *embed);

/**
 * Internal engine hook typedefs, you only need these if you're writing an
 * engine.
 */

typedef int (*Ewl_Engine_Cb_Init)(Ewl_Engine *engine);	/**< Initialize the engine */
typedef void (*Ewl_Engine_Cb_Shutdown)(Ewl_Engine *engine);	/**< Shutdown the engine */
typedef void (*Ewl_Engine_Cb_Window_New)(Ewl_Window *win);	/**< Create the window */
typedef void (*Ewl_Engine_Cb_Window_Destroy)(Ewl_Window *win);/**< Destroy the window */
typedef void (*Ewl_Engine_Cb_Window_Move)(Ewl_Window *win);	/**< Move the window to 
							the given location */
typedef void (*Ewl_Engine_Cb_Window_Resize)(Ewl_Window *win);	/**< Resize the window */
typedef void (*Ewl_Engine_Cb_Window_Min_Max_Size_Set)(Ewl_Window *win); /**< Set the min 
							       and max sizes */
typedef void (*Ewl_Engine_Cb_Window_Show)(Ewl_Window *win);	/**< Show the window */
typedef void (*Ewl_Engine_Cb_Window_Hide)(Ewl_Window *win);	/**< Hide the window */
typedef void (*Ewl_Engine_Cb_Window_Title_Set)(Ewl_Window *win); /**< Set the window title */
typedef void (*Ewl_Engine_Cb_Window_Name_Class_Set)(Ewl_Window *win);	/**< Set the window 
								name/class */
typedef void (*Ewl_Engine_Cb_Window_Borderless_Set)(Ewl_Window *win);	/**< Set the borderless 
							 state of the window */
typedef void (*Ewl_Engine_Cb_Window_Dialog_Set)(Ewl_Window *win);	/**< Set the dialog 
						       setting of the window */
typedef void (*Ewl_Engine_Cb_Window_Fullscreen_Set)(Ewl_Window *win);	/**< Set the fullscreen 
							setting of the window */
typedef void (*Ewl_Engine_Cb_Window_Transient_For)(Ewl_Window *win);	/**< Set the window 
								transient */
typedef void (*Ewl_Engine_Cb_Window_Modal_Set)(Ewl_Window *win); /**< Set the window modal */
typedef void (*Ewl_Engine_Cb_Window_Raise)(Ewl_Window *win);	/**< Raise the window */
typedef void (*Ewl_Engine_Cb_Window_Lower)(Ewl_Window *win);	/**< Lower the window */
typedef int  (*Ewl_Engine_Cb_Keyboard_Grab)(Ewl_Window *win);	/**< Set the keyboard grab */
typedef void  (*Ewl_Engine_Cb_Keyboard_Ungrab)(Ewl_Window *win); /**< Set the keyboard 
							ungrab */
typedef int  (*Ewl_Engine_Cb_Pointer_Grab)(Ewl_Window *win);	/**< Set the pointer grab */
typedef void  (*Ewl_Engine_Cb_Pointer_Ungrab)(Ewl_Window *win); /**< Set the pointer ungrab */
typedef void (*Ewl_Engine_Cb_Window_Selection_Text_Set)(Ewl_Window *win, 
							const char *txt); /**< Set the 
									   selection text */
typedef void (*Ewl_Engine_Cb_Window_Geometry_Get)(Ewl_Window *win, 
						int *width, int *height); /**< Get the window 
								geometry */
typedef void (*Ewl_Engine_Cb_Window_Dnd_Aware_Set)(Ewl_Window *win);	 /**< Set the window 
								dnd aware */
typedef void (*Ewl_Engine_Cb_Canvas_Setup)(Ewl_Window *win, int debug); /**< Setup the 
						         render canvas */
typedef void  (*Ewl_Engine_Cb_Canvas_Render)(Ewl_Embed *embed);
typedef void  (*Ewl_Engine_Cb_Canvas_Freeze)(Ewl_Embed *embed);
typedef void  (*Ewl_Engine_Cb_Canvas_Thaw)(Ewl_Embed *embed);
typedef void  (*Ewl_Engine_Cb_Theme_Freeze)();
typedef void  (*Ewl_Engine_Cb_Theme_Thaw)();
typedef char *(*Ewl_Engine_Cb_Theme_Data_Get)(char *path, char *key);
typedef void *(*Ewl_Engine_Cb_Theme_Widget_Group)(Ewl_Widget *w);
typedef void *(*Ewl_Engine_Cb_Theme_Object_Add)(Ewl_Embed *embed);
typedef void  (*Ewl_Engine_Cb_Theme_Object_Del)(void *obj);
typedef void  (*Ewl_Engine_Cb_Theme_Object_Move)(void *obj, int x, int y);
typedef void  (*Ewl_Engine_Cb_Theme_Object_Resize)(void *obj, int w, int h);
typedef void *(*Ewl_Engine_Cb_Theme_Object_Show)(void *obj, void *clip);
typedef void *(*Ewl_Engine_Cb_Theme_Object_Hide)(void *obj, void *clip);
typedef void *(*Ewl_Engine_Cb_Theme_Object_Clip_Set)(void *obj, void *clip);
typedef void *(*Ewl_Engine_Cb_Theme_Object_Clip_Unset)(void *obj);
typedef void (*Ewl_Engine_Cb_Theme_Object_Stack_Add)(Ewl_Widget *w);
typedef void (*Ewl_Engine_Cb_Theme_Object_Layer_Update)(Ewl_Widget *w);
typedef void  (*Ewl_Engine_Cb_Theme_Object_File_Set)(void *obj, char *path, 
								char *group);
typedef void *(*Ewl_Engine_Cb_Theme_Object_Load_Error)(void *obj);
typedef void  (*Ewl_Engine_Cb_Theme_Object_Min_Size_Get)(void *obj, int *w, int *h);
typedef void  (*Ewl_Engine_Cb_Theme_Object_Max_Size_Get)(void *obj, int *w, int *h);
typedef void  (*Ewl_Engine_Cb_Theme_Object_Signal_Send)(void *obj, 
							char *state, char *source);
typedef void  (*Ewl_Engine_Cb_Theme_Object_Part_Text_Set)(void *obj, 
							char *part, char *text);
typedef void  (*Ewl_Engine_Cb_Theme_Object_Color_Class_Set)(char *name, int r, 
							int g, int b, int r2, 
							int g2, int b2, int r3, 
							int g3, int b3);
typedef void *(*Ewl_Engine_Cb_Theme_Clip_Add)(Ewl_Embed *embed);
typedef void  (*Ewl_Engine_Cb_Theme_Clip_Del)(void *clip);
typedef void *(*Ewl_Engine_Cb_Theme_Clip_Clipees_Get)(void *clip);
typedef void  (*Ewl_Engine_Cb_Theme_Clip_Color_Set)(void *clip, int r, int g, 
								int b, int a);

#endif

