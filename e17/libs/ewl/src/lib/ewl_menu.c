#include "ewl_private.h"
#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

/**
 * @return Returns a pointer to a new menu on success, NULL on failure.
 * @brief Create a new internal menu
 */
Ewl_Widget *
ewl_menu_new(void)
{
	Ewl_Menu *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = NEW(Ewl_Menu, 1);
	if (!menu)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_menu_init(menu)) {
		ewl_widget_destroy(EWL_WIDGET(menu));
		menu = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(menu), DLEVEL_STABLE);
}

/**
 * @param menu: the menu to initialize
 * @return Returns no value.
 * @brief Initialize an internal menu to starting values
 */
int
ewl_menu_init(Ewl_Menu *menu)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("menu", menu, FALSE);

	/*
	 * Initialize the defaults of the inherited fields.
	 */
	if (!ewl_menu_base_init(EWL_MENU_BASE(menu)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(menu), "menu_container");
	ewl_widget_inherit(EWL_WIDGET(menu), EWL_MENU_TYPE);

	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_MOUSE_MOVE,
			    ewl_menu_expand_mouse_move_cb, NULL);

	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_FOCUS_IN,
			    ewl_menu_expand_cb, NULL);
	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_REALIZE,
			    ewl_menu_realize_cb, NULL);
	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_CONFIGURE,
			    ewl_menu_configure_cb, NULL);

	menu->menubar_parent = NULL;

	/*
	 * Create the popup menu portion of the widget.
	 */
	menu->base.popup = ewl_window_new();
	ewl_window_keyboard_grab_set(EWL_WINDOW(menu->base.popup), TRUE);
	ewl_window_pointer_grab_set(EWL_WINDOW(menu->base.popup), TRUE);
	
	ewl_window_override_set(EWL_WINDOW(menu->base.popup), TRUE); 
	ewl_window_borderless_set(EWL_WINDOW(menu->base.popup));
	ewl_widget_internal_set(menu->base.popup, TRUE);
	ewl_widget_appearance_set(EWL_WIDGET(menu->base.popup), EWL_MENU_TYPE);
	ewl_object_fill_policy_set(EWL_OBJECT(menu->base.popup),
				   EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(menu->base.popup),
				 EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_TOP);
	ewl_callback_append(menu->base.popup, EWL_CALLBACK_MOUSE_DOWN,
			    ewl_menu_hide_cb, menu);

	ewl_callback_append(menu->base.popup, EWL_CALLBACK_MOUSE_MOVE,
				ewl_menu_mouse_move_cb, menu);
			    
	ewl_callback_prepend(menu->base.popup, EWL_CALLBACK_DESTROY,
				ewl_menu_popup_destroy_cb, menu);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_menu_realize_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Menu *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	menu = EWL_MENU(w);

	ewl_menu_popup_move_cb(menu->base.popup, NULL, w);

	/*
	 * Position the popup menu relative to the menu.
	 */
	ewl_callback_append(EWL_WIDGET(EWL_WINDOW(menu->base.popup)),
				       EWL_CALLBACK_CONFIGURE,
				       ewl_menu_popup_move_cb, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	int x, y;
	Ewl_Menu *menu;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	menu = EWL_MENU(w);
	emb = ewl_embed_widget_find(w);

	ewl_embed_window_position_get(EWL_EMBED(emb), &x, &y);
	menu->popup_x = x + CURRENT_X(w);
	menu->popup_y = y + CURRENT_Y(w);

	ewl_menu_popup_move_cb(menu->base.popup, NULL, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_expand_mouse_move_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__) 
{
	Ewl_Menu *menu;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	
	menu = EWL_MENU(w);
	if (menu->menubar_parent) {
		Ewl_Menu *sub, *hide_menu = NULL;
		Ewl_Container *bar;
		int vis = 0;
        
		bar = EWL_CONTAINER(menu->menubar_parent);
		ewl_container_child_iterate_begin(bar);
		while ((sub = EWL_MENU(ewl_container_child_next(bar)))) {
			if ((sub != EWL_MENU(w)) 
					&& ewl_widget_type_is(EWL_WIDGET(sub), EWL_MENU_TYPE)
					&& (sub->base.popup) 
					&& VISIBLE(sub->base.popup)) {
				hide_menu = sub;
				vis++;
				break;
			}
		}

		if (vis && hide_menu) {
			ewl_widget_hide(hide_menu->base.popup);
			ewl_callback_call(w, EWL_CALLBACK_FOCUS_IN);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_expand_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Menu *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	menu = EWL_MENU(w);
	
	ewl_widget_show(menu->base.popup);
	ewl_window_raise(EWL_WINDOW(menu->base.popup));
	ewl_widget_focus_send(menu->base.popbox);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_popup_move_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data)
{
        Ewl_Widget *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	menu = EWL_WIDGET (user_data);
	if (EWL_MENU_ITEM(menu)->inmenu) {
		ewl_window_move(EWL_WINDOW(w),
				EWL_MENU(menu)->popup_x + CURRENT_W(menu),
				EWL_MENU(menu)->popup_y);
	}
	else {
		ewl_window_move(EWL_WINDOW(w), 
				EWL_MENU(menu)->popup_x,
				EWL_MENU(menu)->popup_y + CURRENT_H(menu));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_mouse_move_cb(Ewl_Widget *w, void *ev_data, void *user_data) 
{
	Ewl_Event_Mouse_Move *ev;
	Ewl_Menu *menu;
	Ewl_Embed *embed, *menu_embed;
	int x, y, wx, wy;
	int width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	DCHECK_TYPE("user_data", user_data, EWL_MENU_TYPE);

	ev = ev_data;
	menu = user_data;
	
	embed = ewl_embed_widget_find(EWL_WIDGET(menu)->parent);
	menu_embed = ewl_embed_widget_find(EWL_WIDGET(menu->base.popup));
	ewl_embed_window_position_get(EWL_EMBED(embed), &wx, &wy);
	ewl_embed_window_position_get(EWL_EMBED(menu->base.popup), &x, &y);
	ewl_object_current_size_get(EWL_OBJECT(menu->base.popup), &width, &height);

	if (((ev->x + x) > x) && ((ev->y + y) > y) 
			&& ((ev->x + x) < (x + width)) 
			&& ((ev->y + y) < (y + height))) {
		if (ewl_embed_active_embed_get() != menu_embed)
			ewl_embed_active_set(menu_embed, 1);
	} 
	else {
		if (menu->menubar_parent) 
			ewl_embed_mouse_move_feed(embed, (ev->x + x) - wx, 
							(ev->y + y) - wy, 0);
	}
}

void
ewl_menu_hide_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (w == ewl_embed_focused_widget_get(EWL_EMBED(w)))
		ewl_widget_hide(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_popup_destroy_cb(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
					void *data)
{
	Ewl_Menu *m;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	m = data;
	if (m->base.popup) m->base.popup = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

