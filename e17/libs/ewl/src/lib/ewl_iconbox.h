#ifndef __EWL_ICONBOX_H__
#define __EWL_ICONBOX_H__


#define EWL_ICONBOX_ICON_PADDING 15
#define EWL_ICONBOX_MOVE_TOLERANCE 5
#define EWL_ICONBOX_MINIMUM_SIZE 50

/**
 * @file ewl_iconbox.h
 * @defgtroup Ewl_IconBox IconBox: The Icon Box Layout Container
 * @brief Defines the Ewl_IconBox class, used to display a collection of icons 
 *  	  with annotations and support arranging/manipulating contained icons
 *
 */

/** 
 * @themekey /iconbox/file
 * @themekey /iconbox/group
 */


typedef struct Ewl_IconBox Ewl_IconBox;

typedef struct Ewl_IconBox_Icon Ewl_IconBox_Icon;
struct Ewl_IconBox_Icon
{
	Ewl_Border box;
	
	Ewl_Widget *image;
	Ewl_Widget *floater;
	/*Ewl_Widget *box;*/
	int drag;
	int selected ;
	Ewl_IconBox* icon_box_parent; /* Our reference upwards, kinda breaks OO conventions */
};






/**
 * @def EWL_ICONBOX(iconbox)
 * Typecast a pointer to an Ewl_IconBox pointer
 */
#define EWL_ICONBOX(iconbox) ((Ewl_IconBox *) iconbox)
#define EWL_ICONBOX_ICON(icon) ((Ewl_IconBox_Icon *) icon)

/** 
 * @struct Ewl_IconBox
 * Inherits from an Ewl_Container to provide layout facilities for child widgets placed inside
 * Layout is either free-form, or auto-arranged to a grid.
 */
struct Ewl_IconBox
{
	Ewl_Box	box; /**< Inherit from Ewl_Container */

	Ewl_Widget *ewl_iconbox_scrollpane;
	Ewl_Widget *ewl_iconbox_pane_inner;

	Ewl_Widget* ewl_iconbox_context_menu;
	Ewl_Widget* ewl_iconbox_context_menu_item;
	Ewl_Widget* ewl_iconbox_menu_floater;
	Ewl_Widget* ewl_iconbox_menu_box;
	Ewl_Widget* ewl_iconbox_view_menu;

	Ecore_List *ewl_iconbox_icon_list;  /* The icon list */
	char* test;

	Ewl_IconBox_Icon* drag_icon;
	Ewl_IconBox_Icon* edit_icon;
	int xdown, ydown;		   /* Last icon button down x/y, for edge resistence */
	int dx, dy;			   /* Drag start x/y */

	/* Objects for label editing..*/
	Ewl_Widget* entry;
	Ewl_Widget* entry_floater;
	Ewl_Widget* entry_box;

	/*Objects for selection */
	Ewl_Widget* select;
	Ewl_Widget* select_floater;
	int drag_box;			  /* Are we dragging? */
	

	/* Editable flag */
	int editable;

};


Ewl_Widget	*ewl_iconbox_new();
Ewl_Widget	*ewl_iconbox_icon_new();
int		ewl_iconbox_init(Ewl_IconBox* iconbox);
int		ewl_iconbox_icon_init(Ewl_IconBox_Icon* icon);


/* External functions */
void ewl_iconbox_editable_set(Ewl_IconBox *ib, int edit);
void ewl_iconbox_icon_arrange(Ewl_IconBox* ib);
void ewl_iconbox_deselect_all(Ewl_IconBox* ib);
Ewl_IconBox_Icon* ewl_iconbox_icon_add(Ewl_IconBox* iconbox, char* name, char* icon_file);


/* Internal callbacks */
void ewl_iconbox_arrange_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_iconbox_expansion_cb(Ewl_Widget *w, void *ev_data, void *user_data);

/* Internal functions */
void ewl_iconbox_icon_select(Ewl_IconBox_Icon* ib, int loc);
void ewl_iconbox_icon_deselect(Ewl_IconBox_Icon *ib);
void ewl_iconbox_label_edit_key_down(Ewl_Widget *w, void *ev_data, void* user_data);
void ewl_iconbox_inner_pane_calculate(Ewl_IconBox* ib);
void ewl_iconbox_mouse_up(Ewl_Widget *w, void *ev_data, void *user_data);




#endif
