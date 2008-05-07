/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_menu.h"
#include "ewl_context_menu.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

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
        DCHECK_PARAM_PTR_RET(menu, FALSE);

        /*
         * Initialize the defaults of the inherited fields.
         */
        if (!ewl_menu_item_init(EWL_MENU_ITEM(menu)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(menu), "menu_container");
        ewl_widget_inherit(EWL_WIDGET(menu), EWL_MENU_TYPE);

        /*
         * add the callbacks
         */
        ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_MOUSE_MOVE,
                            ewl_menu_cb_mouse_move, NULL);
        ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_FOCUS_IN,
                            ewl_menu_cb_expand, NULL);
        ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_CONFIGURE,
                            ewl_menu_cb_configure, NULL);
        ewl_callback_prepend(EWL_WIDGET(menu), EWL_CALLBACK_DESTROY,
                                ewl_menu_cb_destroy, NULL);

        /*
         * Create the popup menu portion of the widget.
         */
        menu->popup = ewl_context_menu_new();
        ewl_popup_follow_set(EWL_POPUP(menu->popup), EWL_WIDGET(menu));
        ewl_popup_type_set(EWL_POPUP(menu->popup),
                                        EWL_POPUP_TYPE_MENU_VERTICAL);
        ewl_widget_internal_set(menu->popup, TRUE);
        ewl_widget_appearance_set(EWL_WIDGET(menu->popup), EWL_MENU_TYPE);

        /* redirect the menu container to the popup menu */
        ewl_container_redirect_set(EWL_CONTAINER(menu),
                                        EWL_CONTAINER(menu->popup));
        ewl_callback_prepend(menu->popup, EWL_CALLBACK_DESTROY,
                                ewl_menu_cb_popup_destroy, menu);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param menu: The menu to setup
 * @param info: The info to set into the menu
 * @return Returns no value
 * @brief Initializes @a menu with @a info
 */
void
ewl_menu_from_info(Ewl_Menu *menu, Ewl_Menu_Info *info)
{
        int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(menu);
        DCHECK_PARAM_PTR(info);
        DCHECK_TYPE(menu, EWL_MENU_TYPE);

        for (i = 0; info[i].name != NULL; i++)
        {
                Ewl_Widget *item;

                item = ewl_menu_item_new();
                ewl_button_label_set(EWL_BUTTON(item), info[i].name);
                ewl_button_image_set(EWL_BUTTON(item), info[i].img, NULL);
                ewl_container_child_append(EWL_CONTAINER(menu), item);

                if (info[i].cb)
                        ewl_callback_append(item, EWL_CALLBACK_CLICKED,
                                                        info[i].cb, NULL);
                ewl_widget_show(item);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param menu: the menu to work with
 * @return Returns no value
 * @brief Collapses the popup portion of the menu
 */
void
ewl_menu_collapse(Ewl_Menu *menu)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(menu);
        DCHECK_TYPE(menu, EWL_MENU_TYPE);

        ewl_widget_hide(menu->popup);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_menu_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Menu *menu;
        Ewl_Box *parent;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_MENU_TYPE);

        menu = EWL_MENU(w);
        parent = EWL_BOX(menu->menubar_parent);

        if ((parent && (ewl_box_orientation_get(parent)
                                == EWL_ORIENTATION_VERTICAL))
                        || EWL_MENU_ITEM(menu)->inmenu)
                ewl_popup_type_set(EWL_POPUP(menu->popup),
                                        EWL_POPUP_TYPE_MENU_HORIZONTAL);
        else
                ewl_popup_type_set(EWL_POPUP(menu->popup),
                                        EWL_POPUP_TYPE_MENU_VERTICAL);

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
ewl_menu_cb_mouse_move(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Menu *menu;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_MENU_TYPE);

        menu = EWL_MENU(w);
        if (menu->menubar_parent) {
                Ewl_Menu *sub, *hide_menu = NULL;
                Ewl_Container *bar;

                bar = EWL_CONTAINER(menu->menubar_parent);
                ewl_container_child_iterate_begin(bar);
                while ((sub = EWL_MENU(ewl_container_child_next(bar)))) {
                        if ((sub != EWL_MENU(w))
                                        && EWL_MENU_IS(sub)
                                        && (sub->popup)
                                        && VISIBLE(sub->popup)) {
                                hide_menu = sub;
                                break;
                        }
                }

                if (hide_menu) {
                        ewl_widget_hide(hide_menu->popup);
                        ewl_callback_call(w, EWL_CALLBACK_FOCUS_IN);
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
 * @brief The expand callback
 */
void
ewl_menu_cb_expand(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Menu *menu;
        Ewl_Menu_Item *item;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_MENU_TYPE);

        menu = EWL_MENU(w);
        item = EWL_MENU_ITEM(w);

        ewl_widget_show(menu->popup);
        ewl_window_raise(EWL_WINDOW(menu->popup));

        if (item->inmenu) {
                Ewl_Context_Menu *cm;

                cm = EWL_CONTEXT_MENU(item->inmenu);
                cm->open_menu = EWL_WIDGET(menu);
        }
        else
                ewl_widget_focus_send(menu->popup);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The popup destroy callback
 */
void
ewl_menu_cb_popup_destroy(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data)
{
        Ewl_Menu *m;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_MENU_TYPE);

        /* We need to set the popup to NULL here in case the popup gets
         * freed before the menu does. This makes sure we don't segv in the
         * ewl_menu_cb_destroy function when we try to delete the callback
         * on the deleted popup widget from our dangling reference */
        m = data;
        if (m->popup) m->popup = NULL;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_menu_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Menu *menu;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_MENU_TYPE);

        menu = EWL_MENU(w);
        if (menu->popup) {
                ewl_callback_del(menu->popup, EWL_CALLBACK_DESTROY,
                                                ewl_menu_cb_popup_destroy);
                ewl_widget_destroy(menu->popup);
                menu->popup = NULL;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int
ewl_menu_mouse_feed(Ewl_Menu *menu, int x, int y)
{
        int emb_x = 0, emb_y = 0, emb_w = 0, emb_h = 0;

        DCHECK_PARAM_PTR_RET(menu, FALSE);
        DCHECK_TYPE_RET(menu, EWL_MENU_TYPE, FALSE);

        ewl_embed_window_position_get(EWL_EMBED(menu->popup), &emb_x, &emb_y);
        ewl_object_current_size_get(EWL_OBJECT(menu->popup), &emb_w, &emb_h);

        x -= emb_x;
        y -= emb_y;

        if ((x > 0) && (y > 0) && (x <= emb_w) && (y <= emb_h))
        {
                ewl_embed_mouse_move_feed(EWL_EMBED(menu->popup), x, y, 0);
                DRETURN_INT(TRUE, DLEVEL_STABLE);
        }

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

