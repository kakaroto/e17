#ifndef __EWL_ICONBOX_H__
#define __EWL_ICONBOX_H__

#define EWL_ICONBOX_ICON_PADDING 15
#define EWL_ICONBOX_MOVE_TOLERANCE 5
#define EWL_ICONBOX_MINIMUM_SIZE 50
#define LABEL_CHARS 10

#include <sys/time.h>
#include <time.h>

/**
 * @file ewl_iconbox.h
 * @defgroup Ewl_Iconbox Iconbox: The Icon Box Layout Container
 * @brief Defines the Ewl_Iconbox class, used to display a collection of icons 
 *  	  with annotations and support arranging/manipulating contained icons
 *
 */

/** 
 * @themekey /iconbox/file
 * @themekey /iconbox/group
 */

typedef struct Ewl_Iconbox Ewl_Iconbox;

typedef struct Ewl_Iconbox_Icon Ewl_Iconbox_Icon;
struct Ewl_Iconbox_Icon
{
	Ewl_Box box;
	
	Ewl_Widget *image;
	Ewl_Widget *w_label;

	int drag;
	int selected ;

	Ewl_Iconbox *icon_box_parent; /* Our reference upwards, kinda breaks OO conventions */
	char *label;		      /*Our icon's full (unabbreviated) label) */
	char *label_compressed;
};

/**
 * @def EWL_ICONBOX(iconbox)
 * Typecast a pointer to an Ewl_Iconbox pointer
 */
#define EWL_ICONBOX(iconbox) ((Ewl_Iconbox *) iconbox)
#define EWL_ICONBOX_ICON(icon) ((Ewl_Iconbox_Icon *) icon)

/** 
 * @struct Ewl_Iconbox
 * Inherits from an Ewl_Container to provide layout facilities for child widgets placed inside
 * Layout is either free-form, or auto-arranged to a grid.
 */
struct Ewl_Iconbox
{
	Ewl_Box	box; /**< Inherit from Ewl_Container */

	Ewl_Widget *ewl_iconbox_scrollpane;
	Ewl_Widget *ewl_iconbox_pane_inner;

	Ewl_Widget *ewl_iconbox_context_menu;
	Ewl_Widget *ewl_iconbox_context_menu_item;
	Ewl_Widget *ewl_iconbox_menu_floater;
	Ewl_Widget *ewl_iconbox_menu_box;
	Ewl_Widget *ewl_iconbox_view_menu;

	Ewl_Widget *icon_menu_floater;
	Ewl_Widget *icon_menu_item;
	Ewl_Widget *icon_menu;


	Ecore_List *ewl_iconbox_icon_list;  /* The icon list */
	char *test;

	Ewl_Iconbox_Icon *drag_icon;
	Ewl_Iconbox_Icon *edit_icon;
	Ewl_Iconbox_Icon *select_icon;

	int xdown, ydown;		   /* Last icon button down x/y, for edge resistence */
	int dx, dy;			   /* Drag start x/y */
	int lx, ly;			   /* Layout x, layout y */
	int ox, oy;			   /* Layout x offset, layout y offset */
	int iw, ih;			   /* Custom icon width/height*/

	/*Background*/
	Ewl_Widget *background;

	/* Objects for label editing..*/
	Ewl_Widget *entry;
	Ewl_Widget *entry_floater;
	Ewl_Widget *entry_box;

	/* Objects for selection */
	Ewl_Widget *select;
	Ewl_Widget *select_floater;
	int drag_box;			  /* Are we dragging? */

	/* Key event/repeat timer */
	struct timeval lasttime;
	void (*key_event_cb)(Ewl_Iconbox *ib, void *data, char *keyname);
	void *key_event_data;

	/* Editable flag */
	int editable;
};


Ewl_Widget	*ewl_iconbox_new(void);
Ewl_Widget	*ewl_iconbox_icon_new(void);
int		 ewl_iconbox_init(Ewl_Iconbox *iconbox);
int		 ewl_iconbox_icon_init(Ewl_Iconbox_Icon *icon);


/* External functions */
void 		 ewl_iconbox_icon_size_custom_set(Ewl_Iconbox *ib, int w, int h);
void 		 ewl_iconbox_editable_set(Ewl_Iconbox *ib, int edit);
void 		 ewl_iconbox_icon_arrange(Ewl_Iconbox *ib);
void 		 ewl_iconbox_deselect_all(Ewl_Iconbox *ib);
Ewl_Iconbox_Icon*ewl_iconbox_icon_add(Ewl_Iconbox *iconbox, const char *name, 
								const char *icon_file);
void 		 ewl_iconbox_scrollpane_goto_root(Ewl_Iconbox *ib);
void 		 ewl_iconbox_clear(Ewl_Iconbox *iconbox);
void 		 ewl_iconbox_icon_image_set(Ewl_Iconbox_Icon *icon, const char *filename);
void 		 ewl_iconbox_background_set(Ewl_Iconbox *ib, const char *file);
void 		 ewl_iconbox_context_menu_item_add(Ewl_Iconbox *ib, Ewl_Widget *item);
void 		 ewl_iconbox_icon_menu_item_add(Ewl_Iconbox *ib, Ewl_Widget *item);
void 		 ewl_iconbox_icon_remove(Ewl_Iconbox *ib, Ewl_Iconbox_Icon *icon);
void  		 ewl_iconbox_scrollpane_recalculate(Ewl_Iconbox *ib);
void 		 ewl_iconbox_controlled_key_callback_register(Ewl_Iconbox *ib, 
					void (*cb)(Ewl_Iconbox *, void *, char *), 
					void *data);
Ecore_List	*ewl_iconbox_get_selection(Ewl_Iconbox *ib);

/* Internal callbacks */
void ewl_iconbox_dnd_drop_cb(Ewl_Widget *item, void *ev_data, void *user_data);
void ewl_iconbox_dnd_position_cb(Ewl_Widget *item, void *ev_data, void *user_data);
void ewl_iconbox_arrange_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_expansion_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_mouse_move_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_pane_mouse_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_mouse_up(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_icon_mouse_up(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_icon_mouse_down(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_icon_label_mouse_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_icon_destroy_cb(Ewl_Widget *w, void *ev_data ,void *user_data );
void ewl_iconbox_key_press_cb (Ewl_Widget *w, void *ev_data, void *user_data);

#endif

