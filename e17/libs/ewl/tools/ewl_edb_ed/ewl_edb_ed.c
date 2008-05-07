#include <Ewl.h>
#include <Edb.h>

static E_DB_File *db_file = NULL;

typedef enum {
        EWL_EDB_TYPE_NONE,
        EWL_EDB_TYPE_INT,
        EWL_EDB_TYPE_STR,
        EWL_EDB_TYPE_FLOAT,
        EWL_EDB_TYPE_DATA
} Ewl_Edb_Type;

static Ewl_Edb_Type current_type = EWL_EDB_TYPE_NONE;
static Ewl_Widget *key_name_box = NULL;
static Ewl_Widget *val_box = NULL;
static Ewl_Widget *tree = NULL;

void win_del_cb(Ewl_Widget *w, void *event, void *data);
void open_file_cb(Ewl_Widget *w, void *event, void *data);
void fd_win_del_cb(Ewl_Widget *w, void *event, void *data);
void open_cb(Ewl_Widget *w, void *event, void *data);
void save_cb(Ewl_Widget *w, void *event, void *data);
void add_cb(Ewl_Widget *w, void *event, void *data);
void delete_cb(Ewl_Widget *w, void *event, void *data);
void type_sel_cb(Ewl_Widget *w, void *event, void *data);

int main(int argc, char ** argv) {
    Ewl_Widget *win = NULL, *box = NULL, *o = NULL;
    Ewl_Widget *menu_box = NULL, *hbox = NULL;
    Ewl_Widget *combo = NULL, *box2 = NULL;

    if (!ewl_init(&argc, argv)) {
        printf("Unable to init ewl\n");
        return 0;
    }

    win = ewl_window_new();
    ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, win_del_cb, NULL);
    ewl_window_title_set(EWL_WINDOW(win), "EWL EDB ed");
    ewl_window_class_set(EWL_WINDOW(win), "EWL_EDB_ed");
    ewl_window_name_set(EWL_WINDOW(win), "EWL_EDB_ed");
    ewl_object_size_request(EWL_OBJECT(win), 400, 300);
    ewl_widget_show(win);

    box = ewl_vbox_new();
    ewl_container_child_append(EWL_CONTAINER(win), box);
    ewl_widget_show(box);

        /* box to hold menu */
    menu_box = ewl_hbox_new();
    ewl_container_child_append(EWL_CONTAINER(box), menu_box);
    ewl_object_fill_policy_set(EWL_OBJECT(menu_box),
            EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
    ewl_widget_show(menu_box);

    hbox = ewl_hbox_new();
    ewl_container_child_append(EWL_CONTAINER(box), hbox);
    ewl_widget_show(hbox);

        /* create the tree */
        {
                char * headers [] = {
                                "type", "key", "value"
                };
            tree = ewl_tree_new(3);
            ewl_container_child_append(EWL_CONTAINER(hbox), tree);
                ewl_object_padding_set(EWL_OBJECT(tree), 2, 0, 0, 0);
                ewl_tree_headers_set(EWL_TREE(tree), headers);
            ewl_widget_show(tree);
        }

        box2 = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(hbox), box2);
        ewl_widget_show(box2);

    /* create the combo box */
    combo = ewl_combo_new("data type");
    ewl_container_child_append(EWL_CONTAINER(box2), combo);
        ewl_object_padding_set(EWL_OBJECT(combo), 5, 0, 0, 0);
    ewl_widget_show(combo);

    {
        o = ewl_menu_item_new(NULL, "int");
        ewl_container_child_append(EWL_CONTAINER(combo), o);
                ewl_callback_append(o, EWL_CALLBACK_SELECT, type_sel_cb, (int *)EWL_EDB_TYPE_INT);
        ewl_widget_show(o);

        o = ewl_menu_item_new(NULL, "str");
        ewl_container_child_append(EWL_CONTAINER(combo), o);
                ewl_callback_append(o, EWL_CALLBACK_SELECT, type_sel_cb, (int *)EWL_EDB_TYPE_STR);
        ewl_widget_show(o);

        o = ewl_menu_item_new(NULL, "float");
        ewl_container_child_append(EWL_CONTAINER(combo), o);
                ewl_callback_append(o, EWL_CALLBACK_SELECT, type_sel_cb, (int *)EWL_EDB_TYPE_FLOAT);
        ewl_widget_show(o);

        o = ewl_menu_item_new(NULL, "data");
        ewl_container_child_append(EWL_CONTAINER(combo), o);
                ewl_callback_append(o, EWL_CALLBACK_SELECT, type_sel_cb, (int *)EWL_EDB_TYPE_DATA);
        ewl_widget_show(o);
    }

    /* value entry box */
    val_box = ewl_entry_new("");
    ewl_container_child_append(EWL_CONTAINER(box2), val_box);
        ewl_object_alignment_set(EWL_OBJECT(val_box), EWL_FLAG_ALIGN_CENTER);
    ewl_widget_show(val_box);

        /* add/delete buttons on right */
        {
                Ewl_Widget *add_del_box = ewl_hbox_new();
                ewl_container_child_append(EWL_CONTAINER(box2), add_del_box);
                ewl_widget_show(add_del_box);

                o = ewl_button_new("add");
                ewl_container_child_append(EWL_CONTAINER(add_del_box), o);
                ewl_callback_append(o, EWL_CALLBACK_CLICKED, add_cb, NULL);
                ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
                ewl_object_padding_set(EWL_OBJECT(o), 5, 5, 2, 2);
                ewl_widget_show(o);

                o = ewl_button_new("delete");
                ewl_container_child_append(EWL_CONTAINER(add_del_box), o);
                ewl_callback_append(o, EWL_CALLBACK_CLICKED, delete_cb, NULL);
                ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
                ewl_object_padding_set(EWL_OBJECT(o), 0, 5, 2, 2);
                ewl_widget_show(o);
        }

    /* create the menu */
    {
        Ewl_Widget *file = NULL;

        file = ewl_menu_new(NULL, "file");
        ewl_container_child_append(EWL_CONTAINER(menu_box), file);
        ewl_object_fill_policy_set(EWL_OBJECT(file), EWL_FLAG_FILL_SHRINK);
        ewl_object_padding_set(EWL_OBJECT(file), 2, 2, 2, 0);
        ewl_widget_show(file);

        o = ewl_menu_item_new(NULL, "open");
        ewl_container_child_append(EWL_CONTAINER(file), o);
                ewl_callback_append(o, EWL_CALLBACK_SELECT, open_cb, NULL);
        ewl_widget_show(o);

        o = ewl_menu_item_new(NULL, "save");
        ewl_container_child_append(EWL_CONTAINER(file), o);
                ewl_callback_append(o, EWL_CALLBACK_SELECT, save_cb, NULL);
        ewl_widget_show(o);

        o = ewl_menu_item_new(NULL, "quit");
        ewl_container_child_append(EWL_CONTAINER(file), o);
        ewl_callback_append(o, EWL_CALLBACK_SELECT, win_del_cb, NULL);
        ewl_widget_show(o);
    }

    /* key name entry box */
    key_name_box = ewl_entry_new("");
    ewl_container_child_append(EWL_CONTAINER(box), key_name_box);
    ewl_widget_show(key_name_box);

        o = ewl_hseparator_new();
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        /* save /quit buttons on bottom */
        box2 = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(box), box2);
        ewl_object_fill_policy_set(EWL_OBJECT(box2),
                                                EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
        ewl_object_alignment_set(EWL_OBJECT(box2), EWL_FLAG_ALIGN_RIGHT);
        ewl_widget_show(box2);

        o = ewl_button_stock_new(EWL_STOCK_SAVE);
        ewl_container_child_append(EWL_CONTAINER(box2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, save_cb, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
        ewl_object_padding_set(EWL_OBJECT(o), 5, 5, 2, 2);
        ewl_widget_show(o);

        o = ewl_button_stock_new(EWL_STOCK_QUIT);
        ewl_container_child_append(EWL_CONTAINER(box2), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, win_del_cb, NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINK);
        ewl_object_padding_set(EWL_OBJECT(o), 0, 5, 2, 2);
        ewl_widget_show(o);

        if (argc > 1)
                open_file_cb(NULL, argv[1], NULL);

    ewl_main();
    return 1;
}

void win_del_cb(Ewl_Widget *w, void *event, void *data) {
    ewl_main_quit();

    return;
    w = NULL;
    event = NULL;
    data = NULL;
}

void open_file_cb(Ewl_Widget *w, void *event, void *data) {
        Ewl_Widget *fd_win = (Ewl_Widget *)data;
        char *file = (char *)event;
        char **key_list = NULL;
        int num_ret = 0;

        fd_win_del_cb(NULL, NULL, fd_win);
        if (file == NULL)
                return;

        db_file = e_db_open(file);

        if (db_file == NULL) {
                printf("Unable to open file %s\n", file);
                return;
        }

        key_list = e_db_dump_key_list(file, &num_ret);
        {
                int i = 0;

                for(i = 0; i < num_ret; i++) {
                        if (key_list[i] != NULL) {
                                Ewl_Widget * widgets[3];
                                char *type = e_db_type_get(db_file, key_list[i]);
                                char val[512];

                                if (!strcmp(type, "str")) {
                                        snprintf(val, sizeof(val), "%s", e_db_str_get(db_file, key_list[i]));

                                } else if (!strcmp(type, "float")) {
                                        float f = 0.0;
                                        e_db_float_get(db_file, key_list[i], &f);
                                        snprintf(val, sizeof(val), "%.2f", f);

                                } else if (!strcmp(type, "data")) {
                                        snprintf(val, sizeof(val), "Data not shown");

                                } else if (!strcmp(type, "int")) {
                                        int integer = 0;
                                        e_db_int_get(db_file, key_list[i], &integer);
                                        snprintf(val, sizeof(val), "%d", integer);
                                }

                                widgets[0] = ewl_text_new(type);
                                ewl_widget_show(widgets[0]);

                                widgets[1] = ewl_text_new(key_list[i]);
                                ewl_widget_show(widgets[1]);

                                widgets[2] = ewl_text_new(val);
                                ewl_widget_show(widgets[2]);

                                ewl_tree_row_add(EWL_TREE(tree), NULL, widgets);
                                free(key_list[i]);
                        }
                }
        }

        free(key_list);

        return;
        w = NULL;
        data = NULL;
}

void fd_win_del_cb(Ewl_Widget *w, void *event, void *data) {
        Ewl_Widget *fd_win = (Ewl_Widget *)data;
        ewl_widget_hide(fd_win);
        ewl_widget_destroy(fd_win);

        return;
        w = NULL;
        event = NULL;
        data = NULL;
}

void open_cb(Ewl_Widget *w, void *event, void *data) {
        Ewl_Widget *fd_win = NULL, *fd = NULL;

        fd_win = ewl_window_new();
        ewl_window_title_set(EWL_WINDOW(fd_win), "EWL EDB ed open");
        ewl_window_class_set(EWL_WINDOW(fd_win), "EWL_EDB_ed_open");
        ewl_window_name_set(EWL_WINDOW(fd_win), "EWL_EDB_ed_open");
        ewl_callback_append(fd_win, EWL_CALLBACK_DELETE_WINDOW, fd_win_del_cb, fd_win);
        ewl_widget_show(fd_win);

        fd = ewl_filedialog_new(EWL_FILEDIALOG_TYPE_OPEN);
        ewl_container_child_append(EWL_CONTAINER(fd_win), fd);
        ewl_callback_append(fd, EWL_CALLBACK_VALUE_CHANGED, open_file_cb, fd_win);
        ewl_widget_show(fd);

        return;
        w = NULL;
        event = NULL;
        data = NULL;
}

void save_cb(Ewl_Widget *w, void *event, void *data) {
        if (db_file == NULL)
                return;

        e_db_close(db_file);
        db_file = NULL;
        e_db_flush();

        return;
        w = NULL;
        event = NULL;
        data = NULL;
}

void type_sel_cb(Ewl_Widget *w, void *event, void *data) {
        current_type = (Ewl_Edb_Type)data;

        return;
        w = NULL;
        event = NULL;
}

void add_cb(Ewl_Widget *w, void *event, void *data) {
        char *key = ewl_text_text_get(EWL_TEXT(key_name_box));
        char *val = ewl_text_text_get(EWL_TEXT(val_box));
        Ewl_Widget *widgets[3];

        if ((key == NULL)  || (val == NULL))
                return;

        switch(current_type) {
                case EWL_EDB_TYPE_INT:
                        {
                                int ival = atoi(val);
                                widgets[0] = ewl_text_new("int");
                                ewl_widget_show(widgets[0]);
                                e_db_int_set(db_file, key, ival);
                        }
                        break;

                case EWL_EDB_TYPE_STR:
                        widgets[0] = ewl_text_new("str");
                        ewl_widget_show(widgets[0]);
                        e_db_str_set(db_file, key, val);
                        break;

                case EWL_EDB_TYPE_FLOAT:
                        {
                                float fval = atof(val);
                                widgets[0] = ewl_text_new("float");
                                ewl_widget_show(widgets[0]);
                                e_db_float_set(db_file, key, fval);
                        }
                        break;

                case EWL_EDB_TYPE_DATA:
                        widgets[0] = ewl_text_new("data");
                        ewl_widget_show(widgets[0]);
                        e_db_data_set(db_file, key, val, strlen(val));
                        free(val);
                        val = strdup("");
                        break;

                default:
                        printf("No type selected\n");
                        break;
        }

        widgets[1] = ewl_text_new(key);
        ewl_widget_show(widgets[1]);

        widgets[2] = ewl_text_new(val);
        ewl_widget_show(widgets[2]);

        ewl_tree_row_add(EWL_TREE(tree), NULL, widgets);

        return;
        w = NULL;
        event = NULL;
        data = NULL;
}

void delete_cb(Ewl_Widget *w, void *event, void *data) {
        char *current_key = NULL;
//        char *current_key = ewl_table_get_selected(EWL_TABLE(table));

        if (current_key == NULL)
                return;

        e_db_data_del(db_file, current_key);

        return;
        w = NULL;
        event = NULL;
        data = NULL;
}




