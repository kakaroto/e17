/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_box.h"
#include "ewl_popup.h"
#include "ewl_label.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

#include <Evas.h>

/**
 * Ewl_Attach_Tooltip
 */
typedef struct Ewl_Attach_Tooltip Ewl_Attach_Tooltip;

/**
 * @brief Inherits from Ewl_Widget and extends to provide information on a
 * tooltip
 */
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
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

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
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(data, EWL_WIDGET_TYPE);

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
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

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
 * @param w: The widget to attach the dnd data too
 * @param c: The cursor to display during drag.
 * @param data: The data to transfer on drop.
 * @param size: The amount of data send
 * @return Returns no value
 * @brief Attaches the DND data @p data to the widget @p w with the displayed
 * cursor @p c.
 */
void
ewl_attach_dnd_drag_set(Ewl_Widget *w, Ewl_Widget *c, void *data, int size)
{
        Ewl_Attach_Dnd *dnd_data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        dnd_data = ewl_attach_get(w, EWL_ATTACH_TYPE_DND_DATA);
        if (!c && !data) {
                if (dnd_data)
                        FREE(dnd_data);
        }
        else {
                if (!dnd_data)
                        dnd_data = NEW(Ewl_Attach_Dnd, 1);
                dnd_data->cursor = c;
                dnd_data->data = data;
                dnd_data->size = size;
        }

        ewl_attach_other_set(w, EWL_ATTACH_TYPE_DND_DATA, dnd_data);

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
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

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
        DCHECK_PARAM_PTR(list);

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
        DCHECK_PARAM_PTR(list);
        DCHECK_PARAM_PTR(attach);

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
                unsigned int i;
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
        REALLOC(list->list, void *, list->len);
        list->list[list->len - 1] = attach;

        ewl_attach_attach_type_setup(parent, attach);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_attach_attach_type_setup(Ewl_Widget *w, Ewl_Attach *attach)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(attach);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

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
        DCHECK_PARAM_PTR(list);

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
                unsigned int i;
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

                                REALLOC(list->list, void *, list->len);
                        }
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void *
ewl_attach_list_get(Ewl_Attach_List *list, Ewl_Attach_Type type)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(list, NULL);

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
                unsigned int i;
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
        DCHECK_PARAM_PTR_RET(attach, FALSE);

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
        DCHECK_PARAM_PTR(attach);

        /* XXX should we clean up _WIDGET in here? */

        if ((attach->data_type == EWL_ATTACH_DATA_TYPE_TEXT))
        {
                IF_FREE(attach->data);
        }


        IF_FREE(attach);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_attach_parent_setup(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

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
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /* make sure the timer gets cleaned up if the widget goes away */
        if ((ewl_attach_tooltip) && (w == ewl_attach_tooltip->to)
                        && (ewl_attach_tooltip->timer))
        {
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
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(attach);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
                                ewl_attach_cb_tooltip_mouse_move, NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
                                ewl_attach_cb_tooltip_mouse_down, NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_OUT,
                                ewl_attach_cb_tooltip_mouse_out, NULL);
        ewl_callback_append(w, EWL_CALLBACK_OBSCURE,
                                ewl_attach_cb_tooltip_mouse_out, NULL);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_attach_tooltip_detach(Ewl_Attach *attach)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(attach);

        /* make sure the display attach is our attach */
        if (!ewl_attach_tooltip || (ewl_attach_tooltip->attach != attach))
                DRETURN(DLEVEL_STABLE);

        if (ewl_attach_tooltip->timer)
                ecore_timer_del(ewl_attach_tooltip->timer);

        ewl_attach_tooltip->timer = NULL;
        ewl_attach_tooltip->to = NULL;
        ewl_attach_tooltip->x = 0;
        ewl_attach_tooltip->y = 0;

        /* cleanup the display window */
        if (ewl_attach_tooltip->box)
        {
                if (attach->data_type != EWL_ATTACH_DATA_TYPE_TEXT)
                        ewl_container_child_remove(
                                        EWL_CONTAINER(ewl_attach_tooltip->box),
                                        EWL_WIDGET(attach->data));

                ewl_widget_destroy(ewl_attach_tooltip->box);
                ewl_attach_tooltip->box = NULL;
        }

        /* destroy window if needed */
        if (ewl_attach_tooltip->win && VISIBLE(ewl_attach_tooltip->win))
                ewl_widget_hide(ewl_attach_tooltip->win);

        ewl_attach_tooltip->attach = NULL;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_attach_cb_tooltip_mouse_move(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Attach *attach;
        Ewl_Event_Mouse *e;
        const char *delay_str;
        double delay = 1.0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        e = ev;
        attach = ewl_attach_list_get(w->attach, EWL_ATTACH_TYPE_TOOLTIP);
        if (!attach)
                DRETURN(DLEVEL_STABLE);

        if (!ewl_attach_tooltip)
                ewl_attach_tooltip = NEW(Ewl_Attach_Tooltip, 1);

        /* we only want to kill this tooltip if the move is outside
         * the move tolerance */
        if (ewl_attach_tooltip && (ewl_attach_tooltip->attach == attach))
        {
                int amt = 0;

                amt = ewl_theme_data_int_get(w, "/tooltip/tolerance");
                if ((e->x >= (ewl_attach_tooltip->x - amt))
                                && (e->x <= (ewl_attach_tooltip->x + amt))
                                && (e->y >= (ewl_attach_tooltip->y - amt))
                                && (e->y <= (ewl_attach_tooltip->y + amt)))
                        DRETURN(DLEVEL_STABLE);
        }

        ewl_attach_tooltip_detach(attach);

        ewl_attach_tooltip->attach = attach;
        ewl_attach_tooltip->to = w;

        ewl_attach_tooltip->x = e->x;
        ewl_attach_tooltip->y = e->y;

        delay_str = (const char *)ewl_theme_data_str_get(w, "/tooltip/delay");
        if (delay_str) delay = atof(delay_str);

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
        if (!attach) DRETURN(DLEVEL_STABLE);
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
        if (!attach) DRETURN(DLEVEL_STABLE);
        ewl_attach_tooltip_detach(attach);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_attach_cb_tooltip_timer(void *data)
{
        Ewl_Widget *w;
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, ECORE_CALLBACK_CANCEL);

        /* we are returning false so that will shutdown the timer, just need
         * to make sure we don't try to del it a second time so set it NULL */
        ewl_attach_tooltip->timer = NULL;

        w = data;
        emb = ewl_embed_widget_find(w);
        if (!emb)
                DRETURN_INT(ECORE_CALLBACK_CANCEL, DLEVEL_STABLE);

        if (!(ewl_attach_tooltip->win))
        {
                int oh, ov;

                ewl_attach_tooltip->win = ewl_popup_new();
                ewl_popup_type_set(EWL_POPUP(ewl_attach_tooltip->win),
                                        EWL_POPUP_TYPE_MOUSE);
                ewl_popup_follow_set(EWL_POPUP(ewl_attach_tooltip->win),
                                                        EWL_WIDGET(emb));
                ewl_attach_tooltip->embed = EWL_WIDGET(emb);
                ewl_callback_prepend(ewl_attach_tooltip->win,
                                        EWL_CALLBACK_DESTROY,
                                        ewl_attach_cb_tooltip_win_destroy,
                                        NULL);

                ov = ewl_theme_data_int_get(EWL_WIDGET(w),
                                                        "/tooltip/voffset");
                oh = ewl_theme_data_int_get(EWL_WIDGET(w),
                                                        "/tooltip/hoffset");

                ewl_popup_offset_set(EWL_POPUP(ewl_attach_tooltip->win),
                                                oh, ov);
                ewl_widget_appearance_set(ewl_attach_tooltip->win,
                                                EWL_ATTACH_TOOLTIP_TYPE);
                ewl_widget_inherit(ewl_attach_tooltip->win,
                                                EWL_ATTACH_TOOLTIP_TYPE);
        }
        else
        {
                /* see if we need to add to the embed */
                if ((!ewl_attach_tooltip->embed)
                                || (ewl_attach_tooltip->embed
                                                        != EWL_WIDGET(emb)))
                {
                        ewl_attach_tooltip->embed = EWL_WIDGET(emb);
                        ewl_popup_follow_set(EWL_POPUP(ewl_attach_tooltip->win),
                                                ewl_attach_tooltip->embed);
                }
        }
        ewl_widget_show(ewl_attach_tooltip->win);

        if (!(ewl_attach_tooltip->box))
        {
                ewl_attach_tooltip->box = ewl_hbox_new();
                ewl_container_child_append(
                                EWL_CONTAINER(ewl_attach_tooltip->win),
                                                ewl_attach_tooltip->box);
        }
        ewl_widget_show(ewl_attach_tooltip->box);

        if (ewl_attach_tooltip->attach->data_type ==
                        EWL_ATTACH_DATA_TYPE_WIDGET)
        {
                ewl_container_child_append(
                                EWL_CONTAINER(ewl_attach_tooltip->box),
                                EWL_WIDGET(ewl_attach_tooltip->attach->data));

                ewl_widget_show(EWL_WIDGET(ewl_attach_tooltip->attach->data));
        }
        else
        {
                Ewl_Widget *o;

                o = ewl_label_new();
                ewl_label_text_set(EWL_LABEL(o),
                                ewl_attach_tooltip->attach->data);
                ewl_container_child_append(
                                EWL_CONTAINER(ewl_attach_tooltip->box), o);
                ewl_widget_show(o);
        }

        ewl_popup_mouse_position_set(EWL_POPUP(ewl_attach_tooltip->win),
                                        ewl_attach_tooltip->x,
                                        ewl_attach_tooltip->y);

        DRETURN_INT(ECORE_CALLBACK_CANCEL, DLEVEL_STABLE);
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

