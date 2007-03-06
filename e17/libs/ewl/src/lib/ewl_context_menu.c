/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "ewl_base.h"
#include "ewl_box.h"
#include "ewl_menu.h"
#include "ewl_context_menu.h"
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static Ewl_Context_Menu *ewl_context_menu_grabber = NULL;

static int ewl_context_menu_mouse_feed(Ewl_Context_Menu *cm, 
					Ewl_Embed *emb, int x, int y);

static void ewl_context_menu_grabber_set(Ewl_Context_Menu *cm);
static void ewl_context_menu_grabber_unset(Ewl_Context_Menu *cm);

/**
 * @return Returns pointer to new context_menu widget on success, 
 * NULL on failure.
 * @brief Allocate a new context_menu widget
 */
Ewl_Widget *
ewl_context_menu_new(void)
{
	Ewl_Context_Menu  *cm;

	DENTER_FUNCTION(DLEVEL_STABLE);

	cm = NEW(Ewl_Context_Menu, 1);
	if (!cm)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_context_menu_init(cm)) {
		ewl_widget_destroy(EWL_WIDGET(cm));
		cm = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(cm), DLEVEL_STABLE);
}

/**
 * @param cm: the context_menu to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the context_menu and inherited fields
 *
 * Clears the contents of the context_menu and stores the
 * default values.
 */
int
ewl_context_menu_init(Ewl_Context_Menu *cm)
{
	Ewl_Widget *w;
	Ewl_Widget *box;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cm", cm, FALSE);

	w = EWL_WIDGET(cm);
	if (!ewl_popup_init(EWL_POPUP(w)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(w, EWL_CONTEXT_MENU_TYPE);
	/* XXX This isn't really correct */
	ewl_widget_appearance_set(w, EWL_MENU_TYPE);
	//ewl_widget_appearance_set(w, EWL_CONTEXT_MENU_TYPE"/"EWL_POPUP_TYPE);
	ewl_object_alignment_set(EWL_OBJECT(w),
				 EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_TOP);
	ewl_popup_type_set(EWL_POPUP(cm), EWL_POPUP_TYPE_MOUSE);

	box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(cm), box);
	ewl_widget_show(box);

	/* redirect the context menu to the inner box */
	ewl_container_redirect_set(EWL_CONTAINER(cm), EWL_CONTAINER(box));
	ewl_widget_internal_set(box, TRUE);
	ewl_container_add_notify_set(EWL_CONTAINER(box), 
					ewl_context_menu_cb_child_add);

	/*
	 * add the callbacks
	 */
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN, 
				ewl_context_menu_cb_mouse_down, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE, 
				ewl_context_menu_cb_mouse_move, NULL);
	ewl_callback_append(w, EWL_CALLBACK_HIDE, 
				ewl_context_menu_cb_hide, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param cm: the context menu to attach
 * @param w: a widget that gets the context menu attached
 *
 * This function attach a context menu to a widget. You can attach one
 * context menu to more than one widget, but all widgets have to share
 * the same embed, i.e. they must be in the same window or embed.
 */
void
ewl_context_menu_attach(Ewl_Context_Menu *cm, Ewl_Widget *w)
{
	Ewl_Embed *emb;
	Ewl_Widget *follow;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cm", cm);
	DCHECK_TYPE("cm", cm, EWL_CONTEXT_MENU_TYPE);
	DCHECK_PARAM_PTR("w", w);

	emb = ewl_embed_widget_find(w);

	if (!emb) {
		DWARNING("widget w has no embed parent");
		DRETURN(DLEVEL_STABLE);
	}

	follow = ewl_popup_follow_get(EWL_POPUP(cm));

	if (!follow)
		ewl_popup_follow_set(EWL_POPUP(cm), EWL_WIDGET(emb));
	else if (follow != EWL_WIDGET(emb)) {
		DWARNING("Sorry, the context menu is not supposed to work in"
				"more then one window");
		DRETURN(DLEVEL_STABLE);
	}

	 ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN, 
			 ewl_context_menu_cb_attach_mouse_down, cm);

	DRETURN(DLEVEL_STABLE);
}

/**
 * @param cm: the context menu to detach
 * @param w: a widget that has @a cm attached
 *
 * This function detachs an earlier attached context menu to a widget
 */
void 
ewl_context_menu_detach(Ewl_Context_Menu *cm, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cm", cm);
	DCHECK_TYPE("cm", cm, EWL_CONTEXT_MENU_TYPE);
	DCHECK_PARAM_PTR("w", w);

	ewl_callback_del(w, EWL_CALLBACK_MOUSE_DOWN, 
				    ewl_context_menu_cb_attach_mouse_down);

	DRETURN(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The mouse down callback
 */
void 
ewl_context_menu_cb_attach_mouse_down(Ewl_Widget * w, void * ev_data, 
					void * user_data)
{
	Ewl_Context_Menu *cm;
	Ewl_Event_Mouse_Down *ev;

	ev = ev_data;

	if (ev->button != 3)
		DRETURN(DLEVEL_STABLE);
	
	cm = EWL_CONTEXT_MENU(user_data);

	ewl_popup_mouse_position_set(EWL_POPUP(cm), ev->base.x, ev->base.y);
	ewl_widget_show(EWL_WIDGET(cm));
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The mouse down callback
 */
void
ewl_context_menu_cb_mouse_down(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	
	if (w == ewl_embed_focused_widget_get(EWL_EMBED(w))) {
		while (EWL_POPUP_IS(w)) {
			ewl_widget_hide(w);
			w = ewl_popup_follow_get(EWL_POPUP(w));
			if (!w)
				break;
			w = EWL_WIDGET(ewl_embed_widget_find(w));
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
 * @brief The hide callback
 */
void
ewl_context_menu_cb_hide(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Context_Menu *cm;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	cm = EWL_CONTEXT_MENU(w);
	
	if (cm->open_menu) {
		ewl_widget_hide(cm->open_menu);
		cm->open_menu = NULL;
	}

	ewl_context_menu_grabber_unset(cm);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The mouse move callback
 */
void
ewl_context_menu_cb_mouse_move(Ewl_Widget *w, void *ev_data, 
					void *user_data __UNUSED__) 
{
	Ewl_Event_Mouse *ev;
	Ewl_Context_Menu *cm;
	Ewl_Embed *popup_embed;
	int width, height;
	int ex, ey;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ev = ev_data;
	cm = EWL_CONTEXT_MENU(w);
	
	popup_embed = ewl_embed_widget_find(w);
	ewl_object_current_size_get(EWL_OBJECT(w), &width, &height);
	ewl_embed_window_position_get(EWL_EMBED(w), &ex, &ey);
	ex += ev->x;
	ey += ev->y;

	if (ev->x > 0 && ev->y > 0 && ev->x <= width && ev->y <= height) {
		/* The mouse is actually in the popup, so that the
		 * popup to be active if it shouldn't be */
		if (ewl_embed_active_embed_get() != popup_embed)
			ewl_embed_active_set(popup_embed, TRUE);

		return;
	}
	else if (cm->open_menu 
			&& ewl_context_menu_mouse_feed(cm,
					EWL_EMBED(cm->open_menu), ex, ey)) {
		DRETURN(DLEVEL_STABLE);
	}
	else {
		Ewl_Widget *follow;
		Ewl_Embed *popup;
		
		/* We now feed the mouse moves to only the parent if the mouse
		 * is inside of it */
		popup = EWL_EMBED(w);
		do {
			follow = ewl_popup_follow_get(EWL_POPUP(popup));
			if (!follow)
				break;
			popup = ewl_embed_widget_find(follow);

			if (ewl_context_menu_mouse_feed(cm, popup, ex, ey)) {
				break;
			}
		} while (EWL_POPUP_IS(popup));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child add callback
 */
void
ewl_context_menu_cb_child_add(Ewl_Container *c, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (EWL_MENU_ITEM_IS(w)) {
		Ewl_Widget *cm = ewl_widget_parent_get(EWL_WIDGET(c));
		EWL_MENU_ITEM(w)->inmenu = cm;
	}
	
	if (ewl_widget_internal_is(w) || !ewl_widget_focusable_get(w))
		DRETURN(DLEVEL_STABLE);

	ewl_callback_append(w, EWL_CALLBACK_MOUSE_IN, 
				ewl_context_menu_cb_child_mouse_in, c);
	if (!EWL_MENU_IS(w))
		ewl_callback_append(w, EWL_CALLBACK_CLICKED,
				ewl_context_menu_cb_child_clicked,
				ewl_widget_parent_get(EWL_WIDGET(c)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The hide callback
 */
void
ewl_context_menu_cb_child_mouse_in(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data)
{
	Ewl_Context_Menu *cm;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	
	emb = ewl_embed_widget_find(w);
	cm = EWL_CONTEXT_MENU(emb);

	/* hide the open sub menu */
	if (cm->open_menu) {
		ewl_widget_hide(cm->open_menu);
		cm->open_menu = NULL;
	}
	/* send the focus to the child */
	ewl_widget_focus_send(w);
	
	/* 
	 * grab the key and mouse events again for this window 
	 */
	ewl_context_menu_grabber_set(cm);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The hide callback
 */
void
ewl_context_menu_cb_child_clicked(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data)
{
	Ewl_Widget *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	
	c = EWL_WIDGET(user_data);

	while (EWL_POPUP_IS(c)) {
		ewl_widget_hide(c);
		c = ewl_popup_follow_get(EWL_POPUP(c));
		if (!c)
			break;
		c = EWL_WIDGET(ewl_embed_widget_find(c));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int 
ewl_context_menu_mouse_feed(Ewl_Context_Menu *cm, Ewl_Embed *emb, int x, int y)
{
	int emb_x, emb_y, emb_w, emb_h;

	DCHECK_PARAM_PTR_RET("cm", cm, FALSE);
	DCHECK_TYPE_RET("cm", cm, EWL_CONTEXT_MENU_TYPE, FALSE);
	DCHECK_PARAM_PTR_RET("emb", emb, FALSE);
	DCHECK_TYPE_RET("emb", emb, EWL_EMBED_TYPE, FALSE);

	ewl_embed_window_position_get(emb, &emb_x, &emb_y);
	ewl_object_current_size_get(EWL_OBJECT(emb), &emb_w, &emb_h);

	x -= emb_x;
	y -= emb_y;
	if (x > 0 && y > 0 && x <= emb_w && y <= emb_h) {
		ewl_embed_mouse_move_feed(emb, x, y, 0);
		DRETURN_INT(DLEVEL_STABLE, TRUE);
	}

	DRETURN_INT(DLEVEL_STABLE, FALSE);
}

static void
ewl_context_menu_grabber_set(Ewl_Context_Menu *cm)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cm", cm);
	DCHECK_TYPE("cm", cm, EWL_CONTEXT_MENU_TYPE);

	if (ewl_context_menu_grabber)
		ewl_context_menu_grabber_unset(ewl_context_menu_grabber);

	ewl_context_menu_grabber = cm;
	ewl_window_keyboard_grab_set(EWL_WINDOW(cm), TRUE);
	ewl_window_pointer_grab_set(EWL_WINDOW(cm), TRUE);
	
	DRETURN(DLEVEL_STABLE);
}

static void
ewl_context_menu_grabber_unset(Ewl_Context_Menu *cm)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cm", cm);
	DCHECK_TYPE("cm", cm, EWL_CONTEXT_MENU_TYPE);

	if (cm == ewl_context_menu_grabber) {
		ewl_window_keyboard_grab_set(EWL_WINDOW(cm), FALSE);
		ewl_window_pointer_grab_set(EWL_WINDOW(cm), FALSE);
		ewl_context_menu_grabber = NULL;
	}
	
	DRETURN(DLEVEL_STABLE);
}

