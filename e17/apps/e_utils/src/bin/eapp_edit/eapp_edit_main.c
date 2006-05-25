#include <Eet.h>
#include <Ewl.h>
#include <Ecore_File.h>
#include <Engrave.h>

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define TREE_COLS 2

static void eapp_usage(void);
static int eapp_ui_init(char *file, char *lang, char *winclass);
static int eapp_populate(Ewl_Tree *tree, char *file, char *lang, char *winclass);
static char *eapp_eet_read(Eet_File *ef, char *key, char *lang);
static void eapp_eet_write(Eet_File *ef, char *key, char *lang, char *val, int size);
static void eapp_engrave_write(char *file);

static void eapp_cb_quit(Ewl_Widget *w, void *ev, void *data);
static void eapp_cb_save(Ewl_Widget *w, void *ev, void *data);
static void eapp_cb_fd_show(Ewl_Widget *w, void *ev, void *data);
static void eapp_cb_fd_hide(Ewl_Widget *w, void *ev, void *data);
static void eapp_cb_fd_changed(Ewl_Widget *w, void *ev, void *data);

typedef struct Eapp_Item Eapp_Item;
struct Eapp_Item
{
    char *key;
    char *name;
    int checkbox;
};

static Eapp_Item keys[] = {
            {"app/info/name", "App Name", 0},
            {"app/info/generic", "Generic Info", 0},
            {"app/info/comment", "Comment", 0},
            {"app/info/exe", "Executable", 0},
            {"app/window/name", "Window Name", 0},
            {"app/window/title", "Window Title", 0},
            {"app/window/role", "Window Role", 0},
            {"app/window/class", "Window Class", 0},
            {"app/icon/class", "Icon Class", 0},
            {"app/info/startup_notify", "Startup Notify", 1},
            {"app/info/wait_exit", "Wait Exit", 1}
        };

int
main(int argc, char ** argv)
{
    char *file = NULL;
    char *lang = NULL;
    char *winclass = NULL;
    int ret = 1;
    int i;

    if (!ewl_init(&argc, argv))
    {
        fprintf(stderr, "Error initing EWL.\n");
        goto SHUTDOWN;
    }

    for (i = 1; i < argc; i++)
    {
        if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help")))
        {
            eapp_usage();
            goto EWL_SHUTDOWN;
        }
        else if ((!strcmp(argv[i], "-l")) || (!strcmp(argv[i], "--lang")))
        {
            if (++i < argc)
            {
                if (lang) free(lang);
                lang = strdup(argv[i]);
            }
            else
            {
                fprintf(stderr, "Error, missing argument for lang.\n");
                goto ARGS_SHUTDOWN;
            }
        }
        else if ((!strcmp(argv[i], "-c")) || (!strcmp(argv[i], "--win-class")))
        {
            if (++i < argc)
            {
                if (winclass) free(winclass);
                winclass = strdup(argv[i]);
            }
            else
            {
                fprintf(stderr, "Error, missing argument for win-class.\n");
                goto ARGS_SHUTDOWN;
            }
        }
        else
        {
            if (file) free(file);
            file = strdup(argv[i]);
        }
    }

    if (!file)
    {
        fprintf(stderr, "Error missing file argument.\n");
        eapp_usage();
        goto EWL_SHUTDOWN;
    }

    if (!eet_init())
    {
        fprintf(stderr, "Error initialzing eet.\n");
        goto ARGS_SHUTDOWN;
    }

    if (!eapp_ui_init(file, lang, winclass))
    {
        fprintf(stderr, "Error initializing e_utils_eapp_edit.\n");
        goto EET_SHUTDOWN;
    }

    ewl_main();
    ret = 0;

EET_SHUTDOWN:
    eet_shutdown();
ARGS_SHUTDOWN:
    if (file) free(file);
    if (lang) free(lang);
    if (winclass) free(winclass);
EWL_SHUTDOWN:
    ewl_shutdown();
SHUTDOWN:
    return ret;
}

static void
eapp_usage(void)
{
    printf("usage: e_util_eapp_edit [OPTIONS] file.eap\n"
            "\t if <file.eap> doesn't exist a new file will be created\n\n"
            " [OPTIONS]\n"
            "  -h           \t - view this help screen.\n"
            "  -l [str]     \t - Set the language for the meta data.\n"
            "  -c [str]     \t - Set the window class.\n"
            "\n");
}

static void
eapp_cb_quit(Ewl_Widget *w, void *ev, void *data)
{
    ewl_main_quit();
}

static int
eapp_ui_init(char *file, char *lang, char *winclass)
{
    Ewl_Widget *win, *vbox, *hbox, *tree, *o;
    char tmp[PATH_MAX];
    const char *headers[] = { "Property Name", "Value" };

    snprintf(tmp, PATH_MAX, "%s/data/e_utils_eapp_edit/default.edj", 
                                                    PACKAGE_DATA_DIR);
    win = ewl_window_new();
    ewl_window_title_set(EWL_WINDOW(win), "Eapp Editor");
    ewl_window_class_set(EWL_WINDOW(win), "Eapp Editor");
    ewl_window_name_set(EWL_WINDOW(win), "Eapp_Editor");
    ewl_object_size_request(EWL_OBJECT(win), 300, 450);
    ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, eapp_cb_quit, NULL);
    ewl_widget_show(win);

    vbox = ewl_vbox_new();
    ewl_container_child_append(EWL_CONTAINER(win), vbox);
    ewl_widget_show(vbox);

    tree = ewl_tree_new(TREE_COLS);
    ewl_container_child_append(EWL_CONTAINER(vbox), tree);
    ewl_tree_headers_set(EWL_TREE(tree), (char **)headers);
//    ewl_tree_headers_visible_set(EWL_TREE(tree), FALSE);
    ewl_theme_data_str_set(tree, "/cell/file", tmp);
    ewl_theme_data_str_set(tree, "/cell/group", "moocow");
    ewl_widget_show(tree);

    if (!eapp_populate(EWL_TREE(tree), file, lang, winclass))
    {
        fprintf(stderr, "Error getting eap info.\n");
        return 0;
    }

    hbox = ewl_hbox_new();
    ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
    ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_NONE);
    ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
    ewl_box_spacing_set(EWL_BOX(hbox), 5);
    ewl_widget_show(hbox);

    o = ewl_button_new();
    ewl_button_label_set(EWL_BUTTON(o), "Save");
    ewl_container_child_append(EWL_CONTAINER(hbox), o);
    ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
    ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
    ewl_widget_data_set(o, "file", file);
    ewl_widget_data_set(o, "lang", lang);
    ewl_callback_append(o, EWL_CALLBACK_CLICKED, eapp_cb_save, NULL);
    ewl_widget_show(o);

    o = ewl_button_new();
    ewl_button_label_set(EWL_BUTTON(o), "Cancel");
    ewl_container_child_append(EWL_CONTAINER(hbox), o);
    ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
    ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
    ewl_callback_append(o, EWL_CALLBACK_CLICKED, eapp_cb_quit, NULL);
    ewl_widget_show(o);

    return 1;
}

static int
eapp_populate(Ewl_Tree *tree, char *file, char *lang, char *winclass)
{
    Ewl_Widget *row[TREE_COLS];
    Eet_File *ef = NULL;
    char *v;
    int i;

    if (ecore_file_exists(file))
    {
        ef = eet_open(file, EET_FILE_MODE_READ);
        if (!ef)
        {
            fprintf(stderr, "Error, unable to open eap file.");
            return 0;
        }
    }

    row[0] = ewl_label_new();
    ewl_label_text_set(EWL_LABEL(row[0]), "Set Icon");
    ewl_object_alignment_set(EWL_OBJECT(row[0]), EWL_FLAG_ALIGN_LEFT);
    ewl_object_fill_policy_set(EWL_OBJECT(row[0]), EWL_FLAG_FILL_NONE);
    ewl_widget_show(row[0]);

    /* add the icon */
    row[1] = ewl_image_new();
    ewl_image_file_set(EWL_IMAGE(row[1]), file, "icon");
    ewl_image_proportional_set(EWL_IMAGE(row[1]), TRUE);
    ewl_image_size_set(EWL_IMAGE(row[1]), 32, 32);
    /* FIXME: This can give it a decent background, but introduces a sizing
     * issue to track down
     * ewl_widget_appearance_set(row[1], "entry"); */
    ewl_widget_name_set(row[1], "icon");
    ewl_widget_show(row[1]);

    row[0] = ewl_tree_row_add(tree, NULL, row);
    ewl_callback_append(row[0], EWL_CALLBACK_CLICKED, eapp_cb_fd_show, NULL);

    /* add all the eet data */
    for (i = 0; i < (sizeof(keys) / sizeof(keys[0])); i++)
    {
        row[0] = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(row[0]), keys[i].name);
        ewl_object_fill_policy_set(EWL_OBJECT(row[0]), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(row[0]), EWL_FLAG_ALIGN_LEFT);
        ewl_widget_show(row[0]);

        v = eapp_eet_read(ef, keys[i].key, lang);
        if (keys[i].checkbox)
        {
            row[1] = ewl_checkbutton_new();
            ewl_button_label_set(EWL_BUTTON(row[1]), "");
            ewl_checkbutton_checked_set(EWL_CHECKBUTTON(row[1]), v[0] == 1);
        }
        else
        {
            if (!strcmp(keys[i].key, "app/window/class"))
            {
                if (winclass) v = winclass;
            }

            row[1] = ewl_entry_new();
            ewl_text_text_set(EWL_TEXT(row[1]), v);
        }
        ewl_widget_name_set(row[1], keys[i].key);
        ewl_widget_show(row[1]);
        if (v) free(v);
        v = NULL;

        ewl_tree_row_add(tree, NULL, row);
    }
    
    if (ef) eet_close(ef);

    return 1;
}

static void
eapp_cb_fd_show(Ewl_Widget *w, void *ev, void *data)
{
    Ewl_Widget *fd;

    fd = ewl_widget_name_find("fd");
    if (!fd)
    {
        fd = ewl_filedialog_new();
    	ewl_widget_name_set(fd, "fd");
        ewl_callback_append(fd, EWL_CALLBACK_VALUE_CHANGED,
                                eapp_cb_fd_changed, NULL);
        ewl_widget_show(fd);
    }
    ewl_widget_show(fd);
}

static void
eapp_cb_fd_hide(Ewl_Widget *w, void *ev, void *data)
{
    ewl_widget_hide(w);
}

static void
eapp_cb_fd_changed(Ewl_Widget *w, void *ev, void *data)
{
    Ewl_Dialog_Event *e;
    Ewl_Widget *o;
    
    e = ev;
    if (e->response == EWL_STOCK_OK)
    {
        char *icon;
        const char *icon2;
        char icon3[PATH_MAX];
	
        icon = ewl_filedialog_selected_file_get(EWL_FILEDIALOG(w));
        icon2 = ewl_filedialog_directory_get(EWL_FILEDIALOG(w));
        snprintf(icon3, PATH_MAX, "%s/%s", icon2, icon);

        o = ewl_widget_name_find("icon");
        ewl_widget_data_set(o, "file", strdup(icon3));
        ewl_image_file_set(EWL_IMAGE(o), icon3, "");
    }

    o = ewl_widget_name_find("fd");
    ewl_widget_hide(o);
}

static void
eapp_cb_save(Ewl_Widget *w, void *ev, void *data)
{
    Eet_File *ef;
    char *file = NULL, *lang = NULL;
    int i;

    file = ewl_widget_data_get(w, "file");
    lang = ewl_widget_data_get(w, "lang");

    eapp_engrave_write(file);

    ef = eet_open(file, EET_FILE_MODE_READ_WRITE);
    if (!ef)
    {
        fprintf(stderr, "Error opening file for READ/WRITE.\n");
        return;
    }

    /* add all the eet data */
    for (i = 0; i < (sizeof(keys) / sizeof(keys[0])); i++)
    {
        Ewl_Widget *o;
        char *v = NULL;
        int s = 0;
        char c;

        o = ewl_widget_name_find(keys[i].key);
        if (keys[i].checkbox)
        {
            s = 1;
            c = ewl_checkbutton_is_checked(EWL_CHECKBUTTON(o));
            v = &c;
        }
        else
        {
            v = ewl_text_text_get(EWL_TEXT(o));
            if (v) s = strlen(v);
        }

        eapp_eet_write(ef, keys[i].key, lang, v, s);
    }

    eet_close(ef);
    eapp_cb_quit(NULL, NULL, NULL);
}

static char *
eapp_eet_read(Eet_File *ef, char *key, char *lang)
{
    if (ef)
    {
        int size;
        char buf[4096];
        char *ret, *r;

        if (lang)
            snprintf(buf, sizeof(buf), "%s[%s]", key, lang);
        else
            snprintf(buf, sizeof(buf), "%s", key);

        r = eet_read(ef, buf, &size);
        if (r)
        {
            ret = malloc(sizeof(char) * (size + 1));
            if (ret)
            {
                snprintf(ret, size + 1, "%s", r);
            }
            else ret = strdup("");

            free(r);
            return ret;
        }
    }
    return strdup("");
}

static void
eapp_eet_write(Eet_File *ef, char *key, char *lang, char *val, int size)
{
    char buf[4096];

    if (ef)
    {
        if (lang)
            snprintf(buf, sizeof(buf), "%s[%s]", key, lang);
        else
            snprintf(buf, sizeof(buf), "%s", key);

        if (size == 0)
            eet_delete(ef, buf);
        else
            eet_write(ef, buf, val, size, 0);
    }
}

static void
eapp_engrave_write(char *file)
{
    Engrave_File *eet;
    Ewl_Widget *o;
    char *icon;

    o = ewl_widget_name_find("icon");
    icon = ewl_widget_data_get(o, "file");
    if (icon)
    {
        char *icon_dir, *icon_file;
        Engrave_Image *image;
        Engrave_Group *grp;
        Engrave_Part *part;
        Engrave_Part_State *ps;

        icon_file = strrchr(icon, '/');
        *icon_file = '\0';
        icon_dir = strdup(icon);

        *icon_file = '/';
        icon_file++;

        eet = engrave_file_new();
        engrave_file_image_dir_set(eet, icon_dir);
        image = engrave_image_new(icon_file, ENGRAVE_IMAGE_TYPE_COMP, 0);
        engrave_file_image_add(eet, image);

        grp = engrave_group_new();
        engrave_group_name_set(grp, "icon");
        engrave_group_max_size_set(grp, 48, 48);
        engrave_file_group_add(eet, grp);

        part = engrave_part_new(ENGRAVE_PART_TYPE_IMAGE);
        engrave_part_name_set(part, "image");
        engrave_part_mouse_events_set(part, 0);
        engrave_group_part_add(grp, part);
    
        ps = engrave_part_state_new();
        engrave_part_state_name_set(ps, "default", 0.0);
        engrave_part_state_aspect_set(ps, 1.0, 1.0);
        engrave_part_state_image_normal_set(ps, image);
        engrave_part_state_add(part, ps);

        engrave_edj_output(eet, file);
        engrave_file_free(eet);

        free(icon_dir);
    }
    else
    {
        /* if the file dosen't exist, create it */
        if (!ecore_file_exists(file))
        {
            eet = engrave_file_new();
            engrave_edj_output(eet, file);
            engrave_file_free(eet);
        }
    }
}

