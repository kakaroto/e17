/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include <Ewl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "eli_help_reader.h"

#define HELP_READER_WINDOW(hr) ((Help_Reader_Window *)(hr))

typedef struct Help_Reader_Window Help_Reader_Window;

struct Help_Reader_Window
{
    Ewl_Window    win;
    Ewl_Widget  * text;
    char        * page;
    Ewl_Widget  * prev_button;
    Ewl_Widget  * next_button;
    Ecore_DList * history;
};

static Ewl_Widget * help_reader_win = NULL;
static char * help_reader_dir = NULL;
/* this is just an unique reference */
#define TRIGGER_TXT (&help_reader_win)

static void help_reader_trigger_cb_destroy(Ewl_Widget * w, void * ev, 
                                                                void * data);
static void help_reader_trigger_cb_clicked(Ewl_Widget * w, void * ev,
                                                                void * data);
static void help_reader_text_cb_append(void *data, const char *text, 
                                    Eli_Help_Reader_Node *styles, size_t len);
static void help_reader_text_cb_enter(void *data, Eli_Help_Reader_Node *styles,
                                         size_t len);
static void help_reader_text_cb_leave(void *data, Eli_Help_Reader_Node *styles,
                                         size_t len);
static Ewl_Widget * help_reader_window_new(void);
static void help_reader_window_cb_destroy(Ewl_Widget * w, void * e, void * d);
static void help_reader_window_cb_delete(Ewl_Widget * w, void * e, void * d);
static void help_reader_window_page_set(Help_Reader_Window * win, 
                                        const char * page, int history);
static void help_reader_window_history_add(Help_Reader_Window * win,
                                        const char * path);
static const char * help_reader_window_history_next(Help_Reader_Window * win);
static const char * help_reader_window_history_prev(Help_Reader_Window * win);
static int help_reader_window_history_has_next(Help_Reader_Window * win);
static int help_reader_window_history_has_prev(Help_Reader_Window * win);
static void help_reader_prev_button_cb_clicked(Ewl_Widget * w, void * ev,
                                                                void * data);
static void help_reader_next_button_cb_clicked(Ewl_Widget * w, void * ev,
                                                                void * data);
static void help_reader_home_button_cb_clicked(Ewl_Widget * w, void * ev,
                                                                void * data);
static void help_reader_window_buttons_update(Help_Reader_Window * win);
/*
 * external API
 */

void
help_reader_init(const char * basedir)
{
    if (help_reader_dir)
        free(help_reader_dir);
    help_reader_dir = strdup(basedir);
}

void
help_reader_shutdown(void)
{
    if (help_reader_dir)
        free(help_reader_dir);
    help_reader_dir = NULL;

    if (help_reader_win)
        ewl_widget_destroy(help_reader_win);
    help_reader_win = NULL;
}

void
help_reader_page_set(const char * page)
{
    if (!help_reader_win)
        help_reader_win = help_reader_window_new();

    help_reader_window_page_set(HELP_READER_WINDOW(help_reader_win), page, 1);
}

static void
help_reader_window_page_set(Help_Reader_Window *win, const char * page, int history)
{
    Ewl_Widget * t;
    char buffer[4096];
    char * lang;
    char * ptr;

    /* clean the text */
    t = win->text;
    ewl_text_clear(EWL_TEXT(t));
    ewl_container_reset(EWL_CONTAINER(t));
    ewl_text_wrap_set(EWL_TEXT(t), EWL_TEXT_WRAP_WORD);
    
    /* add the page to the history */
    if (history)
        help_reader_window_history_add(win, page);

    /*
     * Determine the locale we will use
     */
    lang = setlocale(LC_MESSAGES, NULL);
    /* remove the encoding, we only support utf-8 */
    ptr = strchr(lang, '.');
    if (ptr)
        *ptr = 0;
    ptr = strchr(lang, '-');
    if (ptr)
        *ptr = 0;

    /*
     * create the path
     */
    snprintf(buffer, sizeof(buffer), "%s/%s-%s.ehf", help_reader_dir, lang,
                                                    page);
    if (ecore_file_exists(buffer))
        goto PARSE;

    /* try only with the language */
    ptr = strchr(lang, '_');
    if (ptr)
        *ptr = 0;

    snprintf(buffer, sizeof(buffer), "%s/%s-%s.ehf", help_reader_dir, lang,
                                                    page);
    if (ecore_file_exists(buffer))
        goto PARSE;

    /* still doesn't work?, try it with the English page */
    snprintf(buffer, sizeof(buffer), "%s/en-%s.ehf", help_reader_dir, page);
    if (ecore_file_exists(buffer))
        goto PARSE;

    //help_reader_text_page_not_found(t);
    return;

PARSE:
    eli_help_reader_parse(buffer, help_reader_text_cb_enter, 
                                  help_reader_text_cb_append,
                                  help_reader_text_cb_leave, 
                                  win);
}


/*
 * Internal functions
 */
static Ewl_Widget *
help_reader_window_new(void)
{
    Ewl_Widget * win, * c, * w, * vbox;
    
    win = calloc(1, sizeof(Help_Reader_Window));
    ewl_window_init(EWL_WINDOW(win));
    ewl_object_size_request(EWL_OBJECT(win), 600, 500);
    ewl_window_name_set(EWL_WINDOW(win), "Elitaire Help");
    ewl_window_class_set(EWL_WINDOW(win), "Elitaire");
    ewl_callback_append(win, EWL_CALLBACK_DESTROY,
                        help_reader_window_cb_destroy, NULL);
    ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, 
                        help_reader_window_cb_delete, NULL);
    ewl_widget_show(win);

    /* vbox to hold the toolbar and the text view */
    vbox = ewl_vbox_new();
    ewl_container_child_append(EWL_CONTAINER(win), vbox);
    ewl_widget_show(vbox);

    /* the toolbar */
    c = ewl_htoolbar_new();
    ewl_toolbar_icon_part_hide(EWL_TOOLBAR(c), EWL_ICON_PART_LABEL);
    ewl_container_child_append(EWL_CONTAINER(vbox), c);
    ewl_widget_show(c);

    /* the toolbar buttons */
    w = ewl_icon_simple_new();
    ewl_stock_type_set(EWL_STOCK(w), EWL_STOCK_ARROW_LEFT);
    ewl_callback_append(w, EWL_CALLBACK_CLICKED,
                        help_reader_prev_button_cb_clicked, win);
    ewl_container_child_append(EWL_CONTAINER(c), w);
    ewl_widget_disable(w);
    ewl_widget_show(w);
    HELP_READER_WINDOW(win)->prev_button = w;

    w = ewl_icon_simple_new();
    ewl_stock_type_set(EWL_STOCK(w), EWL_STOCK_ARROW_RIGHT);
    ewl_callback_append(w, EWL_CALLBACK_CLICKED,
                        help_reader_next_button_cb_clicked, win);
    ewl_container_child_append(EWL_CONTAINER(c), w);
    ewl_widget_disable(w);
    ewl_widget_show(w);
    HELP_READER_WINDOW(win)->next_button = w;

    w = ewl_icon_simple_new();
    ewl_stock_type_set(EWL_STOCK(w), EWL_STOCK_HOME);
    ewl_callback_append(w, EWL_CALLBACK_CLICKED,
                        help_reader_home_button_cb_clicked, win);
    ewl_container_child_append(EWL_CONTAINER(c), w);
    ewl_widget_show(w);

    /* the text view */
    c = ewl_scrollpane_new();
    ewl_container_child_append(EWL_CONTAINER(vbox), c);
    ewl_widget_show(c);

    w = ewl_text_new();
    ewl_container_child_append(EWL_CONTAINER(c), w);
    ewl_widget_show(w);

    HELP_READER_WINDOW(win)->text = w;

    /* the histroy */
    HELP_READER_WINDOW(win)->history = ecore_dlist_new();
    ecore_dlist_free_cb_set(HELP_READER_WINDOW(win)->history, free);

    return win;
}

static void
help_reader_window_cb_destroy(Ewl_Widget * w, void * ev, void * data)
{
    help_reader_win = NULL;
    ecore_dlist_destroy(HELP_READER_WINDOW(w)->history);
    /* XXX remove that later */
    ewl_main_quit();
}

static void
help_reader_window_cb_delete(Ewl_Widget * w, void * ev, void * data)
{
    ewl_widget_destroy(w);
}

static void
help_reader_prev_button_cb_clicked(Ewl_Widget * w, void * ev, void * data)
{
    Help_Reader_Window *win = data;
    const char * page;

    page = help_reader_window_history_prev(win);
    help_reader_window_page_set(win, page, FALSE);
}

static void
help_reader_next_button_cb_clicked(Ewl_Widget * w, void * ev, void * data)
{
    Help_Reader_Window *win = data;
    const char * page;

    page = help_reader_window_history_next(win);
    help_reader_window_page_set(win, page, FALSE);
}

static void
help_reader_home_button_cb_clicked(Ewl_Widget * w, void * ev, void * data)
{
    Help_Reader_Window *win = data;

    help_reader_window_page_set(win, "index", TRUE);
}

/* history stuff */
static void
help_reader_window_buttons_update(Help_Reader_Window * win)
{
    if (help_reader_window_history_has_next(win))
        ewl_widget_enable(win->next_button);
    else
        ewl_widget_disable(win->next_button);
    
    if (help_reader_window_history_has_prev(win))
        ewl_widget_enable(win->prev_button);
    else
        ewl_widget_disable(win->prev_button);
}

static void
help_reader_window_history_add(Help_Reader_Window * win, const char * path)
{
    /* first remove every item after the current one in the history,
     * ie. remove the future :) */
    while (help_reader_window_history_has_next(win))
    {
        void * data;
        data = ecore_dlist_last_remove(win->history);
        free(data);
    }

    ecore_dlist_append(win->history, strdup(path));

    /* move the iterator to the end */
    ecore_dlist_last_goto(win->history);
    help_reader_window_buttons_update(win);
}

static const char *
help_reader_window_history_next(Help_Reader_Window * win)
{
    ecore_dlist_next(win->history);
    help_reader_window_buttons_update(win);
    return ecore_dlist_current(win->history);
}

static const char *
help_reader_window_history_prev(Help_Reader_Window * win)
{
    ecore_dlist_previous(win->history);
    help_reader_window_buttons_update(win);
    return ecore_dlist_current(win->history);
}

static int
help_reader_window_history_has_next(Help_Reader_Window * win)
{
    int current, last;

    if (ecore_dlist_empty_is(win->history))
        return FALSE;
    current = ecore_dlist_index(win->history);
    last = ecore_dlist_count(win->history) - 1;

    if (current < last)
        return TRUE;

    return FALSE;
}

static int
help_reader_window_history_has_prev(Help_Reader_Window * win)
{
    if (ecore_dlist_empty_is(win->history))
        return FALSE;

    if (ecore_dlist_index(win->history) > 0)
        return TRUE;

    return FALSE;
}

/*
 * Parser functions
 */
static void
help_reader_text_cb_append(void *data, const char *text, 
                                    Eli_Help_Reader_Node *styles, size_t len)
{
    Help_Reader_Window * win = data;
    Ewl_Text * t = EWL_TEXT(win->text);
    size_t i;
    int bold = 0;
    int italic = 0;
    int pref = 0;
    unsigned int size = 12;
    unsigned int r, g, b, a;
    unsigned int t_len;
    unsigned int t_pos;
    r = g = b = 0; a = 255;
    const char * url = NULL;

    for (i = 0;i < len;i++)
    {
        switch (styles[i].type)
        {
        case ELI_HELP_READER_NODE_HEADER:
            {
                unsigned int h = 1;
                if (styles[i].parameter)
                    h = atoi(styles[i].parameter);
                h--;
                if (h > 5)
                    h = 5;

                size = 24 - h * 5;
                bold = 1;
            }
            break;
        case ELI_HELP_READER_NODE_LINK:
            r = 10; g = 0; b = 255; a = 255;
            url = styles[i].parameter;
            break;
        case ELI_HELP_READER_NODE_EMPHASIZE:
            italic = 1;
            break;
        case ELI_HELP_READER_NODE_STRONG:
            bold = 1;
            break;
        case ELI_HELP_READER_NODE_PREFORMATED:
            pref = 1;
            break;
        case ELI_HELP_READER_NODE_TITLE:
            ewl_window_title_set(EWL_WINDOW(win), text);
            return;
        case ELI_HELP_READER_NODE_BLOCK:
        case ELI_HELP_READER_NODE_PARAGRAPH:
        case ELI_HELP_READER_NODE_UNKNOWN: 
        default:
            break;
        }
    }

    if (pref)
        ewl_text_font_set(t, "ewl/monospace");
    else if (bold && italic)
        ewl_text_font_set(t, "ewl/default/bold-italic");
    else if (italic)
        ewl_text_font_set(t, "ewl/default/italic");
    else if (bold)
        ewl_text_font_set(t, "ewl/default/bold");
    else
        ewl_text_font_set(t, NULL);
    
    ewl_text_font_size_set(t, size);
    ewl_text_color_set(t, r, g, b, a);
    t_pos = ewl_text_length_get(t);
    ewl_text_text_append(t, text);
    t_len = ewl_text_length_get(t) - t_pos;

    if (url)
    {
        Ewl_Widget * trig;

        trig = ewl_text_trigger_new(EWL_TEXT_TRIGGER_TYPE_TRIGGER);
        ewl_attach_mouse_cursor_set(trig, EWL_MOUSE_CURSOR_HAND2);
        ewl_text_trigger_start_pos_set(EWL_TEXT_TRIGGER(trig), t_pos);
        ewl_text_trigger_length_set(EWL_TEXT_TRIGGER(trig), t_len);
        ewl_callback_append(trig, EWL_CALLBACK_CLICKED,
                            help_reader_trigger_cb_clicked, win);
        ewl_callback_append(trig, EWL_CALLBACK_DESTROY,
                            help_reader_trigger_cb_destroy, NULL);
        ewl_widget_data_set(trig, TRIGGER_TXT, strdup(url));
        ewl_container_child_append(EWL_CONTAINER(t), trig);
        ewl_widget_show(trig);

    }
}

static void
help_reader_text_cb_leave(void *data, Eli_Help_Reader_Node *styles, size_t len)
{
    Help_Reader_Window *win = data;
    Ewl_Text *t = EWL_TEXT(win->text);
    size_t last = len - 1;
    
    switch (styles[last].type)
    {
    case ELI_HELP_READER_NODE_HEADER:
        ewl_text_text_append(t, "\n");
        ewl_text_font_size_set(t, 6);
        ewl_text_text_append(t, "\n");
        break;
    case ELI_HELP_READER_NODE_BLOCK:
    case ELI_HELP_READER_NODE_PARAGRAPH:
        ewl_text_text_append(t, "\n");
        break;
    default:
        break;
    }
}

static void
help_reader_text_cb_enter(void *data, Eli_Help_Reader_Node *styles, size_t len)
{
    Help_Reader_Window * win = data;
    Ewl_Text * t = EWL_TEXT(win->text);
    size_t last = len - 1;

    if (styles[last].type == ELI_HELP_READER_NODE_PARAGRAPH)
    {
        ewl_text_font_size_set(t, 12);
        ewl_text_text_append(t, "\t");
    }
    if (styles[last].type == ELI_HELP_READER_NODE_HEADER)
    {
        ewl_text_font_size_set(t, 12);
        ewl_text_text_append(t, "\n");
    }
}

/* 
 * trigger functions 
 */
static void 
help_reader_trigger_cb_destroy(Ewl_Widget * w, void * ev, void * data)
{
    void * url;

    url = ewl_widget_data_del(w, TRIGGER_TXT);
    if (url) free(url);
}

static void
help_reader_trigger_cb_clicked(Ewl_Widget * w, void * ev, void * data)
{
    char * url;
    Help_Reader_Window * win = data;

    url = ewl_widget_data_get(w, TRIGGER_TXT);
    help_reader_window_page_set(win, url, 1);
}

