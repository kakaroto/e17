#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

typedef struct Ewl_Attach_Tooltip Ewl_Attach_Tooltip;
struct Ewl_Attach_Tooltip
{
	Ewl_Widget *embed;

	Ewl_Widget *win;
	Ewl_Widget *box;

	Ewl_Attach *attach;

	Evas_Coord x;
	Evas_Coord y;

	Ecore_Timer *timer;
	Ewl_Widget *to;
};

static Ewl_Attach_List *ewl_attach_list_new(void);
static void ewl_attach_list_free(Ewl_Attach_List *list);

static void ewl_attach_list_add(Ewl_Attach_List *list, Ewl_Widget *parent, 
							Ewl_Attach *attach);
static void *ewl_attach_list_get(Ewl_Attach_List *list, Ewl_Attach_Type type);

static Ewl_Attach *ewl_attach_new(Ewl_Attach_Type t, 
				Ewl_Attach_Data_Type dt, void *data);
static int ewl_attach_init(Ewl_Attach *attach, Ewl_Attach_Type t, 
				Ewl_Attach_Data_Type dt, void *data);
static void ewl_attach_free(Ewl_Attach *attach);

static void ewl_attach_parent_setup(Ewl_Widget *w);
static void ewl_attach_cb_parent_destroy(Ewl_Widget *w, void *ev, void *data);
static void ewl_attach_attach_type_setup(Ewl_Widget *w, Ewl_Attach *attach);

static void ewl_attach_tooltip_attach(Ewl_Widget *w, Ewl_Attach *attach);
static void ewl_attach_tooltip_detach(Ewl_Attach *attach);

static void ewl_attach_cb_tooltip_win_destroy(Ewl_Widget *w, void *ev, void *data);

static void ewl_attach_cb_tooltip_mouse_move(Ewl_Widget *w, void *ev, void *data);
static void ewl_attach_cb_tooltip_mouse_down(Ewl_Widget *w, void *ev, void *data);
static void ewl_attach_cb_tooltip_mouse_out(Ewl_Widget *w, void *ev, void *data);
static int ewl_attach_cb_tooltip_timer(void *data);

static Ewl_Attach_Tooltip *ewl_attach_tooltip = NULL;

/**
 * @param w: The widget to attach the text too
 * @param t: The type of the attachment
 * @param data: The text to set as the attachment
 * @return Returns no value
 * @brief Attaches the text @p data to the widget @p w
 */
void
ewl_attach_text_set(Ewl_Widget *w, Ewl_Attach_Type t, const char *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (!w->attach)
		ewl_attach_parent_setup(w);

	if (data)
	{
		Ewl_Attach *attach;

		attach = ewl_attach_new(t, EWL_ATTACH_DATA_TYPE_TEXT, 
							(void *)data);
		if (attach)
			ewl_attach_list_add(w->attach, w, attach);
	}
	else
		ewl_attach_list_del(w->attach, t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to attach the widget too
 * @param t: The type of data being attached
 * @param data: The wiget to attach
 * @return Returns no value
 * @brief Attaches a widget @p data to the widget @p w
 */
void
ewl_attach_widget_set(Ewl_Widget *w, Ewl_Attach_Type t, Ewl_Widget *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	DCHECK_TYPE("data", data, EWL_WIDGET_TYPE);

	if (!w->attach)
		ewl_attach_parent_setup(w);

	if (data)
	{
		Ewl_Attach *attach;

		attach = ewl_attach_new(t, EWL_ATTACH_DATA_TYPE_WIDGET, 
							(void *)data);
		if (attach)
			ewl_attach_list_add(w->attach, w, attach);
	}
	else
		ewl_attach_list_del(w->attach, t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to attach the data too
 * @param t: The type of attachment
 * @param data: The data to attach
 * @return Returns no value
 * @brief Attaches the data @p data to the widget @p w with the attache type
 * of @p t
 */
void
ewl_attach_other_set(Ewl_Widget *w, Ewl_Attach_Type t, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (!w->attach)
		ewl_attach_parent_setup(w);

	if (data)
	{
		Ewl_Attach *attach;

		attach = ewl_attach_new(t, EWL_ATTACH_DATA_TYPE_OTHER, data);
		if (attach)
			ewl_attach_list_add(w->attach, w, attach);
	}
	else
		ewl_attach_list_del(w->attach, t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to get the attachment from
 * @param t: The type of attachment to get
 * @return Returns the data for the given attachment type
 * @brief Get the attachment of type @p t from the widget @p w
 */
void *
ewl_attach_get(Ewl_Widget *w, Ewl_Attach_Type t)
{
	Ewl_Attach *attach;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, NULL);

	if (!w->attach) 
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	attach = ewl_attach_list_get(w->attach, t);
	if (attach)
	{
		DRETURN_PTR(attach->data, DLEVEL_STABLE);
	}
	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

static Ewl_Attach_List *
ewl_attach_list_new(void)
{
	Ewl_Attach_List *list;

	DENTER_FUNCTION(DLEVEL_STABLE);

	list = NEW(Ewl_Attach_List, 1);

	DRETURN_PTR(list, DLEVEL_STABLE);
}

static void
ewl_attach_list_free(Ewl_Attach_List *list)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("list", list);

	if (list->direct)
		ewl_attach_list_del(list, EWL_ATTACH(list->list)->type);
	else
	{
		while (list->len)
		{
			if (!list->direct)
				ewl_attach_list_del(list, 
					EWL_ATTACH(list->list[0])->type);
			else
				ewl_attach_list_del(list, 
					EWL_ATTACH(list->list)->type);
		}
	}
	FREE(list);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_attach_list_add(Ewl_Attach_List *list, Ewl_Widget *parent, Ewl_Attach *attach)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("list", list);
	DCHECK_PARAM_PTR("attach", attach);

	if (!list->len)
	{
		list->len = 1;
		list->direct = 1;
		list->list = (void *)attach;

		ewl_attach_attach_type_setup(parent, attach);
		DRETURN(DLEVEL_STABLE);
	}
	else if (list->direct)
	{
		Ewl_Attach *tmp;
		tmp = EWL_ATTACH(list->list);

		/* replace if the same type */
		if (tmp->type == attach->type)
		{
			ewl_attach_free(tmp);
			list->list = (void *)attach;

			ewl_attach_attach_type_setup(parent, attach);
			DRETURN(DLEVEL_STABLE);
		}

		list->list = malloc(sizeof(void *));
		list->list[0] = tmp;
		list->direct = 0;
	}
	else
	{
		int i;
		Ewl_Attach *tmp;

		/* replace if in list already */
		for (i = 0; i < list->len; i++)
		{
			tmp = EWL_ATTACH(list->list[i]);
			if (tmp->type == attach->type)
			{
				ewl_attach_free(tmp);
				list->list[i] = attach;

				ewl_attach_attach_type_setup(parent, attach);
				DRETURN(DLEVEL_STABLE);
			}
		}
	}

	list->len ++;
	list->list = realloc(list->list, list->len * sizeof(void *));
	list->list[list->len - 1] = attach;

	ewl_attach_attach_type_setup(parent, attach);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_attach_attach_type_setup(Ewl_Widget *w, Ewl_Attach *attach)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("attach", attach);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	switch (attach->type)
	{
		case EWL_ATTACH_TYPE_TOOLTIP:
			ewl_attach_tooltip_attach(w, attach);
			break;

		case EWL_ATTACH_TYPE_COLOR:
		case EWL_ATTACH_TYPE_NAME:
		default:
			break;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param list: The Ewl_Attach_List to delete
 * @param type: The Ewl_Attach_Type to delete
 * @return Returns no value
 * @brief Deletes the given type @p type from the list @p list
 */
void
ewl_attach_list_del(Ewl_Attach_List *list, Ewl_Attach_Type type)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("list", list);

	if (!list->len)
	{
		DRETURN(DLEVEL_STABLE);
	}
	else if (list->direct)
	{
		Ewl_Attach *tmp;

		tmp = EWL_ATTACH(list->list);
		if (tmp->type == type)
		{
			ewl_attach_free(tmp);
			list->len --;
			list->direct = 0;
			list->list = NULL;
		}
		DRETURN(DLEVEL_STABLE);
	}
	else
	{
		int i;
		Ewl_Attach *tmp;

		for (i = 0; i < list->len; i++)
		{
			tmp = EWL_ATTACH(list->list[i]);

			if (tmp->type == type)
			{
				ewl_attach_free(tmp);
				list->len --;

				/* if not the last entry */
				if (i != list->len)
					memmove(list->list + i, 
						list->list + i + 1, 
						list->len * sizeof(void *));

				list->list = realloc(list->list, 
						list->len * sizeof(void *));
			}
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void *
ewl_attach_list_get(Ewl_Attach_List *list, Ewl_Attach_Type type)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("list", list, NULL);

	if (!list->len)
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}
	else if (list->direct)
	{
		Ewl_Attach *tmp;

		tmp = EWL_ATTACH(list->list);
		if (tmp->type == type)
		{
			DRETURN_PTR(tmp, DLEVEL_STABLE);
		}
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}
	else
	{
		int i;
		Ewl_Attach *tmp;

		for (i = 0; i < list->len; i++)
		{
			tmp = EWL_ATTACH(list->list[i]);

			if (tmp->type == type)
			{
				DRETURN_PTR(tmp, DLEVEL_STABLE);
			}
		}
	}
	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

static Ewl_Attach *
ewl_attach_new(Ewl_Attach_Type t, Ewl_Attach_Data_Type dt, void *data)
{
	Ewl_Attach *attach;

	DENTER_FUNCTION(DLEVEL_STABLE);

	attach = NEW(Ewl_Attach, 1);
	if (!attach)
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_attach_init(attach, t, dt, data))
	{
		FREE(attach);
	}

	DRETURN_PTR(attach, DLEVEL_STABLE);
}

static int
ewl_attach_init(Ewl_Attach *attach, Ewl_Attach_Type t, 
			Ewl_Attach_Data_Type dt, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("attach", attach, FALSE);

	attach->type = t;

	if (dt == EWL_ATTACH_DATA_TYPE_TEXT)
		attach->data = strdup(data);
	else
		attach->data = data;
	attach->data_type = dt;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static void
ewl_attach_free(Ewl_Attach *attach)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("attach", attach);

	/* XXX should we clean up _WIDGET in here? */

	if ((attach->data_type == EWL_ATTACH_DATA_TYPE_TEXT)
			|| (attach->type == EWL_ATTACH_DATA_TYPE_OTHER)) {
		IF_FREE(attach->data);
	}
		

	IF_FREE(attach);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_attach_parent_setup(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	w->attach = ewl_attach_list_new();
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, 
				ewl_attach_cb_parent_destroy, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_attach_cb_parent_destroy(Ewl_Widget *w, void *ev __UNUSED__, 
				void *data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* make sure the timer gets cleaned up if the widget goes away */
	if ((ewl_attach_tooltip) && (w == ewl_attach_tooltip->to) 
			&& (ewl_attach_tooltip->timer)) {
		ecore_timer_del(ewl_attach_tooltip->timer);
		ewl_attach_tooltip->timer = NULL;
	}

	if (w->attach)
		ewl_attach_list_free(w->attach);
	w->attach = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_attach_tooltip_attach(Ewl_Widget *w, Ewl_Attach *attach)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("attach", attach);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
				ewl_attach_cb_tooltip_mouse_move, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
				ewl_attach_cb_tooltip_mouse_down, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_OUT,
				ewl_attach_cb_tooltip_mouse_out, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_attach_tooltip_detach(Ewl_Attach *attach)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("attach", attach);

	/* make sure the display attach is our attach */
	if (ewl_attach_tooltip->attach != attach)
	{
		DRETURN(DLEVEL_STABLE);
	}

	if (ewl_attach_tooltip)
	{
		if (ewl_attach_tooltip->timer)
			ecore_timer_del(ewl_attach_tooltip->timer);

		ewl_attach_tooltip->timer = NULL;
		ewl_attach_tooltip->to = NULL;
		ewl_attach_tooltip->x = 0;
		ewl_attach_tooltip->y = 0;

		/* hide window if needed */
		if (ewl_attach_tooltip->win && (VISIBLE(ewl_attach_tooltip->win)))
			ewl_widget_hide(ewl_attach_tooltip->win);

		/* cleanup the display window */
		if (ewl_attach_tooltip->box)
		{
			if (attach->data_type == EWL_ATTACH_DATA_TYPE_TEXT)
			{
				ewl_widget_destroy(ewl_attach_tooltip->box);
				ewl_attach_tooltip->box = NULL;
			}
			else
				ewl_container_child_remove(
					EWL_CONTAINER(ewl_attach_tooltip->box), 
					EWL_WIDGET(attach->data));
		}
		ewl_attach_tooltip->attach = NULL;
	}
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_attach_cb_tooltip_mouse_move(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
	Ewl_Attach *attach;
	Ewl_Embed *emb;
	Ewl_Event_Mouse_Move *e;
//	int x, y;
	int offset;
	char *delay_str;
	double delay;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev", ev);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	e = ev;
	attach = ewl_attach_list_get(w->attach, EWL_ATTACH_TYPE_TOOLTIP);
	if (!attach)
		DRETURN(DLEVEL_STABLE);

	if (!ewl_attach_tooltip)
		ewl_attach_tooltip = NEW(Ewl_Attach_Tooltip, 1);

	ewl_attach_tooltip_detach(attach);

	ewl_attach_tooltip->attach = attach;
	ewl_attach_tooltip->to = w;

	offset = ewl_theme_data_int_get(w, "/tooltip/offset");

	emb = ewl_embed_widget_find(w);

	/*
	 * Position the tooltip on the side with the most available space
	 */
	if ((e->x - CURRENT_X(emb)) > (CURRENT_X(emb) + CURRENT_W(emb) - e->x))
		ewl_attach_tooltip->x = e->x - offset;
	else
		ewl_attach_tooltip->x = e->x + offset;

	if ((e->y - CURRENT_Y(emb)) > (CURRENT_Y(emb) + CURRENT_H(emb) - e->y))
		ewl_attach_tooltip->y = e->y - offset;
	else
		ewl_attach_tooltip->y = e->y + offset;

#if 0 
	emb = ewl_embed_widget_find(w);
	ewl_window_position_get(EWL_WINDOW(emb), &x, &y);

	ewl_attach_tooltip->x = x + CURRENT_X(w) + e->x + offset;
	ewl_attach_tooltip->y = y + CURRENT_Y(w) + e->y + offset;
#endif

	delay_str = ewl_theme_data_str_get(w, "/tooltip/delay");
	if (delay_str)
	{
		delay = atof(delay_str);
		FREE(delay_str)
	}
	else
		delay = 1.0;

	ewl_attach_tooltip->timer = ecore_timer_add(delay, 
					ewl_attach_cb_tooltip_timer, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_attach_cb_tooltip_mouse_down(Ewl_Widget *w __UNUSED__, 
				void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Attach *attach;

	DENTER_FUNCTION(DLEVEL_STABLE);

	attach = ewl_attach_list_get(w->attach, EWL_ATTACH_TYPE_TOOLTIP);
	if (!attach)
		DRETURN(DLEVEL_STABLE);

	ewl_attach_tooltip_detach(attach);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_attach_cb_tooltip_mouse_out(Ewl_Widget *w __UNUSED__, 
				void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Attach *attach;

	DENTER_FUNCTION(DLEVEL_STABLE);

	attach = ewl_attach_list_get(w->attach, EWL_ATTACH_TYPE_TOOLTIP);
	if (!attach)
		DRETURN(DLEVEL_STABLE);

	ewl_attach_tooltip_detach(attach);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_attach_cb_tooltip_timer(void *data)
{
	Ewl_Widget *w;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, FALSE);

	/* we are returning false so that will shutdown the timer, just need
	 * to make sure we don't try to del it a second time so set it NULL */
	ewl_attach_tooltip->timer = NULL;

	w = data;
	emb = ewl_embed_widget_find(w);
	if (!emb)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	if (!(ewl_attach_tooltip->win))
	{
		Ewl_Container *redir;

		ewl_attach_tooltip->embed = EWL_WIDGET(emb);
#if 0
		ewl_attach_tooltip->win = ewl_window_new();
		ewl_window_title_set(EWL_WINDOW(ewl_attach_tooltip->win), 
								"Tooltip");
		ewl_window_name_set(EWL_WINDOW(ewl_attach_tooltip->win), 
								"Tooltip");
		ewl_window_class_set(EWL_WINDOW(ewl_attach_tooltip->win), 
								"Tooltip");
		ewl_window_transient_for(EWL_WINDOW(emb), 
					EWL_WINDOW(ewl_attach_tooltip->win));
		ewl_window_borderless_set(EWL_WINDOW(ewl_attach_tooltip->win));
		ewl_window_raise(EWL_WINDOW(ewl_attach_tooltip->win));
#endif

		/* XXX this should really be in it's own window */
		ewl_attach_tooltip->win = ewl_hbox_new();

		/*
		 * Temporarily override any redirect settings as this must go
		 * into the top level of the embed.
		 */
		redir = ewl_container_redirect_get(EWL_CONTAINER(emb));
		ewl_container_redirect_set(EWL_CONTAINER(emb), NULL);
		ewl_container_child_append(EWL_CONTAINER(emb), 
						ewl_attach_tooltip->win);
		ewl_container_redirect_set(EWL_CONTAINER(emb), redir);

		ewl_widget_layer_top_set(ewl_attach_tooltip->win, TRUE);

		ewl_callback_prepend(ewl_attach_tooltip->win, 
					EWL_CALLBACK_DESTROY,
					ewl_attach_cb_tooltip_win_destroy, 
					NULL);
		ewl_object_fill_policy_set(EWL_OBJECT(ewl_attach_tooltip->win), 
							EWL_FLAG_FILL_NONE);

		ewl_widget_appearance_set(ewl_attach_tooltip->win,
						EWL_ATTACH_TOOLTIP_TYPE);
		ewl_widget_inherit(ewl_attach_tooltip->win, 
						EWL_ATTACH_TOOLTIP_TYPE);
	}
	else
	{
		if ((!ewl_attach_tooltip->embed) 
				|| (ewl_attach_tooltip->embed 
							!= EWL_WIDGET(emb))) 
		{
			if (ewl_attach_tooltip->embed 
					&& (ewl_attach_tooltip->embed 
							!= EWL_WIDGET(emb))) 
			{
				ewl_container_child_remove(
					EWL_CONTAINER(ewl_attach_tooltip->embed),
					ewl_attach_tooltip->win);
			} 

			ewl_attach_tooltip->embed = EWL_WIDGET(emb);
			ewl_container_child_append(EWL_CONTAINER(emb), 
				ewl_attach_tooltip->win);
		}
	}

	if (!(ewl_attach_tooltip->box))
	{
		ewl_attach_tooltip->box = ewl_hbox_new();
		ewl_container_child_append(EWL_CONTAINER(ewl_attach_tooltip->win), 
							ewl_attach_tooltip->box);
		ewl_object_fill_policy_set(EWL_OBJECT(ewl_attach_tooltip->box), 
							EWL_FLAG_FILL_NONE);
	}

	if (ewl_attach_tooltip->attach->data_type == EWL_ATTACH_DATA_TYPE_WIDGET)
	{
		ewl_container_child_append(EWL_CONTAINER(ewl_attach_tooltip->box), 
					EWL_WIDGET(ewl_attach_tooltip->attach->data));
		ewl_widget_show(EWL_WIDGET(ewl_attach_tooltip->attach->data));
	}
	else
	{
		Ewl_Widget *o;

		o = ewl_text_new();
		ewl_text_text_set(EWL_TEXT(o), ewl_attach_tooltip->attach->data);
		ewl_container_child_append(EWL_CONTAINER(ewl_attach_tooltip->box), o);
		ewl_widget_show(o);
	}

	ewl_object_position_request(EWL_OBJECT(ewl_attach_tooltip->win), 
					ewl_attach_tooltip->x, ewl_attach_tooltip->y);

	ewl_widget_show(ewl_attach_tooltip->win);
	ewl_widget_show(ewl_attach_tooltip->box);

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

static void
ewl_attach_cb_tooltip_win_destroy(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
					void *data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_attach_tooltip->embed = NULL;
	ewl_attach_tooltip->win = NULL;
	ewl_attach_tooltip->box = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

