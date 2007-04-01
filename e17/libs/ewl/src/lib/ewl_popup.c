/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "ewl_base.h"
#include "ewl_popup.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static void ewl_popup_position_check(Ewl_Popup *p);
static void ewl_popup_size_check(Ewl_Popup *p);

/**
 * @return Returns pointer to new popup widget on success, NULL on failure.
 * @brief Allocate a new popup widget
 */
Ewl_Widget *
ewl_popup_new(void)
{
	Ewl_Popup  *p;

	DENTER_FUNCTION(DLEVEL_STABLE);

	p = NEW(Ewl_Popup, 1);
	if (!p)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_popup_init(p)) {
		ewl_widget_destroy(EWL_WIDGET(p));
		p = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(p), DLEVEL_STABLE);
}

/**
 * @param p: the popup to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the popup and inherited fields
 *
 * Clears the contents of the popup and stores the
 * default values.
 */
int
ewl_popup_init(Ewl_Popup *p)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, FALSE);

	w = EWL_WIDGET(p);
	if (!ewl_window_init(EWL_WINDOW(w)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(w, EWL_POPUP_TYPE);
	ewl_widget_appearance_set(w, EWL_POPUP_TYPE);
	ewl_object_fill_policy_set(EWL_OBJECT(p), EWL_FLAG_FILL_NONE);

	ewl_window_override_set(EWL_WINDOW(p), TRUE);

	ewl_container_show_notify_set(EWL_CONTAINER(p), 
					ewl_popup_cb_child_show);
	ewl_container_resize_notify_set(EWL_CONTAINER(p),
					ewl_popup_cb_child_resize);

	/*
	 * add the callbacks
	 */
	ewl_callback_append(w, EWL_CALLBACK_SHOW, ewl_popup_cb_show, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, ewl_popup_cb_show, NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, ewl_popup_cb_destroy, 
									NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param p: The popup to set the type for
 * @param type: the new type of the popup
 * @return Returns no value
 * @brief This is to set the type of the popup. The type defines how the
 * popup will handle the positioning.
 */
void
ewl_popup_type_set(Ewl_Popup *p, Ewl_Popup_Type type)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_POPUP_TYPE);

	p->type = type;
	/* XXX: Do we need a configure here? */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: The popup to get the type from
 * @return Returns the type of the popup
 * @brief This is to get the type of the popup. The type defines how the
 * popup will handle the positioning
 */
Ewl_Popup_Type 
ewl_popup_type_get(Ewl_Popup *p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, EWL_POPUP_TYPE_NONE);
	DCHECK_TYPE_RET("p", p, EWL_POPUP_TYPE, EWL_POPUP_TYPE_NONE);

	DRETURN_INT(p->type, DLEVEL_STABLE);
}

/**
 * @param p: The popup to set the follow widget
 * @param w: The widget to follow
 * @return Returns no value
 * @brief Set the follow widget of the popup
 */
void
ewl_popup_follow_set(Ewl_Popup *p, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_POPUP_TYPE);

	if (p->follow == w)
		DRETURN(DLEVEL_STABLE);
	
	if (p->follow) {
		ewl_callback_del_with_data(p->follow, EWL_CALLBACK_DESTROY,
						ewl_popup_cb_follow_destroy, p);
		ewl_callback_del_with_data(p->follow, EWL_CALLBACK_CONFIGURE,
						ewl_popup_cb_follow_configure,
						p);
	}

	if (w) {
		ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, 
					ewl_popup_cb_follow_destroy, p);
		ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
					ewl_popup_cb_follow_configure, p);
	}

	p->follow = w;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: The popup to get the follow from
 * @return Returns the follow of the popup
 * @brief 
 */
Ewl_Widget *
ewl_popup_follow_get(Ewl_Popup *p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, EWL_POPUP_TYPE_NONE);
	DCHECK_TYPE_RET("p", p, EWL_POPUP_TYPE, EWL_POPUP_TYPE_NONE);

	DRETURN_PTR(p->follow, DLEVEL_STABLE);
}


/**
 * @param p: The popup
 * @param fit: TRUE or FALSE
 * @return Returns no value
 * @brief
 */
void
ewl_popup_fit_to_follow_set(Ewl_Popup *p, int fit)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_POPUP_TYPE);

	p->fit_to_follow = !!fit;	

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: The popup 
 * @return Returns if the popup is set to fit the size of the follow
 * @brief 
 */
int
ewl_popup_fit_to_follow_get(Ewl_Popup *p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, EWL_POPUP_TYPE_NONE);
	DCHECK_TYPE_RET("p", p, EWL_POPUP_TYPE, EWL_POPUP_TYPE_NONE);

	DRETURN_INT(p->fit_to_follow, DLEVEL_STABLE);
}

/**
 * @param p: The popup to set the mouse position for
 * @param x: the x coordinate
 * @param y: the y coordinate
 * @return Returns no value
 * @brief This is to set the mouse coordinates to the popup,
 * where it get placed relative to.
 */
void
ewl_popup_mouse_position_set(Ewl_Popup *p, int x, int y)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_POPUP_TYPE);

	p->mouse.x = x;
	p->mouse.y = y;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: The popup to set the offset for
 * @param x: the x offset
 * @param y: the y offset
 * @return Returns no value
 * @brief This is to set the offset to where the popup will be placed
 */
void
ewl_popup_offset_set(Ewl_Popup *p, int x, int y)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_POPUP_TYPE);

	p->offset.x = x;
	p->offset.y = y;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The show callback
 */
void
ewl_popup_cb_show(Ewl_Widget *w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_POPUP_TYPE);

	ewl_popup_size_check(EWL_POPUP(w));
	ewl_popup_position_check(EWL_POPUP(w));

	if (ewl_window_pointer_grab_get(EWL_WINDOW(w)))
		ewl_window_pointer_grab_set(EWL_WINDOW(w), TRUE);

	if (ewl_window_keyboard_grab_get(EWL_WINDOW(w)))
		ewl_window_keyboard_grab_set(EWL_WINDOW(w), TRUE);

	/* Popups should be flagged as transient for their parent windows */
	if (EWL_POPUP(w)->follow) {
		Ewl_Embed *emb;
		emb = ewl_embed_widget_find(EWL_POPUP(w)->follow);
		if (emb) {
			void *pwin = NULL;
			/*
			 * If the followed window is transient, defer to that
			 * window's parent
			 */
			if (EWL_WINDOW_IS(emb)) {
				Ewl_Window *win = EWL_WINDOW(emb);

				if (win->flags & EWL_WINDOW_TRANSIENT)
					pwin = win->transient.ewl->window;
				else if (win->flags & EWL_WINDOW_TRANSIENT_FOREIGN)
					pwin = win->transient.foreign;
			}

			if (!pwin)
				pwin = emb->canvas_window;
			ewl_window_transient_for_foreign(EWL_WINDOW(w), pwin);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_popup_cb_destroy(Ewl_Widget *w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	Ewl_Popup *p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_POPUP_TYPE);

	p = EWL_POPUP(w);
	if (p->follow)
		ewl_callback_del_with_data(p->follow, EWL_CALLBACK_DESTROY,
					ewl_popup_cb_follow_destroy, p);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev_data: UNUSED
 * @param user_data: the popup
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_popup_cb_follow_configure(Ewl_Widget *w __UNUSED__, 
				void *ev_data __UNUSED__, void *user_data)
{
	Ewl_Popup *p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("user_data", user_data, EWL_POPUP_TYPE);

	p = EWL_POPUP(user_data);
	ewl_popup_size_check(p);
	ewl_popup_position_check(p);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev_data: UNUSED
 * @param user_data: the popup
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_popup_cb_follow_destroy(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
				void *user_data)
{
	Ewl_Popup *p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("user_data", user_data, EWL_POPUP_TYPE);

	p = EWL_POPUP(user_data);
	p->follow = NULL;

	if (p->type != EWL_POPUP_TYPE_NONE)
		ewl_widget_hide(EWL_WIDGET(p));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: UNUSED
 * @return Returns no value
 * @brief The child show callback
 */
void
ewl_popup_cb_child_show(Ewl_Container *c, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	
	ewl_overlay_cb_child_show(c, w);
	ewl_popup_position_check(EWL_POPUP(c));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @param size: UNUSED
 * @param o: UNUSED
 * @return Returns no value
 * @brief The child resize callback
 */
void
ewl_popup_cb_child_resize(Ewl_Container *c, Ewl_Widget *w,
	       int size, Ewl_Orientation o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_POPUP_TYPE);
	
	ewl_overlay_cb_child_resize(c, w, size, o);
	ewl_popup_position_check(EWL_POPUP(c));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_popup_position_check(Ewl_Popup *p)
{
	Ewl_Embed *emb;
	int x = 0, y = 0;
	int desk_w = 0, desk_h = 0;
	int win_x = 0, win_y = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_POPUP_TYPE);
	
	if (!p->follow || p->type == EWL_POPUP_TYPE_NONE)
		DRETURN(DLEVEL_STABLE);
	
	emb = ewl_embed_widget_find(p->follow);
	ewl_embed_desktop_size_get(emb, &desk_w, &desk_h);
	ewl_embed_window_position_get(emb, &win_x, &win_y);
	
	if (p->type == EWL_POPUP_TYPE_MOUSE) {
		x = win_x + p->mouse.x;
		y = win_y + p->mouse.y;

		if (x + p->offset.x + CURRENT_W(p) > desk_w)
			x -= p->offset.x + CURRENT_W(p);
		else
			x += p->offset.x;

		if (y + p->offset.y + CURRENT_H(p) > desk_h)
			y -= p->offset.y + CURRENT_H(p);
		else
			y += p->offset.y;
	}
	else if (p->type == EWL_POPUP_TYPE_MENU_VERTICAL) {
		
		x = win_x + CURRENT_X(p->follow);
		y = win_y + CURRENT_Y(p->follow);
		
		if (x + CURRENT_W(p) > desk_w)
			x = desk_w - CURRENT_W(p);
		
		if (y + CURRENT_H(p->follow) + CURRENT_H(p) > desk_h)
			y -= CURRENT_H(p);
		else
			y += CURRENT_H(p->follow);
	}
	else if (p->type == EWL_POPUP_TYPE_MENU_HORIZONTAL) {
		
		x = win_x + CURRENT_X(p->follow);
		y = win_y + CURRENT_Y(p->follow);

		if (x + CURRENT_W(p->follow) + CURRENT_W(p) > desk_w)
			x -= CURRENT_W(p);
		else
			x += CURRENT_W(p->follow);

		if (y + CURRENT_H(p) > desk_h)
			y = desk_h - CURRENT_H(p);
	}

	ewl_window_move(EWL_WINDOW(p), x, y);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_popup_size_check(Ewl_Popup *p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_POPUP_TYPE);
	
	if (!p->follow || p->type == EWL_POPUP_TYPE_NONE || !p->fit_to_follow)
		DRETURN(DLEVEL_STABLE);
	
	if (p->type == EWL_POPUP_TYPE_MENU_VERTICAL)
		ewl_object_w_request(EWL_OBJECT(p), CURRENT_W(p->follow));

	else if (p->type == EWL_POPUP_TYPE_MENU_HORIZONTAL)
		ewl_object_h_request(EWL_OBJECT(p), CURRENT_H(p->follow));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

