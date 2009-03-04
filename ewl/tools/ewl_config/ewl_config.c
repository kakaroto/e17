/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include <Ewl.h>
#include "ewl_private.h"
#include "ewl_debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#define EC_WIN_MAIN "ec/window/main"
#define EC_WIN_ABOUT "ec/window/about"
#define EC_EWL_THEME "ec/ewl/theme"
#define EC_ICON_THEME "ec/icon/theme"
#define EC_ICON_SIZE "ec/icon/size"
#define EC_SAVE_SYSTEM  "ec/save/system"
#define EC_ENGINE_NAME "ec/engine/name"
#define EC_EVAS_IMAGE_CACHE "ec/evas/image/cache"
#define EC_EVAS_FONT_CACHE "ec/evas/font/cache"
#define EC_DEBUG_ENABLE "ec/debug/enable"
#define EC_DEBUG_LEVEL "ec/debug/level"
#define EC_DEBUG_SEGV "ec/debug/segv"
#define EC_DEBUG_BACKTRACE "ec/debug/backtrace"
#define EC_DEBUG_EVAS_RENDER "ec/debug/evas/render"
#define EC_DEBUG_GARBAGE "ec/debug/garbage"
#define EC_THEME_KEYS "ec/theme/keys"
#define EC_THEME_SIGNALS "ec/theme/signals"

static void usage(void);
static void ec_main_win(int save_system);

static void ec_menubar_setup(Ewl_Menubar *m);
static void ec_theme_page_setup(Ewl_Notebook *n);
static void ec_engine_page_setup(Ewl_Notebook *n);
static void ec_debug_page_setup(Ewl_Notebook *n);

static void ec_cb_delete_window(Ewl_Widget *w, void *ev, void *data);
static void ec_cb_about(Ewl_Widget *w, void *ev, void *data);
static void ec_cb_apply(Ewl_Widget *w, void *ev, void *data);
static void ec_cb_revert(Ewl_Widget *w, void *ev, void *data);
static void ec_cb_win_hide(Ewl_Widget *w, void *ev, void *data);

int ec_themes_get(DIR *rep, Ecore_List *list, const char *v);

typedef struct Ec_Gui_Menu Ec_Gui_Menu;
struct Ec_Gui_Menu
{
        char *name;
        Ewl_Menu_Info *info;
};


int
main(int argc, char ** argv)
{
        int ret = 1, i, did_something = 0, save_system = 0;

        if (!ewl_init(&argc, argv))
        {
                fprintf(stderr, "Unable to init ewl.\n");
                goto EXIT;
        }

        for (i = 1; i < argc; i++)
        {
                if ((!strncmp(argv[i], "-s", 2))
                                || (!strncmp(argv[i], "-set", 4)))
                {
                        if (++i < argc)
                        {
                                char *t, *key, *val;

                                t = strdup(argv[i]);
                                key = t;
                                val = strchr(t, '=');

                                if (!val)
                                {
                                        usage();
                                        goto EWL_SHUTDOWN;
                                }

                                *val = '\0';
                                val++;

                                ewl_config_string_set(ewl_config, key, val,
                                                        EWL_STATE_PERSISTENT);
                                free(t);

                                did_something = 1;
                        }
                        else
                        {
                                usage();
                                goto EWL_SHUTDOWN;
                        }
                }
                else if ((!strncmp(argv[i], "-g", 2))
                                || (!strncmp(argv[i], "-get", 4)))
                {
                        if (++i < argc)
                        {
                                const char *val;

                                val = ewl_config_string_get(ewl_config, argv[i]);
                                printf("%s = %s\n", argv[i], val);

                                did_something = 1;
                        }
                        else
                        {
                                usage();
                                goto EWL_SHUTDOWN;
                        }
                }
                else if (!strncmp(argv[i], "-r", 2))
                {
                        if (!ewl_config_can_save_system(ewl_config))
                        {
                                fprintf(stderr, "Insufficient permission to save "
                                                "system configuration.\n");
                                goto EWL_SHUTDOWN;
                        }
                        save_system = 1;
                }
                else if ((!strncmp(argv[i], "-h", 2))
                                || (!strncasecmp(argv[i], "--help", 6)))
                {
                        usage();
                        ret = 0;
                        goto EWL_SHUTDOWN;
                }
        }

        if (did_something)
        {
                if (save_system)
                        ewl_config_system_save(ewl_config);
                else
                        ewl_config_user_save(ewl_config);
        }
        else
        {
                ec_main_win(save_system);
                ewl_main();
        }

        ret = 0;

EWL_SHUTDOWN:
        ewl_shutdown();
EXIT:
        return ret;
}

static void
usage(void)
{
        printf("\n"
                "ewl_config [OPTIONS] [-set key=value] [-get key]\n"
                " OPTIONS\n"
                "  -r                    \t - Save the system configuration file\n"
                "  -h|--help             \t - Print help information\n"
                "\n");
}

static void
ec_main_win(int save_system)
{
        Ewl_Widget *box, *o2, *o;

        o = ewl_window_new();
        ewl_window_title_set(EWL_WINDOW(o), "EWL Configuration");
        ewl_window_class_set(EWL_WINDOW(o), "ewl_config");
        ewl_window_name_set(EWL_WINDOW(o), "ewl_config");
        ewl_object_size_request(EWL_OBJECT(o), 450, 250);
        ewl_widget_name_set(o, EC_WIN_MAIN);
        ewl_callback_append(o, EWL_CALLBACK_DELETE_WINDOW,
                                        ec_cb_delete_window, NULL);
        ewl_widget_show(o);

        box = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(o), box);
        ewl_widget_show(box);

        o = ewl_hmenubar_new();
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ec_menubar_setup(EWL_MENUBAR(o));
        ewl_widget_show(o);

        o = ewl_notebook_new();
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_notebook_tabbar_alignment_set(EWL_NOTEBOOK(o), EWL_FLAG_ALIGN_LEFT);
        ewl_widget_show(o);

        ec_theme_page_setup(EWL_NOTEBOOK(o));
        ec_engine_page_setup(EWL_NOTEBOOK(o));
        ec_debug_page_setup(EWL_NOTEBOOK(o));

        o2 = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), o2);
        ewl_object_fill_policy_set(EWL_OBJECT(o2), EWL_FLAG_FILL_HFILL);
        ewl_widget_show(o2);

        o = ewl_button_new();
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_APPLY);
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ec_cb_apply, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_VFILL);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Revert");
        ewl_button_image_set(EWL_BUTTON(o),
                        ewl_icon_theme_icon_path_get(EWL_ICON_DIALOG_CANCEL,
                                                        EWL_ICON_SIZE_MEDIUM),
                        EWL_ICON_DIALOG_CANCEL);
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ec_cb_revert, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_VFILL);
        ewl_widget_show(o);

        o = ewl_button_new();
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_QUIT);
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ec_cb_delete_window, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_VFILL);
        ewl_widget_show(o);

        o = ewl_spacer_new();
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_widget_show(o);

        o = ewl_checkbutton_new();
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_togglebutton_checked_set(EWL_TOGGLEBUTTON(o), save_system);
        ewl_button_label_set(EWL_BUTTON(o), "Save as system configuration");
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_widget_name_set(o, EC_SAVE_SYSTEM);
        ewl_widget_show(o);

        if (!ewl_config_can_save_system(ewl_config))
                ewl_widget_disable(o);
}

static void
ec_cb_delete_window(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        ewl_main_quit();
}

#define EC_ICON(icon) ewl_icon_theme_icon_path_get(EWL_ICON_ ## icon, EWL_ICON_SIZE_SMALL)
static void
ec_menubar_setup(Ewl_Menubar *m)
{
        Ewl_Widget *menu, *o;
        int i;

        Ewl_Menu_Info file_menu[] = {
                {"Save", EC_ICON(DOCUMENT_SAVE), ec_cb_apply},
                {"Revert", EC_ICON(DOCUMENT_REVERT), ec_cb_revert},
                {"Quit", EC_ICON(APPLICATION_EXIT), ec_cb_delete_window},
                {NULL, NULL, NULL}
        };

        Ewl_Menu_Info help_menu[] = {
                {"About", EC_ICON(HELP_ABOUT), ec_cb_about},
                {NULL, NULL, NULL}
        };

        Ec_Gui_Menu menus[] = {
                {"File", file_menu},
                {"Help", help_menu},
                {NULL, NULL}
        };

        for (i = 0; menus[i].name != NULL; i++)
        {
                /* pack in a spacer before the help text */
                if (menus[i + 1].name == NULL)
                {
                        o = ewl_spacer_new();
                        ewl_container_child_append(EWL_CONTAINER(m), o);
                        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_FILL);
                        ewl_widget_show(o);
                }

                menu = ewl_menu_new();
                ewl_button_label_set(EWL_BUTTON(menu), menus[i].name);
                ewl_container_child_append(EWL_CONTAINER(m), menu);
                ewl_object_fill_policy_set(EWL_OBJECT(menu),
                                EWL_FLAG_FILL_VFILL);
                ewl_menu_from_info(EWL_MENU(menu), menus[i].info);
                ewl_widget_show(menu);
        }
}

static void
ec_theme_page_setup(Ewl_Notebook *n)
{
        Ewl_Widget *box, *o, *o2, *o3;
        DIR *rep;
        Ecore_List *list;
        const char *v;
        int val;
        int sel = -1;
        char *home_dir, path[PATH_MAX];

        box = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(n), box);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(n), box, "Theme");
        ewl_widget_show(box);

        o2 = ewl_border_new();
        ewl_border_label_set(EWL_BORDER(o2), "EWL Theme");
        ewl_object_fill_policy_set(EWL_OBJECT(o2), EWL_FLAG_FILL_VFILL);
        ewl_container_child_append(EWL_CONTAINER(box), o2);
        ewl_widget_show(o2);

        o3 = ewl_scrollpane_new();
        ewl_object_fill_policy_set(EWL_OBJECT(o3), EWL_FLAG_FILL_VFILL
                        | EWL_FLAG_FILL_VSHRINK);
        ewl_container_child_append(EWL_CONTAINER(o2), o3);
        ewl_widget_show(o3);

        o = ewl_list_new();
        list = ecore_list_new();
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_HFILL);
        ewl_widget_name_set(o, EC_EWL_THEME);
        ewl_container_child_append(EWL_CONTAINER(o3), o);
        ewl_mvc_model_set(EWL_MVC(o), ewl_model_ecore_list_instance());
        ewl_mvc_view_set(EWL_MVC(o), ewl_label_view_get());
        ewl_mvc_data_set(EWL_MVC(o), list);
        ewl_widget_show(o);

        v = ewl_config_string_get(ewl_config, EWL_CONFIG_THEME_NAME);
        rep = opendir(PACKAGE_DATA_DIR "/ewl/themes");
        if (rep)
        {
                int ret;

                ret = ec_themes_get(rep, list, v);
                if (ret >= 0)
                        sel = ret;
                closedir(rep);
        }

        home_dir = getenv("HOME");
        snprintf(path, PATH_MAX, "%s/%s", home_dir, ".ewl/themes");
        rep = opendir(path);
        if (rep)
        {
                int ret;

                ret = ec_themes_get(rep, list, v);
                if (ret >= 0)
                        sel = ret;

                closedir(rep);
        }

        if (sel >= 0)
                ewl_mvc_selected_set(EWL_MVC(o), NULL, list, sel, 0);
        ewl_mvc_dirty_set(EWL_MVC(o), TRUE);

        o2 = ewl_border_new();
        ewl_border_label_set(EWL_BORDER(o2), "Icon Theme");
        ewl_container_child_append(EWL_CONTAINER(box), o2);
        ewl_widget_show(o2);

        box = ewl_grid_new();
        ewl_grid_dimensions_set(EWL_GRID(box), 2, 2);
        ewl_grid_column_preferred_w_use(EWL_GRID(box), 0);
        ewl_container_child_append(EWL_CONTAINER(o2), box);
        ewl_widget_show(box);

        o = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(o), "Theme name");
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        o = ewl_entry_new();
        ewl_widget_name_set(o, EC_ICON_THEME);
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_text_text_set(EWL_TEXT(o),
                ewl_config_string_get(ewl_config, EWL_CONFIG_THEME_ICON_THEME));
        ewl_widget_show(o);

        o = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(o), "Icon Size");
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        val = ewl_config_int_get(ewl_config, EWL_CONFIG_THEME_ICON_SIZE);

        o = ewl_spinner_new();
        ewl_range_minimum_value_set(EWL_RANGE(o), 8.0);
        ewl_range_maximum_value_set(EWL_RANGE(o), 192.0);
        ewl_range_step_set(EWL_RANGE(o), 1.0);
        ewl_range_value_set(EWL_RANGE(o), val);
        ewl_spinner_digits_set(EWL_SPINNER(o), 0);
        ewl_widget_name_set(o, EC_ICON_SIZE);
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);
}

static void
ec_engine_page_setup(Ewl_Notebook *n)
{
        Ewl_Widget *box, *o, *o2;

        box = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(n), box);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(n), box, "Engine");
        ewl_widget_show(box);

        o2 = ewl_grid_new();
        ewl_grid_column_preferred_w_use(EWL_GRID(o2), 0);
        ewl_container_child_append(EWL_CONTAINER(box), o2);
        ewl_object_fill_policy_set(EWL_OBJECT(o2), EWL_FLAG_FILL_HFILL);
        ewl_widget_show(o2);

        o = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(o), "Engine name");
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_widget_show(o);

        o = ewl_entry_new();
        ewl_widget_name_set(o, EC_ENGINE_NAME);
        ewl_text_text_set(EWL_TEXT(o),
                ewl_config_string_get(ewl_config, EWL_CONFIG_ENGINE_NAME));
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_widget_show(o);

        o = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(o), "Evas font cache size");
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_widget_show(o);

        o = ewl_entry_new();
        ewl_widget_name_set(o, EC_EVAS_FONT_CACHE);
        ewl_text_text_set(EWL_TEXT(o),
                ewl_config_string_get(ewl_config, EWL_CONFIG_CACHE_EVAS_FONT));
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_widget_show(o);

        o = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(o), "Evas image cache size");
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_widget_show(o);

        o = ewl_entry_new();
        ewl_widget_name_set(o, EC_EVAS_IMAGE_CACHE);
        ewl_text_text_set(EWL_TEXT(o),
                ewl_config_string_get(ewl_config, EWL_CONFIG_CACHE_EVAS_IMAGE));
        ewl_container_child_append(EWL_CONTAINER(o2), o);
        ewl_widget_show(o);
}

static void
ec_debug_page_setup(Ewl_Notebook *n)
{
        Ewl_Widget *box, *o2;
        int i;

        struct
        {
                char *label;
                char *key;
                char *name;
                int spinner;
        } buttons[] = {
                {"Enabled", EWL_CONFIG_DEBUG_ENABLE, EC_DEBUG_ENABLE, 0},
                {"Level", EWL_CONFIG_DEBUG_LEVEL, EC_DEBUG_LEVEL, 1},
                {"Segv", EWL_CONFIG_DEBUG_SEGV, EC_DEBUG_SEGV, 0},
                {"Backtrace", EWL_CONFIG_DEBUG_BACKTRACE, EC_DEBUG_BACKTRACE, 0},
                {"Evas rendering", EWL_CONFIG_DEBUG_EVAS_RENDER, EC_DEBUG_EVAS_RENDER, 0},
                {"Garbage Collection", EWL_CONFIG_DEBUG_GC_REAP, EC_DEBUG_GARBAGE, 0},
                {"Print theme keys", EWL_CONFIG_THEME_PRINT_KEYS, EC_THEME_KEYS, 0},
                {"Print theme signals", EWL_CONFIG_THEME_PRINT_SIGNALS, EC_THEME_SIGNALS, 0},
                {NULL, NULL, NULL, 0}
        };

        box = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(n), box);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(n), box, "Debug");
        ewl_widget_show(box);

        o2 = ewl_grid_new();
        ewl_grid_vhomogeneous_set(EWL_GRID(o2), TRUE);
        ewl_container_child_append(EWL_CONTAINER(box), o2);
        ewl_object_fill_policy_set(EWL_OBJECT(o2), EWL_FLAG_FILL_HFILL);
        ewl_widget_show(o2);

        for (i = 0; buttons[i].label != NULL; i++)
        {
                Ewl_Widget *o;

                if (buttons[i].spinner)
                {
                        Ewl_Widget *s;

                        o = ewl_hbox_new();
                        ewl_widget_show(o);

                        s = ewl_label_new();
                        ewl_label_text_set(EWL_LABEL(s), buttons[i].label);
                        ewl_container_child_append(EWL_CONTAINER(o), s);
                        ewl_widget_show(s);

                        s = ewl_spinner_new();
                        ewl_widget_name_set(s, buttons[i].name);
                        ewl_spinner_digits_set(EWL_SPINNER(s), 0);
                        ewl_range_minimum_value_set(EWL_RANGE(s), 0);
                        ewl_range_maximum_value_set(EWL_RANGE(s), 100);
                        ewl_range_step_set(EWL_RANGE(s), 1);
                        ewl_range_value_set(EWL_RANGE(s),
                                ewl_config_int_get(ewl_config, buttons[i].key));
                        ewl_container_child_append(EWL_CONTAINER(o), s);
                        ewl_widget_show(s);
                }
                else
                {
                        o = ewl_checkbutton_new();
                        ewl_widget_name_set(o, buttons[i].name);
                        ewl_object_alignment_set(EWL_OBJECT(o),
                                        EWL_FLAG_ALIGN_LEFT);
                        ewl_button_label_set(EWL_BUTTON(o), buttons[i].label);
                        ewl_togglebutton_checked_set(EWL_TOGGLEBUTTON(o),
                                ewl_config_int_get(ewl_config, buttons[i].key));
                        ewl_widget_show(o);
                }
                ewl_container_child_append(EWL_CONTAINER(o2), o);
        }
}

static void
ec_cb_about(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Widget *about, *o;

        about = ewl_widget_name_find(EC_WIN_ABOUT);
        if (about)
        {
                ewl_widget_show(about);
                return;
        }

        about = ewl_dialog_new();
        ewl_window_title_set(EWL_WINDOW(about), "About EWL Configuration");
        ewl_window_name_set(EWL_WINDOW(about), "about ewl_config");
        ewl_window_class_set(EWL_WINDOW(about), "ewl_config");
        ewl_widget_name_set(about, EC_WIN_ABOUT);
        ewl_window_transient_for(EWL_WINDOW(about),
                        EWL_WINDOW(ewl_widget_name_find(EC_WIN_MAIN)));
        ewl_callback_append(about, EWL_CALLBACK_DELETE_WINDOW,
                                                        ec_cb_win_hide, about);
        ewl_widget_show(about);
        ewl_dialog_active_area_set(EWL_DIALOG(about), EWL_POSITION_TOP);

        o = ewl_text_new();
        ewl_container_child_append(EWL_CONTAINER(about), o);
        ewl_widget_show(o);

        ewl_text_wrap_set(EWL_TEXT(o), EWL_TEXT_WRAP_WORD);

        ewl_text_font_size_set(EWL_TEXT(o), 25);
        ewl_text_font_set(EWL_TEXT(o), "ewl/default/bold");
        ewl_text_align_set(EWL_TEXT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_text_styles_set(EWL_TEXT(o), EWL_TEXT_STYLE_SOFT_SHADOW);
        ewl_text_text_set(EWL_TEXT(o), "EWL Config\n");

        ewl_text_font_size_set(EWL_TEXT(o), 10);
        ewl_text_font_set(EWL_TEXT(o), "ewl/default");
        ewl_text_align_set(EWL_TEXT(o), EWL_FLAG_ALIGN_LEFT);
        ewl_text_styles_set(EWL_TEXT(o), EWL_TEXT_STYLE_NONE);
        ewl_text_text_append(EWL_TEXT(o), "\n");
        ewl_text_text_append(EWL_TEXT(o), "Configure your EWL install.\n");
        ewl_text_text_append(EWL_TEXT(o), "\n");

        ewl_dialog_active_area_set(EWL_DIALOG(about), EWL_POSITION_BOTTOM);

        o = ewl_button_new();
        ewl_container_child_append(EWL_CONTAINER(about), o);
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_OK);
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ec_cb_win_hide, about);
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_widget_show(o);
}

static void
ec_cb_win_hide(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *data)
{
        ewl_widget_hide(EWL_WIDGET(data));
}

static void
ec_cb_revert(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Widget *o;
        Ecore_List *list;
        int val, sel = 0;
        const char *v, *lval;

        struct
        {
                char *name;
                char *key;
        } checks[] = {
                {EC_DEBUG_ENABLE, EWL_CONFIG_DEBUG_ENABLE},
                {EC_DEBUG_SEGV, EWL_CONFIG_DEBUG_SEGV},
                {EC_DEBUG_BACKTRACE, EWL_CONFIG_DEBUG_BACKTRACE},
                {EC_DEBUG_EVAS_RENDER, EWL_CONFIG_DEBUG_EVAS_RENDER},
                {EC_DEBUG_GARBAGE, EWL_CONFIG_DEBUG_GC_REAP},
                {EC_THEME_KEYS, EWL_CONFIG_THEME_PRINT_KEYS},
                {EC_THEME_SIGNALS, EWL_CONFIG_THEME_PRINT_SIGNALS},
                {NULL, NULL}
        };

        struct
        {
                char *name;
                char *key;
        } strings[] = {
                {EC_ICON_THEME, EWL_CONFIG_THEME_ICON_THEME},
                {EC_ENGINE_NAME, EWL_CONFIG_ENGINE_NAME},
                {EC_EVAS_IMAGE_CACHE, EWL_CONFIG_CACHE_EVAS_IMAGE},
                {EC_EVAS_FONT_CACHE, EWL_CONFIG_CACHE_EVAS_FONT},
                {NULL, NULL}
        };

        o = ewl_widget_name_find(EC_EWL_THEME);
        list = ewl_mvc_data_get(EWL_MVC(o));
        v = ewl_config_string_get(ewl_config, EWL_CONFIG_THEME_NAME);

        ecore_list_first_goto(list);
        while ((lval = ecore_list_next(list)))
        {
                if (!strcmp(v, lval))
                        break;
                sel++;
        }
        ewl_mvc_selected_set(EWL_MVC(o), NULL, list, sel, 0);

        o = ewl_widget_name_find(EC_ICON_SIZE);
        val = ewl_config_int_get(ewl_config, EWL_CONFIG_THEME_ICON_SIZE);
        ewl_range_value_set(EWL_RANGE(o), val);

        for (sel = 0; strings[sel].name != NULL; sel++)
        {
                o = ewl_widget_name_find(strings[sel].name);
                ewl_text_text_set(EWL_TEXT(o),
                        ewl_config_string_get(ewl_config, strings[sel].key));
        }

        o = ewl_widget_name_find(EC_DEBUG_LEVEL);
        ewl_range_value_set(EWL_RANGE(o),
                ewl_config_int_get(ewl_config, EWL_CONFIG_DEBUG_LEVEL));

        for (sel = 0; checks[sel].name != NULL; sel++)
        {
                o = ewl_widget_name_find(checks[sel].name);
                ewl_togglebutton_checked_set(EWL_TOGGLEBUTTON(o),
                        ewl_config_int_get(ewl_config, checks[sel].key));
        }
}

static void
ec_cb_apply(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Widget *o;
        Ecore_List *list;
        int val, i;
        const char *v;
        Ewl_Selection_Idx *idx;

        struct
        {
                char *name;
                char *key;
        } checks[] = {
                {EC_DEBUG_ENABLE, EWL_CONFIG_DEBUG_ENABLE},
                {EC_DEBUG_SEGV, EWL_CONFIG_DEBUG_SEGV},
                {EC_DEBUG_BACKTRACE, EWL_CONFIG_DEBUG_BACKTRACE},
                {EC_DEBUG_EVAS_RENDER, EWL_CONFIG_DEBUG_EVAS_RENDER},
                {EC_DEBUG_GARBAGE, EWL_CONFIG_DEBUG_GC_REAP},
                {EC_THEME_KEYS, EWL_CONFIG_THEME_PRINT_KEYS},
                {EC_THEME_SIGNALS, EWL_CONFIG_THEME_PRINT_SIGNALS},
                {NULL, NULL}
        };

        struct
        {
                char *name;
                char *key;
        } strings[] = {
                {EC_ICON_THEME, EWL_CONFIG_THEME_ICON_THEME},
                {EC_ENGINE_NAME, EWL_CONFIG_ENGINE_NAME},
                {EC_EVAS_IMAGE_CACHE, EWL_CONFIG_CACHE_EVAS_IMAGE},
                {EC_EVAS_FONT_CACHE, EWL_CONFIG_CACHE_EVAS_FONT},
                {NULL, NULL}
        };

        for (i = 0; checks[i].name != NULL; i++)
        {
                o = ewl_widget_name_find(checks[i].name);
                if ((int)ewl_togglebutton_checked_get(EWL_TOGGLEBUTTON(o)) !=
                                ewl_config_int_get(ewl_config, checks[i].key))
                        ewl_config_int_set(ewl_config, checks[i].key,
                                        ewl_togglebutton_checked_get(EWL_TOGGLEBUTTON(o)),
                                        EWL_STATE_PERSISTENT);
        }

        for (i = 0; strings[i].name != NULL; i++)
        {
                char *new;

                o = ewl_widget_name_find(strings[i].name);
                new = ewl_text_text_get(EWL_TEXT(o));
                if (strcmp(new, ewl_config_string_get(ewl_config,
                                                strings[i].key)))
                {
                        ewl_config_string_set(ewl_config,
                                        strings[i].key, new,
                                        EWL_STATE_PERSISTENT);
                }
        }

        o = ewl_widget_name_find(EC_DEBUG_LEVEL);
        if (ewl_range_value_get(EWL_RANGE(o)) !=
                        ewl_config_int_get(ewl_config, EWL_CONFIG_DEBUG_LEVEL))
        {
                ewl_config_int_set(ewl_config, EWL_CONFIG_DEBUG_LEVEL,
                                        ewl_range_value_get(EWL_RANGE(o)),
                                        EWL_STATE_PERSISTENT);
        }

        o = ewl_widget_name_find(EC_ICON_SIZE);
        val = ewl_range_value_get(EWL_RANGE(o));
        if (val != ewl_config_int_get(ewl_config, EWL_CONFIG_THEME_ICON_SIZE))
        {
                ewl_config_int_set(ewl_config,
                                EWL_CONFIG_THEME_ICON_SIZE, val,
                                EWL_STATE_PERSISTENT);
        }

        o = ewl_widget_name_find(EC_EWL_THEME);
        list = ewl_mvc_data_get(EWL_MVC(o));
        idx = ewl_mvc_selected_get(EWL_MVC(o));
        if (idx)
        {
                ecore_list_index_goto(list, idx->row);
                v = ecore_list_current(list);

                if (strcmp(v, ewl_config_string_get(ewl_config,
                                                EWL_CONFIG_THEME_NAME)))
                {
                        ewl_config_string_set(ewl_config,
                                        EWL_CONFIG_THEME_NAME, v,
                                        EWL_STATE_PERSISTENT);
                }
        }

        o = ewl_widget_name_find(EC_SAVE_SYSTEM);
        if (ewl_togglebutton_checked_get(EWL_TOGGLEBUTTON(o)))
        {
                if (!ewl_config_can_save_system(ewl_config))
                        fprintf(stderr, "Cannot save system config.\n");
                else
                        ewl_config_system_save(ewl_config);
        }
        else
                ewl_config_user_save(ewl_config);
}

int
ec_themes_get(DIR *rep, Ecore_List *list, const char *v)
{
        struct dirent *file;
        int z = -1;

        while ((file = readdir(rep)))
        {
                int len;

                len = strlen(file->d_name);

                if ((len >= 4) &&
                                (!strcmp(file->d_name + len - 4, ".edj")))
                {
                        char *t;

                        t = strdup(file->d_name);
                        *(t + len - 4) = '\0';

                        ecore_list_append(list, t);

                        if (!strcmp(t, v))
                                z = ecore_list_count(list) - 1;
                }
        }

        return z;
}


