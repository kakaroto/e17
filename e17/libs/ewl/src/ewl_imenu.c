
#include <Ewl.h>

void            __expand_imenu(Ewl_Widget * w, void *ev_data, void *user_data);

void            __item_clicked(Ewl_Widget * w, void *ev_data, void *user_data);

void            __ewl_imenu_realize(Ewl_Widget * w, void *ev_data,
				    void *user_data);


void            __expand_imenu_animator(int val, void *ev_data);
void            __collapse_imenu_animator(int val, void *ev_data);
void            __expand_imenu_and_title_animator(int val, void *ev_data);
void            __collapse_imenu_and_title_animator(int val, void *ev_data);

void            __ewl_imenu_start(Ewl_Widget * w);

/**
 * ewl_imenu_new - create a new internal menu
 * @image: the image icon to use for this menu
 * @title: the text to place in the menu
 *
 * Returns a pointer to a newly allocated menu on success, NULL on
 * failure.
 */
Ewl_Widget     *ewl_imenu_new(char *image, char *title)
{
	Ewl_IMenu      *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = NEW(Ewl_IMenu, 1);
	if (!menu)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ZERO(menu, Ewl_IMenu, 1);

	ewl_imenu_init(menu, image, title);

	DRETURN_PTR(EWL_WIDGET(menu), DLEVEL_STABLE);
}


/**
 * ewl_imenu_init - initialize an internal menu to starting values
 * @menu: the menu to initialize
 * @follows: the widget the menu will follow
 * @type: the menu type
 *
 * Returns nothing.
 */
void ewl_imenu_init(Ewl_IMenu * menu, char *image, char *title)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("menu", menu);

	/*
	 * Initialize the defaults of the inherited fields.
	 */
	ewl_menu_base_init(EWL_MENU_BASE(menu), image, title);

	/*
	 * The realize needs to create the pop-up.
	 */
	ewl_callback_prepend(EWL_WIDGET(menu), EWL_CALLBACK_SELECT,
			    __expand_imenu, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_imenu_set_title_expandable - allow title to be expanded/contracted
 * @menu: the menu
 *
 * Returns nothing.
 */
void ewl_imenu_set_title_expandable(Ewl_IMenu * menu)
{

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu->t_expand = 1;

	/*
	 * ewl_object_set_maximum_size (EWL_OBJECT(menu->corner), 30, 22);
	 * ewl_object_set_minimum_size (EWL_OBJECT(menu->corner), 30, 22);
	 */

	/*
	 * ewl_callback_append(menu->corner_button, EWL_CALLBACK_MOUSE_UP,
	 * __expand_imenu, menu);
	 * ewl_container_append_child(EWL_CONTAINER(menu->corner), 
	 * menu->corner_button);
	 * 
	 * ewl_widget_configure(EWL_WIDGET(menu->corner));
	 */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __expand_imenu(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_IMenu      *menu;
	Ewl_Window *win;

	menu = EWL_IMENU(w);

	/*
	 * Create the popup menu portion of the menu. Do this prior to
	 * initializing the rest of the fields to avoid the add callback being
	 * called.
	 */
	menu->popup = ewl_floater_new(EWL_WIDGET(menu));
	ewl_box_set_orientation(EWL_BOX(menu->popup), EWL_ORIENTATION_VERTICAL);
	ewl_object_set_fill_policy(EWL_OBJECT(menu->popup),
				   EWL_FILL_POLICY_NONE);
	ewl_object_set_alignment(EWL_OBJECT(menu->popup),
				 EWL_ALIGNMENT_LEFT | EWL_ALIGNMENT_TOP);

	win = ewl_window_find_window_by_widget(w);
	ewl_container_append_child(EWL_CONTAINER(win), menu->popup); 

	/*
	 * Position the popup menu relative to the menu.
	 */
	if (EWL_MENU_ITEM(w)->submenu)
		ewl_floater_set_position(EWL_FLOATER(menu->popup),
					 CURRENT_W(w), 0);
	else {
		ewl_floater_set_position(EWL_FLOATER(menu->popup), 0,
					 CURRENT_H(w));
		ewl_object_set_minimum_width(EWL_OBJECT(menu->popup),
					     CURRENT_W(menu));
	}
}


void __expand_imenu_animator(int val, void *ev_data)
{
}


void __collapse_imenu_animator(int val, void *ev_data)
{
}


void __expand_imenu_and_title_animator(int val, void *ev_data)
{
}


void __collapse_imenu_and_title_animator(int val, void *ev_data)
{
}


void __ewl_imenu_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_IMenu      *menu;

	menu = EWL_IMENU(w);

	ewl_container_append_child(EWL_CONTAINER
				   (ewl_window_find_window_by_widget(w)),
				   menu->popup);
}

void __ewl_imenu_start(Ewl_Widget * w)
{
}
