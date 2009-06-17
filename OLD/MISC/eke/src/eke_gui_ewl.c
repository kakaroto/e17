#include <Ewl.h>

#include "config.h"
#include "eke_gui.h"
#include "eke_gui_ewl.h"

static void eke_gui_ewl_menu_setup(Eke *eke);
static void eke_gui_ewl_exit_cb(Ewl_Widget *w, void *ev, void *data);
static void eke_gui_ewl_manage_feed_cb(Ewl_Widget *w, void *ev, void *data);
static void eke_gui_ewl_manage_feed_exit_cb(Ewl_Widget *w, void *ev, void *data);
static void eke_gui_ewl_add_feed_cb(Ewl_Widget *w, void *ev, void *data);
static void eke_gui_ewl_add_feed_ok_cb(Ewl_Widget *w, void *ev, void *data);

typedef struct Eke_Gui_Ewl_Feed Eke_Gui_Ewl_Feed;
struct Eke_Gui_Ewl_Feed
{
    Ewl_Widget *tab;
    Ewl_Widget *page;
};

int
eke_gui_ewl_init(int *argc, char ** argv)
{
    if (!ewl_init(argc, argv)) return 0;
    return 1;
}

void
eke_gui_ewl_shutdown(void)
{
    ewl_shutdown();
}

void
eke_gui_ewl_create(Eke *eke)
{
    Ewl_Widget *box, *body;

    eke->gui.ewl.win = ewl_window_new();
    ewl_window_title_set(EWL_WINDOW(eke->gui.ewl.win), PACKAGE);
    ewl_window_class_set(EWL_WINDOW(eke->gui.ewl.win), PACKAGE);
    ewl_window_name_set(EWL_WINDOW(eke->gui.ewl.win), PACKAGE);
    ewl_object_size_request(EWL_OBJECT(eke->gui.ewl.win), EKE_GUI_WIDTH, EKE_GUI_HEIGHT);
    ewl_callback_append(eke->gui.ewl.win, EWL_CALLBACK_DELETE_WINDOW,
                                                    eke_gui_ewl_exit_cb, NULL);
    ewl_widget_show(eke->gui.ewl.win);

    box = ewl_vbox_new();
    ewl_container_child_append(EWL_CONTAINER(eke->gui.ewl.win), box);
    ewl_widget_show(box);

    eke->gui.ewl.menubar = ewl_hmenubar_new();
    ewl_container_child_append(EWL_CONTAINER(box), eke->gui.ewl.menubar);
    ewl_widget_show(eke->gui.ewl.menubar);

    eke_gui_ewl_menu_setup(eke);

    body = ewl_hbox_new();
    ewl_container_child_append(EWL_CONTAINER(box), body);
    ewl_widget_show(body);

    eke->gui.ewl.notebook = ewl_notebook_new();
    ewl_container_child_append(EWL_CONTAINER(body), eke->gui.ewl.notebook);
    ewl_notebook_tabs_position_set(EWL_NOTEBOOK(eke->gui.ewl.notebook),
                                                        EWL_POSITION_LEFT);
    ewl_notebook_tabs_alignment_set(EWL_NOTEBOOK(eke->gui.ewl.notebook),
                                                        EWL_FLAG_ALIGN_TOP);
    ewl_widget_show(eke->gui.ewl.notebook);

    eke->gui.ewl.statusbar = ewl_statusbar_new();
    ewl_container_child_append(EWL_CONTAINER(box), eke->gui.ewl.statusbar);
    ewl_widget_show(eke->gui.ewl.statusbar);
}

void
eke_gui_ewl_feed_register(Eke *eke, Eke_Feed *feed)
{
    Eke_Gui_Ewl_Feed *disp;

    disp = NEW(Eke_Gui_Ewl_Feed, 1);
    ecore_hash_set(eke->feeds, feed, disp);
}

void
eke_gui_ewl_feed_change(Eke *eke, Eke_Feed *feed)
{
    Eke_Gui_Ewl_Feed *disp;
    Eke_Feed_Item *item;
    Ewl_Widget *o;

    disp = ecore_hash_get(eke->feeds, feed);
    if (!disp->tab) {
        Ewl_Widget *o;

        disp->tab = ewl_text_new(feed->title);
        ewl_widget_show(disp->tab);

        o = ewl_vbox_new();
        ewl_notebook_page_append(EWL_NOTEBOOK(eke->gui.ewl.notebook),
                                                        disp->tab, o);
        ewl_widget_show(o);

        disp->page = ewl_scrollpane_new();
        ewl_container_child_append(EWL_CONTAINER(o), disp->page);
        ewl_object_insets_set(EWL_OBJECT(disp->page), 5, 5, 5, 5);
        ewl_widget_show(disp->page);
    }
    ewl_container_reset(EWL_CONTAINER(disp->page));

    ecore_list_first_goto(feed->items);
    while ((item = ecore_list_next(feed->items)) != NULL) {
        o = ewl_text_new(NULL);
        ewl_text_wrap_set(EWL_TEXT(o), 1);
        ewl_text_style_set(EWL_TEXT(o), "soft_shadow");
        ewl_text_text_set(EWL_TEXT(o), item->title);
        ewl_container_child_append(EWL_CONTAINER(disp->page), o);
        ewl_widget_show(o);

        if (item->link) {
            o = ewl_text_new(item->link);
            ewl_text_wrap_set(EWL_TEXT(o), 1);
            ewl_container_child_append(EWL_CONTAINER(disp->page), o);
            ewl_widget_show(o);
        }

        if (item->desc) {
            o = ewl_text_new(item->desc);
            ewl_text_wrap_set(EWL_TEXT(o), 1);
            ewl_container_child_append(EWL_CONTAINER(disp->page), o);
            ewl_widget_show(o);
        }

        o = ewl_text_new(" ");
        ewl_container_child_append(EWL_CONTAINER(disp->page), o);
        ewl_widget_show(o);
    }
}

void
eke_gui_ewl_feed_error(Eke *eke, Eke_Feed *feed)
{

    return;
    eke = NULL;
    feed = NULL;
}

static void
eke_gui_ewl_menu_setup(Eke *eke)
{
    Ewl_Widget *feed, *o;

    feed = ewl_menubar_menu_add(EWL_MENUBAR(eke->gui.ewl.menubar), 
                                                    NULL, "feeds");

    o = ewl_menu_item_new(NULL, "manage feeds");
    ewl_container_child_append(EWL_CONTAINER(feed), o);
    ewl_callback_append(o, EWL_CALLBACK_SELECT,
                                eke_gui_ewl_manage_feed_cb, eke);
    ewl_widget_show(o);
}

static void
eke_gui_ewl_exit_cb(Ewl_Widget *w, void *ev, void *data)
{
    ewl_widget_destroy(w);
    ewl_main_quit();

    return;
    ev = NULL;
    data = NULL;
}

static void
eke_gui_ewl_manage_feed_cb(Ewl_Widget *w, void *ev, void *data)
{
    Eke *eke;
    Ewl_Widget *win, *box, *button;

    eke = data;

    win = ewl_dialog_new(EWL_POSITION_BOTTOM);
    ewl_window_title_set(EWL_WINDOW(win), PACKAGE " -- manage feeds");
    ewl_window_name_set(EWL_WINDOW(win), PACKAGE);
    ewl_window_class_set(EWL_WINDOW(win), PACKAGE);
    ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
                            eke_gui_ewl_manage_feed_exit_cb, win);
    ewl_widget_show(win);

    box = ewl_vbox_new();
    ewl_dialog_widget_add(EWL_DIALOG(win), box);
    ewl_widget_show(box);

    button = ewl_button_new("add feed");
    ewl_widget_data_set(button, "manage_feed_win", win);
    ewl_container_child_append(EWL_CONTAINER(box), button);
    ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
    ewl_callback_append(button, EWL_CALLBACK_CLICKED,
                                eke_gui_ewl_add_feed_cb, eke);
    ewl_widget_show(button);

    button = ewl_button_stock_with_id_new(EWL_STOCK_CANCEL,
                                            EWL_RESPONSE_CANCEL);
    ewl_container_child_append(EWL_CONTAINER(win), button);
    ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
    ewl_callback_append(button, EWL_CALLBACK_VALUE_CHANGED,
                                eke_gui_ewl_manage_feed_exit_cb, win);
    ewl_widget_show(button);

    return;
    w = NULL;
    ev = NULL;
}

static void
eke_gui_ewl_manage_feed_exit_cb(Ewl_Widget *w, void *ev, void *data)
{
    Ewl_Widget *win;

    win = data;

    ewl_widget_destroy(win);

    return;
    ev = NULL;
    w = NULL;
}

static void
eke_gui_ewl_add_feed_cb(Ewl_Widget *w, void *ev, void *data)
{
    Eke *eke;
    Ewl_Widget *win, *box, *entry, *o, *name_entry;

    eke = data;

    win = ewl_widget_data_get(w, "manage_feed_win");
    ewl_widget_destroy(win);

    win = ewl_dialog_new(EWL_POSITION_BOTTOM);
    ewl_window_title_set(EWL_WINDOW(win), PACKAGE " -- add feed");
    ewl_window_name_set(EWL_WINDOW(win), PACKAGE);
    ewl_window_class_set(EWL_WINDOW(win), PACKAGE);
    ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
                            eke_gui_ewl_manage_feed_exit_cb, win);
    ewl_widget_show(win);

    box = ewl_vbox_new();
    ewl_dialog_widget_add(EWL_DIALOG(win), box);
    ewl_widget_show(box);

    o = ewl_text_new("Add Feed");
    ewl_container_child_append(EWL_CONTAINER(box), o);
    ewl_widget_show(o);

    o = ewl_text_new(" ");
    ewl_container_child_append(EWL_CONTAINER(box), o);
    ewl_widget_show(o);

    o = ewl_text_new("Feed Name");
    ewl_container_child_append(EWL_CONTAINER(box), o);
    ewl_widget_show(o);

    name_entry = ewl_entry_new("");
    ewl_container_child_append(EWL_CONTAINER(box), name_entry);
    ewl_widget_show(name_entry);

    o = ewl_text_new(" ");
    ewl_container_child_append(EWL_CONTAINER(box), o);
    ewl_widget_show(o);

    o = ewl_text_new("URI [http://<server>[:port]/path/to/file.rss");
    ewl_container_child_append(EWL_CONTAINER(box), o);
    ewl_widget_show(o);

    entry = ewl_entry_new("");
    ewl_container_child_append(EWL_CONTAINER(box), entry);
    ewl_widget_show(entry);
   
    o = ewl_button_stock_with_id_new(EWL_STOCK_OK, EWL_RESPONSE_OK);
    ewl_container_child_append(EWL_CONTAINER(win), o);
    ewl_widget_data_set(o, "entry", entry);
    ewl_widget_data_set(o, "name_entry", name_entry);
    ewl_widget_data_set(o, "win", win);
    ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                eke_gui_ewl_add_feed_ok_cb, eke);
    ewl_widget_show(o);

    o = ewl_button_stock_with_id_new(EWL_STOCK_CANCEL, EWL_RESPONSE_CANCEL);
    ewl_container_child_append(EWL_CONTAINER(win), o);
    ewl_widget_data_set(o, "win", win);
    ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                eke_gui_ewl_manage_feed_exit_cb, win);
    ewl_widget_show(o);

    return;
    ev = NULL;
}

static void
eke_gui_ewl_add_feed_ok_cb(Ewl_Widget *w, void *ev, void *data)
{
    Ewl_Widget *entry, *name_entry, *win;
    Eke *eke;
    Eke_Feed *feed;
    char *uri, *name;

    eke = data;
    win = ewl_widget_data_get(w, "win");
    entry = ewl_widget_data_get(w, "entry");
    name_entry = ewl_widget_data_get(w, "name_entry");

    uri = ewl_entry_text_get(EWL_ENTRY(entry));
    name = ewl_entry_text_get(EWL_ENTRY(name_entry));

    feed = eke_feed_new_from_uri(uri);

    if (name && strcmp(name, "")) 
        eke_feed_name_set(feed, name);

    eke_gui_feed_register(eke, feed);
    eke_feed_update(feed);

    FREE(uri);

    ewl_widget_destroy(win);

    return;
    ev = NULL;
}

void
eke_gui_ewl_begin(void)
{
    ewl_main();
}


