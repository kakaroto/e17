/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_list2.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_scrollport.h"

static Ewl_Widget *ewl_list2_widget_at(Ewl_MVC *mvc, void *data,
                                        unsigned int row,
                                        unsigned int column);

static Ecore_List *ewl_list2_cells_info_generate(Ewl_List2 *list,
                                        unsigned int count);
static void ewl_list2_cells_info_generate_range(Ewl_List2 *list,
                                        unsigned int from, unsigned int to,
                                        int *pref_w, int *pref_h);
static int ewl_list2_size_idler(void *data);

#define GROW_STEP 100

/**
 * @return Returns a new Ewl_Widget on success or NULL on failure
 * @brief Creates and initializes a new Ewl_List widget
 */
Ewl_Widget *
ewl_list2_new(void)
{
        Ewl_Widget *list = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);

        list = NEW(Ewl_List2, 1);
        if (!list)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_list2_init(EWL_LIST2(list)))
        {
                ewl_widget_destroy(list);
                list = NULL;
        }

        DRETURN_PTR(list, DLEVEL_STABLE);
}

/**
 * @param list: The list to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialises an Ewl_List widget to default values
 */
int
ewl_list2_init(Ewl_List2 *list)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(list, FALSE);

        if (!ewl_mvc_init(EWL_MVC(list)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(list), EWL_LIST2_TYPE);
        ewl_widget_inherit(EWL_WIDGET(list), EWL_LIST2_TYPE);

        list->port = ewl_scrollport_new();
        ewl_container_child_append(EWL_CONTAINER(list), list->port);
        ewl_widget_show(list->port);
        ewl_mvc_selected_change_cb_set(EWL_MVC(list),
                                                ewl_list2_cb_selected_change);
        ewl_callback_append(list->port, EWL_CALLBACK_CONFIGURE,
                                        ewl_list2_cb_configure, list);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param list: The list to use
 * @parem sizing: the size acquisition policy
 * @brief policy to querry the size information of the list items
 *
 * Since querrying the size information for huge count of list items is a
 * expensive task, there are several ways to not make the process blocking
 * the application.
 *
 * With @c EWL_LIST2_SIZING_FIXED the size of the first item
 * is used, for all other items. This is pretty fast but may result incorrect
 * sized list items. Use this option when you now that all items have the same
 * size.
 *
 * With @c EWL_LIST2_SIZING_GROW the size are only querried slice by slice 
 * (slice >> item) in the background. You can only scroll to items that
 * already have its size available.
 *
 * @note Changing this value will only be effective if the size querring
 * hasn't been started or if you force it to restart with marking the list
 * dirty.
 */
void
ewl_list2_size_acquisition_set(Ewl_List2 *list, Ewl_Size_Acquisition sizing)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(list);
        DCHECK_TYPE(list, EWL_LIST2_TYPE);

        if (sizing == list->sizing)
                DRETURN(DLEVEL_STABLE);

        list->sizing = sizing;

        DRETURN(DLEVEL_STABLE);
}

/**
 * @oaram list: The list to use
 * @return Returns the size acquisition policy
 * @brief Retrieve the size acquisition policy of the given list
 *
 * @see ewl_list2_size_acquisition_set()
 */
Ewl_Size_Acquisition
ewl_list2_size_acquisition_get(Ewl_List2 *list)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(list, FALSE);
        DCHECK_TYPE_RET(list, EWL_LIST2_TYPE, FALSE);

        DRETURN_INT(list->sizing, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The list to be configured
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief Configures the given list
 */
void
ewl_list2_cb_configure(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data)
{
        Ewl_List2 *list;
        const Ewl_Model *model;
        const Ewl_View *view;
        void *mvc_data;
        void *pr_data;
        unsigned int i;
        int px, py, pw, ph;
        int vx, vy, vw, vh;
        Ecore_List *cache;
        int pos;
        int new_start, new_end;
        int new_start_idx, new_end_idx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_LIST2_TYPE);

        list = EWL_LIST2(data);

        model = ewl_mvc_model_get(EWL_MVC(list));
        view = ewl_mvc_view_get(EWL_MVC(list));
        mvc_data = ewl_mvc_data_get(EWL_MVC(list));
        pr_data = ewl_mvc_private_data_get(EWL_MVC(list));

        /* if either the list isn't dirty or some of the MVC controls have
         * not been set on the list just leave this up to the box to handle */
        if ((!model) || (!view) || (!mvc_data))
                DRETURN(DLEVEL_STABLE);

        if (ewl_mvc_dirty_get(EWL_MVC(list)))
                cache = ewl_list2_cells_info_generate(list,
                                                model->count(mvc_data));
        else
        {
                cache = ecore_list_new();
                ecore_list_free_cb_set(cache,
                                ECORE_FREE_CB(ewl_widget_destroy));
        }
        
        /* get the area geometries */
        ewl_scrollport_visible_area_geometry_get(EWL_SCROLLPORT(list->port),
                                                        &vx, &vy, &vw, &vh);
        ewl_scrollport_area_geometry_get(EWL_SCROLLPORT(list->port),
                                                        &px, &py, &pw, &ph);

        new_start = vy - py;
        new_end = new_start + vh;
        /*
         * cache the which aren't visible widgets
         */
        for (pos = 0, i = 0; i < list->info_count
                        && pos + list->cinfo[i].size < new_start; i++)
        {
                if (list->cinfo[i].cell)
                {
                        ecore_list_append(cache, list->cinfo[i].cell);
                        list->cinfo[i].cell = NULL;
                }
                pos += list->cinfo[i].size;
        }
        new_start_idx = i;
        new_start = pos;

        /* skip the visible widgets */
        while (i < list->info_count && (pos += list->cinfo[i++].size) < new_end)
                ;
        new_end_idx = i;
        /* now cache the rest of the widgets */
        while (i < list->info_count)
        {
                if (list->cinfo[i].cell)
                        ecore_list_append(cache, list->cinfo[i].cell);
                list->cinfo[i].cell = NULL;
                i++;
        }
            
        /*
         * Create the missing widgets
         */
        for (i = new_start_idx; i < new_end_idx; i++)
        {
                Ewl_Widget *cell;
                Ewl_Widget *o;

                if (list->cinfo[i].cell)
                        continue;

                /* first try to get the list from the cache */
                cell = ecore_list_first_remove(cache);
                if (cell)
                {

                        o = ecore_list_first(EWL_CONTAINER(cell)->children);
                        /* assign the new value */
                        view->assign(o, model->fetch(mvc_data, i, 0), i, 0,
                                        pr_data);
                }
                else
                {
                        cell = ewl_cell_new();
                        ewl_container_child_append(EWL_CONTAINER(list->port),
                                        cell);
                        ewl_callback_append(cell, EWL_CALLBACK_CLICKED,
                                        ewl_list2_cb_item_clicked, list);
                        ewl_widget_show(cell);

                        o = view->constructor(0, pr_data);
                        view->assign(o, model->fetch(mvc_data, i, 0), i, 0,
                                        pr_data);
                        ewl_container_child_append(EWL_CONTAINER(cell), o);
                        ewl_widget_show(o);
                }

                list->cinfo[i].cell = cell;
        }

        /*
         * place the cells now
         */
        new_start += py;
        pw = MAX(pw, vw);
        for (i = new_start_idx; i < new_end_idx; i++)
        {
                ewl_object_place(EWL_OBJECT(list->cinfo[i].cell),
                                px, new_start, pw, list->cinfo[i].size);
                new_start += list->cinfo[i].size;
        }

        ecore_list_destroy(cache);

        /* 
         * And finally mark the mvc as clean
         */
        if (ewl_mvc_dirty_get(EWL_MVC(list)))
                ewl_mvc_dirty_set(EWL_MVC(list), FALSE);

        /* redo the selection */
        /* This does not work, because it only selects rows that aren't already
         * marked
         * ewl_list2_cb_selected_change(EWL_MVC(list));*/

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_list2_cell_realize(Ewl_Container *cell)
{
        Ewl_Widget *child;

        ewl_widget_realize_force(EWL_WIDGET(cell));
        ecore_list_first_goto(cell->children);
        while ((child = ecore_dlist_next(cell->children)))
        {
                if (EWL_CONTAINER_IS(child))
                        ewl_list2_cell_realize(EWL_CONTAINER(child));
                else
                        ewl_widget_realize_force(child);
        }
}

/**
 * @internal
 * @param list: the list to use
 * @param count: the new item count of the list
 * @return Returns a list of the previous generated widgets
 */
static Ecore_List *
ewl_list2_cells_info_generate(Ewl_List2 *list, unsigned int count)
{
        Ecore_List *cache;
        unsigned int i;
        int pref_h = 0;
        int pref_w = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(list, NULL);
        DCHECK_TYPE_RET(list, EWL_LIST2_TYPE, NULL);

        /* cache the previous cells */
        cache = ecore_list_new();
        ecore_list_free_cb_set(cache, ECORE_FREE_CB(ewl_widget_destroy));
        if (list->cinfo)
        {
                for (i = 0; i < list->info_count; i++)
                {
                        if (list->cinfo[i].cell)
                                ecore_list_prepend(cache, list->cinfo[i].cell);
                }
                FREE(list->cinfo);
        }

        list->cinfo = NEW(Ewl_List2_Cell_Info, count);
        list->info_size = count;

        /* we are done, if there are no sizes to fetch */
        if (!count)
                DRETURN_PTR(cache, DLEVEL_STABLE);

        if (list->sizing == EWL_SIZE_ACQUISITION_FIXED)
        {
                ewl_list2_cells_info_generate_range(list, 0, 1,
                                &pref_w, &pref_h);

                for (i = 0; i < count; i++)
                        list->cinfo[i].size = pref_h;
                pref_h = pref_h * count;
        }
        else if (list->sizing == EWL_SIZE_ACQUISITION_GROW)
        {
                count = MIN(GROW_STEP, count);
                ewl_list2_cells_info_generate_range(list, 0, count,
                                &pref_w, &pref_h);
                /* generate the rest infos later, so that we don't block
                 * the app */
                if (count != list->info_size)
                        ecore_idler_add(ewl_list2_size_idler, list);
        }
        list->info_count = count;

        /* update the preferred size */
        ewl_scrollport_area_size_set(EWL_SCROLLPORT(list->port), pref_w,
                        pref_h);
        
        DRETURN_PTR(cache, DLEVEL_STABLE);
}

/**
 * @internal
 * @param list: the list to use
 * @param from: the item to start querrying the size infos
 * @param to: the first item not to querry in the range
 * @param pref_w: a pointer to set the new width
 * @param pref_h: a pointer to set the new height
 * @return Returns no value
 */
static void
ewl_list2_cells_info_generate_range(Ewl_List2 *list, unsigned int from,
                unsigned int to, int *pref_w, int *pref_h)
{
        Ewl_Widget *cell, *o;
        const Ewl_Model *model;
        const Ewl_View *view;
        void *mvc_data;
        void *pr_data;
        unsigned int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(list);
        DCHECK_TYPE(list, EWL_LIST2_TYPE);

        model = ewl_mvc_model_get(EWL_MVC(list));
        view = ewl_mvc_view_get(EWL_MVC(list));
        mvc_data = ewl_mvc_data_get(EWL_MVC(list));
        pr_data = ewl_mvc_private_data_get(EWL_MVC(list));
        
        /* 
         * create the widget to retrieve the size from
         */
        cell = ewl_cell_new();
        ewl_container_child_append(EWL_CONTAINER(list->port), cell);
        ewl_widget_show(cell);

        o = view->constructor(0, pr_data);
        ewl_container_child_append(EWL_CONTAINER(cell), o);
        ewl_widget_show(o);

        for (i = from; i < to; i++)
        {
                view->assign(o, model->fetch(mvc_data, i, 0), i, 0,
                                pr_data);
                ewl_list2_cell_realize(EWL_CONTAINER(cell));
                list->cinfo[i].size = PREFERRED_H(cell);
                *pref_h += list->cinfo[i].size;
                *pref_w = MAX(*pref_w, PREFERRED_W(cell));
                ewl_widget_unrealize(cell);
        }
        ewl_widget_destroy(cell);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param data: the list to use
 * */
static int
ewl_list2_size_idler(void *data)
{
        Ewl_List2 *list;
        unsigned int start;
        int pref_w, pref_h;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, ECORE_CALLBACK_CANCEL);
        DCHECK_TYPE_RET(data, EWL_LIST2_TYPE, ECORE_CALLBACK_CANCEL);

        list = data;
        
        ewl_scrollport_area_geometry_get(EWL_SCROLLPORT(list->port), NULL, NULL,
                        &pref_w, &pref_h);

        start = list->info_count;
        list->info_count = MIN(start + GROW_STEP, list->info_size);
        ewl_list2_cells_info_generate_range(list, start, list->info_count,
                                                &pref_w, &pref_h);

        /* update the preferred size */
        ewl_scrollport_area_size_set(EWL_SCROLLPORT(list->port), pref_w,
                        pref_h);

        if (list->info_count == list->info_size)
                DRETURN_INT(ECORE_CALLBACK_CANCEL, DLEVEL_STABLE);

        DRETURN_INT(ECORE_CALLBACK_RENEW, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget that was clicked
 * @param ev: The event data
 * @param data: The list widget
 * @return Returns no value
 * @brief Sets the clicked widget as selected
 */
void
ewl_list2_cb_item_clicked(Ewl_Widget *w, void *ev, void *data)
{
        Ewl_List2 *list;
        Ewl_Event_Mouse_Down *event;
        const Ewl_Model *model;
        void *mvc_data;
        int row;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(data, EWL_LIST2_TYPE);

        event = ev;
        list = data;

        if (ewl_mvc_selection_mode_get(EWL_MVC(list)) ==
                                        EWL_SELECTION_MODE_NONE)
                DRETURN(DLEVEL_STABLE);

        model = ewl_mvc_model_get(EWL_MVC(list));
        mvc_data = ewl_mvc_data_get(EWL_MVC(list));

        /* find the row */
        row = 0;
        for (row = 0; row < list->info_count && w != list->cinfo[row].cell;
                        row++)
                ;

        /* set up the event structure */
        {
                Ewl_Event_MVC_Clicked mvc_event = {
                        event->base.modifiers, event->button, event->clicks,
                        model, mvc_data, row, 0
                };

                ewl_callback_call_with_event_data(EWL_WIDGET(data), 
                                        EWL_CALLBACK_MVC_CLICKED, &mvc_event);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param mvc: The MVC to work with
 * @return Returns no value
 * @brief Called when the selected widgets changes
 */
void
ewl_list2_cb_selected_change(Ewl_MVC *mvc)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_LIST2_TYPE);

        ewl_mvc_highlight(mvc, EWL_CONTAINER(mvc), ewl_list2_widget_at);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_list2_widget_at(Ewl_MVC *mvc, void *data __UNUSED__, unsigned int row,
                                        unsigned int column __UNUSED__)
{
        Ewl_List2 *l;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mvc, NULL);
        DCHECK_TYPE_RET(mvc, EWL_MVC_TYPE, NULL);

        l = EWL_LIST2(mvc);
        if (l->cinfo && row < l->info_count)
                DRETURN_PTR(l->cinfo[row].cell, DLEVEL_STABLE);

        DRETURN_PTR(NULL, DLEVEL_STABLE);
}

