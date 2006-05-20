#ifndef EWL_ICONBOX_H
#define EWL_ICONBOX_H

#define EWL_ICONBOX_ICON_PADDING 15
#define EWL_ICONBOX_MOVE_TOLERANCE 5
#define EWL_ICONBOX_MINIMUM_SIZE 50
#define LABEL_CHARS 10

#include <sys/time.h>
#include <time.h>

/**
 * @addtogroup Ewl_Iconbox Ewl_Iconbox: The Icon Box Layout Container
 * Defines the Ewl_Iconbox class, used to display a collection of icons 
 * with annotations and support arranging/manipulating contained icons
 *
 * @{
 */

/** 
 * @themekey /iconbox/file
 * @themekey /iconbox/group
 */

/**
 * @def EWL_ICONBOX_TYPE
 * The type name for the Ewl_Iconbox widget
 */
#define EWL_ICONBOX_TYPE "iconbox"

/**
 * The Ewl_Iconbox
 */
typedef struct Ewl_Iconbox Ewl_Iconbox;

/**
 * The Ewl_Iconbox_Icon
 */
typedef struct Ewl_Iconbox_Icon Ewl_Iconbox_Icon;

/**
 * Inherits from Ewl_Box and extends to provide an icon layout 
 */
struct Ewl_Iconbox_Icon
{
	Ewl_Box box;		      /**< Inherit from Ewl_Box */
	
	Ewl_Widget *image;	      /**< Image widget */
	Ewl_Widget *w_label;	      /**< Label widget */

	int drag;		      /**< Is the icon being dragged */
	int selected;		      /**< Is the icon selected */

	Ewl_Iconbox *icon_box_parent; /**< reference to our parent */
	char *label;		      /**< the icons full label) */
	char *label_compressed;	      /**< Shortend label */

	int ox,			      /**< icon x position, if user has moved */
	    oy;		      	      /**< icon y position, if user has moved */
};

/**
 * @def EWL_ICONBOX(iconbox)
 * Typecast a pointer to an Ewl_Iconbox pointer
 */
#define EWL_ICONBOX(iconbox) ((Ewl_Iconbox *) iconbox)

/**
 * @def EWL_ICONBOX_ICON(icon)
 * Typecasts a pointer to an Ewl_Iconbox_Icon pointer
 */
#define EWL_ICONBOX_ICON(icon) ((Ewl_Iconbox_Icon *) icon)

/** 
 * Inherits from an Ewl_Box to provide layout facilities for child widgets 
 * placed inside. Layout is either free-form, or auto-arranged to a grid.
 */
struct Ewl_Iconbox
{
	Ewl_Box	box; 				/**< Inherit from Ewl_Box */

	Ewl_Widget *ewl_iconbox_scrollpane;	/**< The scrollpane */
	Ewl_Widget *ewl_iconbox_pane_inner;	/**< The inner pane */

	Ewl_Widget *ewl_iconbox_context_menu;	/**< The context menu */
	Ewl_Widget *ewl_iconbox_context_menu_item;	/**< Context menu item */
	Ewl_Widget *ewl_iconbox_menu_floater;	/**< The menu floater */
	Ewl_Widget *ewl_iconbox_view_menu;	/**< The view menu */

	Ewl_Widget *icon_menu_floater;		/**< Icon menu floater */
	Ewl_Widget *icon_menu_item;		/**< Icon menu item */
	Ewl_Widget *icon_menu;			/**< Icon menu */


	Ecore_List *ewl_iconbox_icon_list;	/**< The icon list */

	Ewl_Iconbox_Icon *drag_icon;		/**< The drag icon */
	Ewl_Iconbox_Icon *edit_icon;		/**< The edit icon */
	Ewl_Iconbox_Icon *select_icon;		/**< The select icon */

	int xdown, 				/**< Last icon button down x, for edge resistence */
	    ydown;		   		/**< Last icon button down y, for edge resistence */
	int dx,					/**< Drag start x */ 
	    dy;			   		/**< Drag start y */
	int lx, 				/**< Layout x */
	    ly;			   		/**< Layout y */
	int ox, 				/**< Layout x offset */
	    oy;			   		/**< Layout y offset */
	int iw, 				/**< Custom icon width */
	    ih;			   		/**< Custom icon height*/

	Ewl_Widget *background;			/**< Background */

	Ewl_Widget *entry;			/**< label editing entry */
	Ewl_Widget *entry_floater;		/**< Entry floater */
	Ewl_Widget *entry_box;			/**< Entry box */

	Ewl_Widget *select;			/**< selection */
	Ewl_Widget *select_floater;		/**< Selection floater */
	int drag_box;			  	/**< Are we dragging? */

	struct timeval lasttime;		/**< Last key event/repeat time */
	void (*key_event_cb)(Ewl_Iconbox *ib, void *data, char *keyname); /**< Key event callback */
	void *key_event_data;			/**< Key event data */

	int editable;				/**< Does the iconbox allow editing */
};

Ewl_Widget	*ewl_iconbox_new(void);
Ewl_Widget	*ewl_iconbox_icon_new(void);
int		 ewl_iconbox_init(Ewl_Iconbox *iconbox);
int		 ewl_iconbox_icon_init(Ewl_Iconbox_Icon *icon);

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

/* 
 * Internal callbacks 
 */
void ewl_iconbox_dnd_drop_cb(Ewl_Widget *item, void *ev_data, void *user_data);
void ewl_iconbox_dnd_position_cb(Ewl_Widget *item, void *ev_data, void *user_data);
void ewl_iconbox_arrange_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_mouse_move_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_pane_mouse_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_mouse_up_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_icon_mouse_up_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_icon_mouse_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_icon_label_mouse_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_icon_destroy_cb(Ewl_Widget *w, void *ev_data ,void *user_data );
void ewl_iconbox_key_press_cb(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif

