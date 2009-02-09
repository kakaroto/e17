/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include <Ewl.h>
#include "eli_hiscore.h"
#include "Eli_App.h"
#include "eli_statistics.h"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "Elitaire.h"
#include "ewl_dialogs.h"

static Ewl_Widget * wh_entry = NULL;

struct _Win_Hi_Points {
    Eli_App * eap;
    float points;
    pointsType type;
};

static Ewl_Widget * _win_highscore_page_new(const char * game);
static void _destroy_cb(Ewl_Widget * w, void * event, void * data);
static void _name_destroy_cb(Ewl_Widget * w, void * event, void * data);
static void _highscore_key_down_cb(Ewl_Widget * w, void * event, void * data);
void _win_highscore_clicked_cb(Ewl_Widget * w, void * event, 
                                                          void * data);
void _highscore_add(Ewl_Window * win, _Win_Hi_Points * wh);
static void * highscore_data_fetch(void * data, unsigned int row, 
		                                  unsigned int col);
static unsigned int highscore_data_count(void * data);
static Ewl_Widget * highscore_widget_constructor(unsigned int col, void * prd);
static void highscore_widget_assign(Ewl_Widget *w, void * data,
                                    unsigned int row, unsigned int col,
                                    void *prd);
static Ewl_Widget * highscore_header_fetch(void * data, unsigned int column,
                                            void * prd);
/*
 * External API
 */

void ewl_frontend_dialog_highscore_open(Eli_App * eap, const char * game)
{
    Ewl_Widget * win;
    Ewl_Widget * o;
    Ewl_Widget * notebook;
    const char ** games;
    int i;
    Ewl_Widget * selected_page = NULL;

    /* Setup and show the hiscore window */
    win = ewl_dialog_new();
    ewl_dialog_action_position_set(EWL_DIALOG(win), EWL_POSITION_BOTTOM);
    ewl_window_title_set(EWL_WINDOW(win), _("Highscore"));
    ewl_window_name_set(EWL_WINDOW(win), "Elitaire");
    ewl_window_class_set(EWL_WINDOW(win), "Elitaire");
    ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, _destroy_cb,
                        NULL);
    ewl_dialog_has_separator_set(EWL_DIALOG(win), 1);
    ewl_window_leader_foreign_set(EWL_WINDOW(win), 
		    		  EWL_EMBED_WINDOW(eap->main_win));
    ewl_object_fill_policy_set(EWL_OBJECT(win), EWL_FLAG_FILL_FILL);
    ewl_widget_show(win);

    /* Setup and show the stock icons */
    ewl_dialog_active_area_set(EWL_DIALOG(win), EWL_POSITION_BOTTOM);
    o = ewl_button_new();
    ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_OK);
    ewl_container_child_append(EWL_CONTAINER(win), o);
    ewl_callback_append(o, EWL_CALLBACK_CLICKED, _destroy_cb, NULL);
    ewl_widget_show(o);

    /* Setup and show th notebook */
    ewl_dialog_active_area_set(EWL_DIALOG(win), EWL_POSITION_TOP);
    notebook = ewl_notebook_new();
    ewl_notebook_tabbar_position_set(EWL_NOTEBOOK(notebook),
                                     EWL_POSITION_LEFT);
    ewl_container_child_append(EWL_CONTAINER(win), notebook);
    ewl_object_fill_policy_set(EWL_OBJECT(notebook), EWL_FLAG_FILL_FILL);
    ewl_widget_show(notebook);

    games = elitaire_available_games_get();

    for (i = 0; games[i][0]; i++) {
        Ewl_Widget * page;

        page = _win_highscore_page_new(games[i]);
        ewl_widget_show(page);
        ewl_container_child_append(EWL_CONTAINER(notebook), page);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(notebook), page,
                                       games[i]);

        if (!strcmp(games[i], game)) selected_page = page;
    }

    if (selected_page)
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(notebook), selected_page);
}

void ewl_frontend_dialog_highscore_add(Eli_App * eap, float points, pointsType type)
{
    Ewl_Widget * win;
    Ewl_Widget * o;
    Ewl_Widget * vbox;
    char * username;
    _Win_Hi_Points * wh;

    /* when thers is already a name window open dont open a new one */
    if (wh_entry) return;

    wh = (_Win_Hi_Points *) calloc(1, sizeof(_Win_Hi_Points));
    wh->eap = eap;
    wh->points = points;
    wh->type = type;

    /* a hopefully good default */
    username = getpwuid(geteuid())->pw_name;

    /* Setup and show the name window */
    win = ewl_icondialog_new();
    ewl_icondialog_icon_set(EWL_ICONDIALOG(win), EWL_ICON_DIALOG_QUESTION);
    ewl_dialog_action_position_set(EWL_DIALOG(win), EWL_POSITION_BOTTOM);
    ewl_window_title_set(EWL_WINDOW(win), _("Name"));
    ewl_window_name_set(EWL_WINDOW(win), "Elitaire Name");
    ewl_window_class_set(EWL_WINDOW(win), "Elitaire");
    ewl_window_transient_for_foreign(EWL_WINDOW(win), 
		    			EWL_EMBED_WINDOW(eap->main_win));
    ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, _name_destroy_cb, wh);
    ewl_dialog_has_separator_set(EWL_DIALOG(win), TRUE);
    ewl_widget_show(win);

    /* Setup and show the stock icons */
    ewl_dialog_active_area_set(EWL_DIALOG(win), EWL_POSITION_BOTTOM);
    o = ewl_button_new();
    ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_OK);
    ewl_container_child_append(EWL_CONTAINER(win), o);
    ewl_callback_append(o, EWL_CALLBACK_CLICKED, _win_highscore_clicked_cb,
                        wh);
    ewl_widget_show(o);

    /* vbox for text and entry */
    ewl_dialog_active_area_set(EWL_DIALOG(win), EWL_POSITION_TOP);
    vbox = ewl_vbox_new();
    ewl_container_child_append(EWL_CONTAINER(win), vbox);
    ewl_widget_show(vbox);

    /* Text */
    o = ewl_text_new();
    ewl_text_text_set(EWL_TEXT(o), _("Please insert your name"));
    ewl_container_child_append(EWL_CONTAINER(vbox), o);
    ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
    ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
    ewl_widget_show(o);

    /* Entry */
    o = ewl_entry_new();
    ewl_text_text_set(EWL_TEXT(o), username);
    ewl_container_child_append(EWL_CONTAINER(vbox), o);
    ewl_widget_focus_send(o);
    ewl_callback_append(EWL_WIDGET(o), EWL_CALLBACK_KEY_DOWN, 
                        _highscore_key_down_cb, wh);
    ewl_text_all_select(EWL_TEXT(o));
    ewl_widget_show(o);

    wh_entry = o;
}

/*
 * Internals
 */
static void _destroy_cb(Ewl_Widget * w, void * event, void * data)
{
    ewl_widget_destroy(EWL_WIDGET(ewl_embed_widget_find(w)));
    if (data) free(data);
}

static void _name_destroy_cb(Ewl_Widget * w, void * event, void * data)
{
    _Win_Hi_Points * wh;

    wh = (_Win_Hi_Points *) data;
    ewl_widget_destroy(EWL_WIDGET(ewl_embed_widget_find(w)));
    wh_entry = NULL;
    
    eli_app_state_leave(wh->eap);
    
    free(wh);
}

static Ewl_Widget * _win_highscore_page_new(const char * game)
{
    Ewl_Widget * page;
    Ewl_Widget * tree;
    Ewl_Widget * grid;
    Ewl_Widget * w;
    Ewl_Model * model;
    Ewl_View * view;
    
    Eli_Statistics stats;
    char buffer[5];
    float qoute;

    /* the page */
    page = ewl_border_new();
    ewl_border_label_set(EWL_BORDER(page), game);
    ewl_border_label_alignment_set(EWL_BORDER(page), EWL_FLAG_ALIGN_CENTER);
    
    /* 
     * the statistics 
     */
    stats = eli_statistics_get(game);

    grid = ewl_grid_new();
    ewl_container_child_append(EWL_CONTAINER(page), grid);
    ewl_object_fill_policy_set(EWL_OBJECT(grid), EWL_FLAG_FILL_NONE
		                                 | EWL_FLAG_FILL_HFILL);
    ewl_grid_dimensions_set(EWL_GRID(grid), 2, 4);
    ewl_widget_show(grid);
    
    /* won games */
    w = ewl_label_new();
    ewl_label_text_set(EWL_LABEL(w), _("Won Games:"));
    ewl_container_child_append(EWL_CONTAINER(grid), w);
    ewl_widget_show(w);
   
    snprintf(buffer, sizeof(buffer), "%d", stats.won_games);
    w = ewl_label_new();
    ewl_label_text_set(EWL_LABEL(w), buffer);
    ewl_object_alignment_set(EWL_OBJECT(w), EWL_FLAG_ALIGN_RIGHT);
    ewl_container_child_append(EWL_CONTAINER(grid), w);
    ewl_widget_show(w);

    /* lost games */
    w = ewl_label_new();
    ewl_label_text_set(EWL_LABEL(w), _("Lost Games:"));
    ewl_container_child_append(EWL_CONTAINER(grid), w);
    ewl_widget_show(w);

    snprintf(buffer, sizeof(buffer), "%d", stats.lost_games);
    w = ewl_label_new();
    ewl_label_text_set(EWL_LABEL(w), buffer);
    ewl_object_alignment_set(EWL_OBJECT(w), EWL_FLAG_ALIGN_RIGHT);
    ewl_container_child_append(EWL_CONTAINER(grid), w);
    ewl_widget_show(w);

    /* relative quoute */
    w = ewl_label_new();
    ewl_label_text_set(EWL_LABEL(w), _("relative Quote:"));
    ewl_container_child_append(EWL_CONTAINER(grid), w);
    ewl_widget_show(w);
  
    if (stats.won_games == 0 && stats.lost_games == 0)
        qoute = 0.0;
    else
        qoute =
            stats.won_games / (float) (stats.won_games + stats.lost_games);

    snprintf(buffer, sizeof(buffer), "%.2f", qoute);
    w = ewl_label_new();
    ewl_label_text_set(EWL_LABEL(w), buffer);
    ewl_object_alignment_set(EWL_OBJECT(w), EWL_FLAG_ALIGN_RIGHT);
    ewl_container_child_append(EWL_CONTAINER(grid), w);
    ewl_widget_show(w);

    /*
     * the highscore
     */
    tree = ewl_tree_new();
    ewl_container_child_append(EWL_CONTAINER(page), tree);
    ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_FILL);
    ewl_mvc_selection_mode_set(EWL_MVC(tree), EWL_SELECTION_MODE_NONE);
    ewl_mvc_data_set(EWL_MVC(tree), eli_highscore_get(game));
    ewl_tree_content_view_set(EWL_TREE(tree), ewl_tree_view_plain_get());
    ewl_tree_column_count_set(EWL_TREE(tree), 3);
    ewl_widget_show(tree);

    /* the model for first column */
    model = ewl_model_new();
    ewl_model_data_fetch_set(model, highscore_data_fetch);
    ewl_model_data_count_set(model, highscore_data_count);

    /* the view for the first column */
    view = ewl_view_new();
    ewl_view_widget_constructor_set(view, highscore_widget_constructor);
    ewl_view_widget_assign_set(view, highscore_widget_assign);
    ewl_view_header_fetch_set(view, highscore_header_fetch);

    /* set the model and the view to the tree */
    ewl_mvc_model_set(EWL_MVC(tree), model);
    ewl_mvc_view_set(EWL_MVC(tree), view);

    return page;
}

static void _highscore_key_down_cb(Ewl_Widget * w, void * event, void * data)
{
    Ewl_Event_Key * ev;

    ev = EWL_EVENT_KEY(event);
    if ((!strcmp(ev->keyname, "Return")) 
            || (!strcmp(ev->keyname, "KP_Return"))
            || (!strcmp(ev->keyname, "Enter"))
            || (!strcmp(ev->keyname, "KP_Enter"))
            || (!strcmp(ev->keyname, "\n")))
    {
        Ewl_Embed *win;

        win = ewl_embed_widget_find(w);
        _highscore_add(EWL_WINDOW(win), (_Win_Hi_Points *) data);
    }
}

void _win_highscore_clicked_cb(Ewl_Widget * w, void * event, void * data)
{
    Ewl_Embed *win;

    win = ewl_embed_widget_find(w);
    _highscore_add(EWL_WINDOW(win), (_Win_Hi_Points *) data);
}

void _highscore_add(Ewl_Window * win, _Win_Hi_Points * wh)
{
    const char * text;
    
    text = ewl_text_text_get(EWL_TEXT(wh_entry));
    wh_entry = NULL;
    eli_highscore_entry_add(wh->eap->current.game, text, wh->points,
                            wh->type);
    eli_app_highscore_open(wh->eap, wh->eap->current.game);
    ewl_widget_destroy(EWL_WIDGET(win));
    eli_app_state_leave(wh->eap);
    free(wh);
}

static void * highscore_data_fetch(void * data, unsigned int row, 
		                                  unsigned int col)
{
    Evas_List * l;

    l = (Evas_List *) data;
    return evas_list_nth(l, row);
}

static unsigned int highscore_data_count(void * data)
{
    Evas_List * l;

    l = (Evas_List *) data;
    return evas_list_count(l);
}

static Ewl_Widget * highscore_widget_constructor(unsigned int col, void *prd)
{
    return ewl_label_new();
}

static void highscore_widget_assign(Ewl_Widget *w, void * data,
                                    unsigned int row, unsigned int col,
                                    void *prd)
{
    Eli_Highscore_Entry *e;

    e = (Eli_Highscore_Entry *) data;

    if (col == 0) {
       char buffer[8];

       snprintf(buffer, sizeof(buffer), "%i.", row + 1);
       ewl_label_text_set(EWL_LABEL(w), buffer);
    }
    else if (col == 1) {
       ewl_object_alignment_set(EWL_OBJECT(w), EWL_FLAG_ALIGN_CENTER);
       ewl_label_text_set(EWL_LABEL(w), e->username);
    }
    else if (col == 2) {
       const char * pstring;

       pstring = pointsType_point_string_get(e->points, 
	      					POINTS_TYPE_INTEGER_GOOD);
       ewl_object_alignment_set(EWL_OBJECT(w), EWL_FLAG_ALIGN_RIGHT);
       ewl_label_text_set(EWL_LABEL(w), pstring);
    }
}

static Ewl_Widget * highscore_header_fetch(void *data , unsigned int column,
                                            void *pr)
{
    Ewl_Widget *l;

    l = ewl_label_new();
    if (column == 0)
        ewl_label_text_set(EWL_LABEL(l), "");
    else if (column == 1)
        ewl_label_text_set(EWL_LABEL(l), "User");
    else
        ewl_label_text_set(EWL_LABEL(l), "Points");
    
    ewl_widget_show(l);
    
    return l;
}

