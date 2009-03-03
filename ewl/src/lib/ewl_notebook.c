/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_notebook.h"
#include "ewl_label.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns a newly allocated notebook on success. NULL on failure
 * @brief Create a new notebook widget
 */
Ewl_Widget *
ewl_notebook_new(void)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Notebook, 1);
        if (!w)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_notebook_init(EWL_NOTEBOOK(w)))
        {
                ewl_widget_destroy(w);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook widget to initialize
 * @return Returns TRUE on success or NULL on failure.
 * @brief Initialize a notebook to default values and callbacks
 */
int
ewl_notebook_init(Ewl_Notebook *n)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(n, FALSE);

        if (!ewl_box_init(EWL_BOX(n)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_box_orientation_set(EWL_BOX(n), EWL_ORIENTATION_VERTICAL);

        ewl_widget_appearance_set(EWL_WIDGET(n), EWL_NOTEBOOK_TYPE);
        ewl_widget_inherit(EWL_WIDGET(n), EWL_NOTEBOOK_TYPE);

        n->tabbar_position = EWL_POSITION_TOP;

        n->body.tabbar = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(n), n->body.tabbar);
        ewl_object_fill_policy_set(EWL_OBJECT(n->body.tabbar),
                                EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(n->body.tabbar),
                                                EWL_FLAG_ALIGN_CENTER);
        ewl_widget_internal_set(n->body.tabbar, TRUE);
        ewl_widget_appearance_set(n->body.tabbar, "top/tabbar");
        ewl_widget_show(n->body.tabbar);

        n->body.pages = ewl_vbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(n->body.pages),
                                   EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(n), n->body.pages);
        ewl_widget_internal_set(n->body.pages, TRUE);
        ewl_widget_layer_priority_set(n->body.pages, -1);
        ewl_widget_appearance_set(n->body.pages, "pages");
        ewl_widget_show(n->body.pages);

        ewl_container_redirect_set(EWL_CONTAINER(n),
                                        EWL_CONTAINER(n->body.pages));

        ewl_container_show_notify_set(EWL_CONTAINER(n->body.pages),
                                        ewl_notebook_cb_child_show);
        ewl_container_hide_notify_set(EWL_CONTAINER(n->body.pages),
                                        ewl_notebook_cb_child_hide);
        ewl_container_add_notify_set(EWL_CONTAINER(n->body.pages),
                                        ewl_notebook_cb_child_add);
        ewl_container_remove_notify_set(EWL_CONTAINER(n->body.pages),
                                        ewl_notebook_cb_child_remove);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to set the tabbar alignment of
 * @param align: The Ewl_Alignment to set the alignment too
 * @return Returns no value.
 * @brief Set the alignment of the tabbar in the notebook widget
 */
void
ewl_notebook_tabbar_alignment_set(Ewl_Notebook *n, unsigned int align)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(n);
        DCHECK_TYPE(n, EWL_NOTEBOOK_TYPE);

        ewl_object_alignment_set(EWL_OBJECT(n->body.tabbar), align);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to get the alignment from
 * @return Returns the current alignment values of the widget
 * @brief Retrieves the alignment of the tabbar in the notebook widget
 */
unsigned int
ewl_notebook_tabbar_alignment_get(Ewl_Notebook *n)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(n, 0);
        DCHECK_TYPE_RET(n, EWL_NOTEBOOK_TYPE, 0);

        DRETURN_INT(ewl_object_alignment_get(EWL_OBJECT(n->body.tabbar)),
                                                        DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to set the tabbar position of
 * @param pos: The Ewl_Position to put the tabbar in
 * @return Returns no value.
 * @brief Set the position of the tabbar in the notebook widget
 */
void
ewl_notebook_tabbar_position_set(Ewl_Notebook *n, Ewl_Position pos)
{
        int t, pw, ph;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(n);
        DCHECK_TYPE(n, "notebook");

        if (n->tabbar_position == pos)
                DRETURN(DLEVEL_STABLE);

        n->tabbar_position = pos;
        switch(pos)
        {
                case EWL_POSITION_LEFT:
                case EWL_POSITION_RIGHT:
                        ewl_box_orientation_set(EWL_BOX(n),
                                                EWL_ORIENTATION_HORIZONTAL);
                        ewl_box_orientation_set(EWL_BOX(n->body.tabbar),
                                                EWL_ORIENTATION_VERTICAL);

                        t = ewl_object_preferred_w_get(EWL_OBJECT(n->body.tabbar));
                        ewl_object_preferred_size_get(EWL_OBJECT(n->body.pages),
                                                                        &pw, &ph);

                        ewl_object_preferred_inner_size_set(EWL_OBJECT(n), t + pw, ph);

                        break;
                case EWL_POSITION_TOP:
                case EWL_POSITION_BOTTOM:
                default:
                        ewl_box_orientation_set(EWL_BOX(n),
                                                EWL_ORIENTATION_VERTICAL);
                        ewl_box_orientation_set(EWL_BOX(n->body.tabbar),
                                                EWL_ORIENTATION_HORIZONTAL);

                        t = ewl_object_preferred_h_get(EWL_OBJECT(n->body.tabbar));
                        ewl_object_preferred_size_get(EWL_OBJECT(n->body.pages),
                                                                        &pw, &ph);

                        ewl_object_preferred_inner_size_set(EWL_OBJECT(n), pw, ph + t);

                        break;
        }

        switch (pos)
        {
                case EWL_POSITION_LEFT:
                        ewl_widget_appearance_set(n->body.tabbar, "left/tabbar");
                        break;
                case EWL_POSITION_RIGHT:
                        ewl_widget_appearance_set(n->body.tabbar, "right/tabbar");
                        break;
                case EWL_POSITION_TOP:
                        ewl_widget_appearance_set(n->body.tabbar, "top/tabbar");
                        break;
                case EWL_POSITION_BOTTOM:
                        ewl_widget_appearance_set(n->body.tabbar, "bottom/tabbar");
                        break;
        }

        ewl_container_child_remove(EWL_CONTAINER(n), n->body.tabbar);

        /* remove the redirect so we can stick the tabbar back in */
        ewl_container_redirect_set(EWL_CONTAINER(n), NULL);

        if ((n->tabbar_position == EWL_POSITION_RIGHT)
                        || (n->tabbar_position == EWL_POSITION_BOTTOM))
                ewl_container_child_append(EWL_CONTAINER(n), n->body.tabbar);

        else if ((n->tabbar_position == EWL_POSITION_LEFT)
                        || (n->tabbar_position == EWL_POSITION_TOP))
                ewl_container_child_prepend(EWL_CONTAINER(n), n->body.tabbar);

        ewl_container_redirect_set(EWL_CONTAINER(n),
                                        EWL_CONTAINER(n->body.pages));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to get the tabbar position of
 * @return Returns the current Ewl_Position of the tabbar
 * @brief Get the position of the tabbar in the notebook widget
 */
Ewl_Position
ewl_notebook_tabbar_position_get(Ewl_Notebook *n)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(n, EWL_POSITION_TOP);
        DCHECK_TYPE_RET(n, EWL_NOTEBOOK_TYPE, EWL_POSITION_TOP);

        DRETURN_INT(n->tabbar_position, DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to set the visible flag of the tabbar
 * @param visible: The flag to set for the tabbar visibility
 * @return Returns no value.
 * @brief Set if the tabbar is visible
 */
void
ewl_notebook_tabbar_visible_set(Ewl_Notebook *n, unsigned int visible)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(n);
        DCHECK_TYPE(n, EWL_NOTEBOOK_TYPE);

        if (visible)
        {
                ewl_widget_state_set(EWL_WIDGET(n->body.pages), "tabs",
                                                    EWL_STATE_PERSISTENT);
                ewl_widget_show(n->body.tabbar);
        }
        else
        {
                ewl_widget_state_set(EWL_WIDGET(n->body.pages), "notabs",
                                                    EWL_STATE_PERSISTENT);
                ewl_widget_hide(n->body.tabbar);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to get the tabbar visiblity of
 * @return Returns the visibility of the tabbar
 * @brief Retrieve if the tabbar is visible
 */
unsigned int
ewl_notebook_tabbar_visible_get(Ewl_Notebook *n)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(n, FALSE);
        DCHECK_TYPE_RET(n, EWL_NOTEBOOK_TYPE, FALSE);

        DRETURN_INT(((VISIBLE(n->body.tabbar)) ? TRUE : FALSE),
                                                        DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to set the visible page of
 * @param page: The page to set visible in the tabbar
 * @return Returns no value.
 * @brief Set the current visible page of the notebook
 */
void
ewl_notebook_visible_page_set(Ewl_Notebook *n, Ewl_Widget *page)
{
        Ewl_Widget *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(n);
        DCHECK_PARAM_PTR(page);
        DCHECK_TYPE(n, EWL_NOTEBOOK_TYPE);
        DCHECK_TYPE(page, EWL_WIDGET_TYPE);

        if (page == n->cur_page)
                DRETURN(DLEVEL_STABLE);

        if (n->cur_page)
        {
                Ewl_Widget *w;

                t = ewl_attach_widget_association_get(n->cur_page);
                if (t) ewl_widget_state_set(t, "default", EWL_STATE_PERSISTENT);

                /* make sure we set n->cur_page null first or the hide
                 * callback won't let us hide */
                w = n->cur_page;
                n->cur_page = NULL;
                ewl_widget_hide(w);
        }

        n->cur_page = page;
        ewl_widget_show(n->cur_page);

        t = ewl_attach_widget_association_get(n->cur_page);
        if (t)
                ewl_widget_state_set(t, "selected", EWL_STATE_PERSISTENT);

        ewl_callback_call(EWL_WIDGET(n), EWL_CALLBACK_VALUE_CHANGED);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to get the visible page of
 * @return Returns the visible page of the notebook
 * @brief Get the current visible page of the notebook
 */
Ewl_Widget *
ewl_notebook_visible_page_get(Ewl_Notebook *n)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(n, NULL);
        DCHECK_TYPE_RET(n, EWL_NOTEBOOK_TYPE, NULL);

        DRETURN_PTR(n->cur_page, DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to set the tab text in
 * @param page: The page to associate the tab text too
 * @param text: The text to set in the tab
 * @return Returns no value.
 * @brief Set the text of the tab for the page @p page to the text @p text
 */
void
ewl_notebook_page_tab_text_set(Ewl_Notebook *n, Ewl_Widget *page,
                                                        const char *text)
{
        Ewl_Widget *t = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(n);
        DCHECK_PARAM_PTR(page);
        DCHECK_TYPE(n, EWL_NOTEBOOK_TYPE);
        DCHECK_TYPE(page, EWL_WIDGET_TYPE);

        if (text)
        {
                t = ewl_label_new();
                ewl_label_text_set(EWL_LABEL(t), text);
                ewl_widget_show(t);
        }

        ewl_notebook_page_tab_widget_set(n, page, t);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to work with
 * @param page: The page to get the tab text from
 * @return Returns the text of the pages tab
 * @brief Get the text of the notebook page @p page
 */
const char *
ewl_notebook_page_tab_text_get(Ewl_Notebook *n, Ewl_Widget *page)
{
        Ewl_Widget *o;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(n, EWL_NOTEBOOK_TYPE);
        DCHECK_PARAM_PTR_RET(page, EWL_WIDGET_TYPE);
        DCHECK_TYPE_RET(n, EWL_NOTEBOOK_TYPE, NULL);
        DCHECK_TYPE_RET(page, EWL_WIDGET_TYPE, NULL);

        /* get the label widget */
        o = ewl_notebook_page_tab_widget_get(n, page);

        DRETURN_PTR((o ? ewl_label_text_get(EWL_LABEL(o)) : NULL), DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to set the tab into
 * @param page: The page to associate the tab with
 * @param tab: The contents of the tab
 * @return Returns no value.
 * @brief Set the widget to use as the tab for the page @p page to widget @p tab
 */
void
ewl_notebook_page_tab_widget_set(Ewl_Notebook *n, Ewl_Widget *page,
                                                        Ewl_Widget *tab)
{
        Ewl_Widget *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(n);
        DCHECK_PARAM_PTR(page);
        DCHECK_TYPE(n, EWL_NOTEBOOK_TYPE);
        DCHECK_TYPE(page, EWL_WIDGET_TYPE);

        if (!tab)
        {
                tab = ewl_label_new();
                ewl_widget_show(tab);
        }

        t = ewl_attach_widget_association_get(page);
        if (!t)
        {
                int idx = 0;

                t = ewl_hbox_new();
                ewl_widget_appearance_set(t, "tab");
                ewl_attach_widget_association_set(page, t);
                ewl_attach_widget_association_set(t, page);
                ewl_widget_show(t);

                ewl_callback_append(t, EWL_CALLBACK_CLICKED,
                                        ewl_notebook_cb_tab_clicked, n);

                idx = ewl_container_child_index_get(EWL_CONTAINER(n), page);
		ewl_container_child_insert(EWL_CONTAINER(n->body.tabbar), t,
                                idx);
        }
        else
                ewl_container_reset(EWL_CONTAINER(t));

        /* if this is the current page set it's tab to selected */
        if (n->cur_page == page)
                ewl_widget_state_set(t, "selected", EWL_STATE_PERSISTENT);

        ewl_container_child_append(EWL_CONTAINER(t), tab);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to get the tab widget from
 * @param page: The page to get the tab from
 * @return Returns the tab widget associated with the given page
 * @brief Retrieve the widget used as the tab for the page @p page
 */
Ewl_Widget *
ewl_notebook_page_tab_widget_get(Ewl_Notebook *n, Ewl_Widget *page)
{
        Ewl_Widget *t, *o;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(n, NULL);
        DCHECK_PARAM_PTR_RET(page, NULL);
        DCHECK_TYPE_RET(n, EWL_NOTEBOOK_TYPE, NULL);
        DCHECK_TYPE_RET(page, EWL_WIDGET_TYPE, NULL);

        t = ewl_attach_widget_association_get(page);
        if (!t)
        {
                DWARNING("We have a notebook page with no tab, bad, very bad.");
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        o = ewl_container_child_get(EWL_CONTAINER(t), 0);

        DRETURN_PTR(o, DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to make homogeneous
 * @param h: Boolean value to set the notebook's homogeneous value
 * @return Returns no value
 * @brief Sets the tabs in the notebook to be the same size
 **/
void
ewl_notebook_tabbar_homogeneous_set(Ewl_Notebook *n, unsigned int h)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(n);
        DCHECK_TYPE(n, EWL_NOTEBOOK_TYPE);

        ewl_box_homogeneous_set(EWL_BOX(n->body.tabbar), !!h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param n: The Ewl_Notebook to check for homogeneous
 * @return Returns the homgeneous value of the notebook @a n
 * @brief Retrieves the homogeneous value of the notebook
 **/
unsigned int
ewl_notebook_tabbar_homogeneous_get(Ewl_Notebook *n)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(n, 0);
        DCHECK_TYPE_RET(n, EWL_NOTEBOOK_TYPE, 0);

        DRETURN_INT(ewl_box_homogeneous_get(EWL_BOX(n->body.tabbar)),
                               DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child show callback
 */
void
ewl_notebook_cb_child_show(Ewl_Container *c, Ewl_Widget *w)
{
        Ewl_Notebook *n;
        int pw, ph;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        n = EWL_NOTEBOOK(EWL_WIDGET(c)->parent);

        if (n->cur_page != w)
                ewl_widget_hide(w);

        ewl_object_preferred_size_get(EWL_OBJECT(n->cur_page), &pw, &ph);
        ewl_object_preferred_inner_size_set(EWL_OBJECT(n->body.pages), pw, ph);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child hide callback
 */
void
ewl_notebook_cb_child_hide(Ewl_Container *c, Ewl_Widget *w)
{
        Ewl_Notebook *n;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        n = EWL_NOTEBOOK(EWL_WIDGET(c)->parent);

        if (n->cur_page == w)
                ewl_widget_show(w);

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
ewl_notebook_cb_child_add(Ewl_Container *c, Ewl_Widget *w)
{
        Ewl_Notebook *n;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        n = EWL_NOTEBOOK(EWL_WIDGET(c)->parent);

        /* stick a null tab in there so that it at least shows up */
        ewl_notebook_page_tab_widget_set(n, w, NULL);

        /* we have no current page, make it this one */
        if (!n->cur_page)
        {
                ewl_notebook_visible_page_set(n, w);
                ewl_widget_show(w);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @param rem_idx: UNUSED
 * @return Returns no value
 * @brief The child remove callback
 */
void
ewl_notebook_cb_child_remove(Ewl_Container *c, Ewl_Widget *w, int idx)
{
        Ewl_Widget *t;
        Ewl_Notebook *n;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        n = EWL_NOTEBOOK(EWL_WIDGET(c)->parent);

        /* we still have a tab, delete it */
        t = ewl_attach_widget_association_get(w);
        if (t)
                ewl_widget_destroy(t);


        /* change visible pages if needed */
        if (w == n->cur_page)
        {
                Ewl_Widget *page, *new_tab;
                int count;

                count = ewl_container_child_count_get(
                                                EWL_CONTAINER(n->body.tabbar));

                if (count <= 0)
                        n->cur_page = NULL;
                else
                {
                        /* make sure we aren't off the end of the list */
                        if (idx >= count) idx = count - 1;

                        new_tab = ewl_container_child_get(
                                        EWL_CONTAINER(n->body.tabbar), idx);

                        if (new_tab)
                        {
                                page = ewl_attach_widget_association_get(
                                                                new_tab);
                                if (page)
                                        ewl_notebook_visible_page_set(
                                                        EWL_NOTEBOOK(n), page);
                        }
                }

        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: The Ewl_Notebook widget
 * @return Returns no value
 * @brief The tab click callback
 */
void
ewl_notebook_cb_tab_clicked(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Widget *page;
        Ewl_Notebook *n;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(data, EWL_NOTEBOOK_TYPE);

        n = data;
        page = ewl_attach_widget_association_get(w);
        ewl_notebook_visible_page_set(n, page);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

