
#include <Ewl.h>

void            __expand_menu(Ewl_Widget * w, void *ev_data, void *user_data);

void            __item_clicked(Ewl_Widget * w, void *ev_data, void *user_data);

void            __ewl_menu_realize(Ewl_Widget * w, void *ev_data,
				   void *user_data);


void            __expand_menu_animator(int val, void *ev_data);
void            __collapse_menu_animator(int val, void *ev_data);
void            __expand_menu_and_title_animator(int val, void *ev_data);
void            __collapse_menu_and_title_animator(int val, void *ev_data);

void            __ewl_menu_start(Ewl_Widget * w);

/**
 * ewl_menu_new - create a new internal menu
 * @image: the image icon to use for this menu
 * @title: the text to place in the menu
 *
 * Returns a pointer to a newly allocated menu on success, NULL on
 * failure.
 */
Ewl_Widget     *ewl_menu_new(char *image, char *title)
{
	Ewl_Menu       *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = NEW(Ewl_Menu, 1);
	if (!menu)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ZERO(menu, Ewl_Menu, 1);

	ewl_menu_init(menu, image, title);

	DRETURN_PTR(EWL_WIDGET(menu), DLEVEL_STABLE);
}


/**
 * ewl_menu_init - initialize an internal menu to starting values
 * @menu: the menu to initialize
 * @follows: the widget the menu will follow
 * @type: the menu type
 *
 * Returns nothing.
 */
void ewl_menu_init(Ewl_Menu * menu, char *image, char *title)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("menu", menu);

	/*
	 * Create the popup menu portion of the menu. Do this prior to
	 * initializing the rest of the fields to avoid the add callback being
	 * called.
	 */
	menu->popup = ewl_window_new();
	ewl_window_set_auto_size(EWL_WINDOW(menu->popup), TRUE);
	ewl_window_set_borderless(EWL_WINDOW(menu->popup));
	ewl_object_set_fill_policy(EWL_OBJECT(menu->popup),
				   EWL_FILL_POLICY_NONE);

	/*
	 * We need a box for laying out the contents of the menu.
	 */
	menu->popbox = ewl_vbox_new();
	ewl_object_set_alignment(EWL_OBJECT(menu->popbox),
				 EWL_ALIGNMENT_LEFT | EWL_ALIGNMENT_TOP);
	ewl_container_append_child(EWL_CONTAINER(menu->popup), menu->popbox);

	/*
	 * Initialize the defaults of the inherited fields.
	 */
	ewl_menu_base_init(EWL_MENU_BASE(menu), image, title);
	/*
	 * ewl_object_set_fill_policy(EWL_OBJECT(menu), EWL_FILL_POLICY_NONE);
	 */

	/*
	 * The realize needs to create the pop-up.
	 */
	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_SELECT,
			    __expand_menu, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_menu_set_title_expandable - allow title to be expanded/contracted
 * @menu: the menu
 *
 * Returns nothing.
 */
void ewl_menu_set_title_expandable(Ewl_Menu * menu)
{

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu->t_expand = 1;

	/*
	 * ewl_object_set_maximum_size (EWL_OBJECT(menu->corner), 30, 22);
	 * ewl_object_set_minimum_size (EWL_OBJECT(menu->corner), 30, 22);
	 */

	/*
	 * ewl_callback_append(menu->corner_button, EWL_CALLBACK_MOUSE_UP,
	 * __expand_menu, menu);
	 * ewl_container_append_child(EWL_CONTAINER(menu->corner), 
	 * menu->corner_button);
	 * 
	 * ewl_widget_configure(EWL_WIDGET(menu->corner));
	 */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __expand_menu(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             x, y;
	Ewl_Menu       *menu;
	Ewl_Window     *pwin;

	menu = EWL_MENU(w);
	pwin = ewl_window_find_window_by_widget(w);

	/*
	 * Position the popup menu relative to the menu.
	 */
	if (EWL_MENU_ITEM(w)->submenu) {
		x = CURRENT_X(pwin) + CURRENT_X(w) + CURRENT_W(w);
		y = CURRENT_Y(pwin) + CURRENT_Y(w);
	} else {
		x = CURRENT_X(pwin) + CURRENT_X(w);
		y = CURRENT_Y(pwin) + CURRENT_Y(w) + CURRENT_H(w);

		ewl_window_set_min_size(EWL_WINDOW(menu->popup),
					CURRENT_W(menu),
					MINIMUM_H(menu->popup));
	}

	ewl_window_move(EWL_WINDOW(menu->popup), x, y);
	printf("Moving popup to %d, %d\n", x, y);

	ewl_widget_show(menu->popup);
	ewl_widget_configure(menu->popup);
}

void __ewl_menu_add_child(Ewl_Container * p, Ewl_Widget * c)
{
	Ewl_Widget     *box;

	box = ewd_list_goto_first(p->children);
}

void __expand_menu_animator(int val, void *ev_data)
{
}


void __collapse_menu_animator(int val, void *ev_data)
{
}


void __expand_menu_and_title_animator(int val, void *ev_data)
{
}


void __collapse_menu_and_title_animator(int val, void *ev_data)
{
}


void __ewl_menu_start(Ewl_Widget * w)
{
}
