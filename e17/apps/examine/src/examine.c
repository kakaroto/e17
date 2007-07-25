
/* by Azundris */

/* Modified for ecore_config by HandyAndE */

#include "Ecore_Config.h"
#include <Ecore_Data.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "examine_client.h"

int             debug = 1;

void            render_ewl(void);
void            print_usage(void);
Ewl_Widget     *add_tab(char *name);
char           *app_name;

Ewl_Widget     *main_win;
Ewl_Widget     *tree_box;

typedef struct examine_panel examine_panel;
struct examine_panel {
  char           *name;
  Ewl_Widget     *container;
  examine_panel  *next;
};

Ewl_Widget     *notebook;
examine_panel  *panels;

char           *examine_separators = ".\\/-\0";
char           *examine_search_path;

/*****************************************************************************/

static void
__destroy_main_window(Ewl_Widget * main_win, void *ex_data, void *user_data)
{
  ewl_widget_destroy(main_win);
  ewl_main_quit();

  return;
}


int
main(int argc, char **argv)
{
  int             ret = ECORE_CONFIG_ERR_SUCC, cc = 0;
  connstate       cs = OFFLINE;

  if (argc <= 1 ||
      (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
    print_usage();
    return 0;
  }

  app_name = argv[1];
  E(2, "examine: connecting to %s.\n", app_name);

  ecore_init();
  ecore_app_args_set(argc, (const char **) argv);
  ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, ecore_config_ipc_sigexit,
                          &cs);

  ewl_init(&argc, argv);

  main_win = ewl_window_new();
  ewl_window_title_set(EWL_WINDOW(main_win), "Examine Configuration Client");
  ewl_window_name_set(EWL_WINDOW(main_win), "Examine");
  ewl_window_class_set(EWL_WINDOW(main_win), "examine");
  ewl_object_size_request(EWL_OBJECT(main_win), 250, 320);
  ewl_object_fill_policy_set((Ewl_Object *) main_win, EWL_FLAG_FILL_FILL);
  ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW,
                      __destroy_main_window, NULL);

  cc++;
  if ((ret = examine_client_init(app_name, &cs)) != ECORE_CONFIG_ERR_SUCC)
    E(0, "examine: %sconnect to %s failed: %d\n", (cc > 1) ? "re" : "",
      app_name, ret);
  else {
    render_ewl();
    ewl_widget_show(main_win);
    ewl_main();

  }

  examine_client_exit();
  ecore_shutdown();

  return ret;
}

void
print_usage(void)
{
  printf("Examine - ecore_config Graphical Configuration Client\n");
  printf("Version 0.0.1 (Nov 3 2003)\n");
  printf("(c)2003 by HandyAndE.\n");
  printf("Based on work in evidence by Azundris.\n");
  printf("Usage: examine [options] target\n\n");
  printf("Supported Options:\n");
  printf("-h, --help        Print this help text\n");

}


/*  callbacks */

static void
cb_quit(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_client_exit();
  ewl_main_quit();
  ewl_widget_destroy(main_win);
  /* ewl_shutdown(); ### segs */
}

static void
cb_save(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_client_save_list();
  /* sets all props where oldvalue != value */
}

static void
cb_revert(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_client_revert_list();
}

static void
cb_set_str(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_prop   *change;
  char           *data;

  change = (examine_prop *) user_data;
  data = ewl_text_text_get(EWL_TEXT(w));
  free(change->value.ptr);
  change->value.ptr = strdup(data);
}

static void
cb_set_int(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_prop   *change;

  change = (examine_prop *) user_data;
  change->value.val = (int) ewl_range_value_get(EWL_RANGE(w));
}

static void
cb_set_float(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_prop   *change;

  change = (examine_prop *) user_data;
  change->value.fval = (float) ewl_range_value_get(EWL_RANGE(w));
}

static void
cb_choose_theme(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_prop   *change;
  char           *theme, *bugfix;
  Ewl_Widget     *sibling;

  change = (examine_prop *) user_data;

  theme = strdup(ewl_text_text_get(EWL_TEXT(EWL_CONTAINER(w)->redirect)));

  ewl_container_child_iterate_begin(EWL_CONTAINER(w->parent));
  while ((sibling = ewl_container_child_next(EWL_CONTAINER(w->parent)))) {
    sibling = EWL_WIDGET(EWL_CONTAINER(sibling)->redirect);
    bugfix = ewl_text_text_get(EWL_TEXT(sibling));
    if (strcmp(bugfix, theme))
      ewl_text_color_set(EWL_TEXT(sibling), 0, 0, 0, 0xFF);
    else
      ewl_text_color_set(EWL_TEXT(sibling), 0xFF, 0, 0, 0xFF);
    ewl_text_text_set(EWL_TEXT(sibling), bugfix);
  }

  if (change->value.ptr)
    free(change->value.ptr);
  change->value.ptr = theme;
}

static void
cb_set_bln(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_prop   *change;

  change = (examine_prop *) user_data;
  change->value.val = ewl_checkbutton_is_checked(EWL_CHECKBUTTON(w));
}

/* UI constructor */

void
draw_tree(examine_prop * prop_item)
{
  Ewl_Widget     *entries[2], *tree_box, *tmp_row, *tmp_col, *tmp_text;
  examine_panel  *panel_ptr;
  char           *key_tmp;
  char           *panel_name;
  char           *sep_tmp;
  char           *panel_tmp;
  int             found;

  panel_ptr = panels;
  while (panel_ptr) {
    ewl_container_reset(EWL_CONTAINER(panel_ptr->container));
    panel_ptr = panel_ptr->next;
  }

  while (prop_item) {
    key_tmp = strdup(prop_item->key);
    panel_name = key_tmp;
    sep_tmp = examine_separators;
    while (*sep_tmp) {
      if (*key_tmp == *sep_tmp) {
        panel_name++;
        break;
      }
      sep_tmp++;
    }

    found = 0;
    panel_tmp = panel_name;
    while (*panel_tmp) {
      sep_tmp = examine_separators;
      while (*sep_tmp) {
        if (*panel_tmp == *sep_tmp) {
          *panel_tmp = '\0';
          found = 1;
          break;
        }
        sep_tmp++;
      }
      if (found)
        break;
      panel_tmp++;
    }

    if (found) {
      tree_box = add_tab(panel_name);
      entries[0] = ewl_text_new();
      ewl_text_text_set(EWL_TEXT(entries[0]), panel_tmp + 1);
    } else {
      tree_box = add_tab("Misc");
      entries[0] = ewl_text_new();
       ewl_text_text_set(EWL_TEXT(entries[0]), panel_name);
    }

    free(key_tmp);
    if (prop_item->type == ECORE_CONFIG_STR) {
      entries[1] = ewl_entry_new();
      ewl_callback_append(entries[1], EWL_CALLBACK_FOCUS_OUT, cb_set_str,
                          prop_item);
    } else if (prop_item->type == ECORE_CONFIG_INT) {
      entries[1] = ewl_spinner_new();

      ewl_spinner_digits_set(EWL_SPINNER(entries[1]), 0);
      ewl_range_step_set(EWL_RANGE(entries[1]), 1);
      if (prop_item->bound & BOUND_BOUND) {
        ewl_range_minimum_value_set(EWL_RANGE(entries[1]), prop_item->min);
        ewl_range_maximum_value_set(EWL_RANGE(entries[1]), prop_item->max);
      } else {
        ewl_range_minimum_value_set(EWL_RANGE(entries[1]), -99999);
        ewl_range_maximum_value_set(EWL_RANGE(entries[1]), 99999);
      }
      if (prop_item->bound & BOUND_STEPPED)
        ewl_range_step_set(EWL_RANGE(entries[1]), prop_item->step);
      ewl_callback_append(entries[1], EWL_CALLBACK_VALUE_CHANGED, cb_set_int,
                          prop_item);
    } else if (prop_item->type == ECORE_CONFIG_FLT) {
      entries[1] = ewl_spinner_new();

/*          ewl_spinner_digits_set(EWL_SPINNER(input), 0);
            ewl_range_step_set(EWL_RANGE(input), 1);*/
      if (prop_item->bound & BOUND_BOUND) {
        ewl_range_minimum_value_set(EWL_RANGE(entries[1]), prop_item->fmin);
        ewl_range_maximum_value_set(EWL_RANGE(entries[1]), prop_item->fmax);
      }
      if (prop_item->bound & BOUND_STEPPED)
        ewl_range_step_set(EWL_RANGE(entries[1]), prop_item->fstep);
      ewl_callback_append(entries[1], EWL_CALLBACK_VALUE_CHANGED, cb_set_float,
                          prop_item);
    } else if (prop_item->type == ECORE_CONFIG_RGB) {
      entries[1] = ewl_entry_new();
      ewl_callback_append(entries[1], EWL_CALLBACK_FOCUS_OUT, cb_set_str,
                          prop_item);
				
    } else if (prop_item->type == ECORE_CONFIG_THM) {
      struct stat     st;
      struct dirent  *next;
      DIR            *dp;
      char           *search_path, *path, *ptr, *end;
      char           *file;
      int             file_len;
      Ecore_List     *themes;
      int             theme_seen;
      char           *theme_item;
      Ewl_Object     *tmp;

      entries[1] = ewl_hbox_new();
      themes = ecore_list_new();
      ewl_object_fill_policy_set(EWL_OBJECT(entries[1]), EWL_FLAG_FILL_ALL |
                                                         EWL_FLAG_FILL_SHRINK);
      
      search_path = strdup(__examine_client_theme_search_path);
      ptr = search_path;
      end = search_path + strlen(search_path);
      path = search_path;
      while (ptr && ptr < end) {
        while (*ptr != '|' && ptr < end)
          ptr++;
        if (ptr < end)
          *ptr = '\0';

        if (stat(path, &st) != 0) {
          ptr++;
          path = ptr;
          continue;
        }
        dp = opendir((const char *) path);
        while ((next = readdir(dp))) {
          if (!strcmp(next->d_name, ".") || !strcmp(next->d_name, "..")
              || strlen(next->d_name) <= 4 || !strstr(next->d_name, ".edj"))
            continue;

          theme_seen = 0;
          ecore_list_first_goto(themes);
          while((theme_item = (char*)ecore_list_next(themes)) != NULL) {
            if (!strcmp(theme_item, next->d_name)) {
              theme_seen = 1;
              break;
            }
          }

          if (!theme_seen) {
            ecore_list_append(themes, next->d_name);
            
            file = malloc(strlen(path) + strlen(next->d_name) + 2); /* 2=/+\0 */
            strcpy(file, path);
            strcat(file, "/");
            strcat(file, next->d_name);

            tmp = EWL_OBJECT(ewl_image_new());
            ewl_image_file_set(EWL_IMAGE(tmp), file, (char *) prop_item->data);
            ewl_object_minimum_size_set(EWL_OBJECT(tmp), 60, 60);
            ewl_object_fill_policy_set(EWL_OBJECT(tmp), EWL_FLAG_FILL_NONE);
            ewl_object_alignment_set(EWL_OBJECT(tmp), EWL_FLAG_ALIGN_CENTER);
            ewl_widget_show(EWL_WIDGET(tmp));
            free(file);

            file_len = strlen(next->d_name) - 4; /* 4 = .edj*/
            file = malloc(file_len + 1);
            strncpy(file, next->d_name, file_len);
            *(file + file_len) = '\0';
            tmp_text = ewl_text_new();
            ewl_text_text_set(EWL_TEXT(tmp_text), file);
            ewl_object_alignment_set(EWL_OBJECT(tmp_text),
                                     EWL_FLAG_ALIGN_CENTER);

            ewl_widget_show(tmp_text);
            free(file);

            tmp_col = ewl_vbox_new();
            ewl_widget_show(tmp_col);

            ewl_container_child_append(EWL_CONTAINER(tmp_col), EWL_WIDGET(tmp));
            ewl_container_child_append(EWL_CONTAINER(tmp_col), tmp_text);
            ewl_container_child_append(EWL_CONTAINER(entries[1]), tmp_col);
            ewl_callback_append(tmp_col, EWL_CALLBACK_CLICKED, cb_choose_theme,
                                prop_item);
          
            ewl_container_redirect_set(EWL_CONTAINER(tmp_col), 
                                       EWL_CONTAINER(tmp_text));
            ewl_object_padding_set(EWL_OBJECT(tmp_col), 2, 2, 0, 0);
            ewl_object_minimum_h_set(EWL_OBJECT(tmp_col), 60);
          }
        }
        ptr++;
        path = ptr;
      }

      free(search_path);
      ecore_list_destroy(themes);
    } else if (prop_item->type == ECORE_CONFIG_BLN) {
      entries[1] = ewl_checkbutton_new();
      ewl_callback_append(entries[1],
                          EWL_CALLBACK_VALUE_CHANGED, cb_set_bln, prop_item);

    } else if (prop_item->type == ECORE_CONFIG_SCT) {
      entries[1] = ewl_label_new();
      ewl_label_text_set(EWL_LABEL(entries[1]), "structure - MAKE ME A BOX"); /* FIXME - make structures borders, not  lines in the row */
    } else if (prop_item->type == ECORE_CONFIG_NIL) {
      entries[1] = ewl_label_new();
      ewl_label_text_set(EWL_LABEL(entries[1]), "undefined");
    } else
      entries[1] = ewl_entry_new();
    prop_item->w = entries[1];
    examine_client_get_val(prop_item->key);

    ewl_widget_show(entries[0]);
    ewl_widget_show(entries[1]);

    tmp_row = ewl_hbox_new();
    ewl_widget_show(tmp_row);
    ewl_container_child_append(EWL_CONTAINER(tmp_row), entries[0]);
    ewl_container_child_append(EWL_CONTAINER(tmp_row), entries[1]);
    ewl_container_child_append(EWL_CONTAINER(tree_box), tmp_row);
    prop_item = prop_item->next;
  }
}

void
render_ewl(void)
{
  Ewl_Widget     *main_box, *row;
  Ewl_Widget     *save, *revert, *quit;

  main_box = ewl_vbox_new();
  ewl_container_child_append(EWL_CONTAINER(main_win), main_box);
  ewl_object_padding_set(EWL_OBJECT(main_box), 2, 2, 2, 2);
  ewl_widget_show(main_box);

  notebook = ewl_notebook_new();
  ewl_notebook_tabbar_alignment_set(EWL_NOTEBOOK(notebook), EWL_FLAG_ALIGN_LEFT);
  ewl_object_fill_policy_set(EWL_OBJECT(notebook), EWL_FLAG_FILL_ALL);
  ewl_container_child_append(EWL_CONTAINER(main_box), notebook);
  ewl_widget_show(notebook);

  examine_client_theme_search_path_get();

  row = ewl_hbox_new();
  ewl_container_child_append(EWL_CONTAINER(main_box), row);
  ewl_object_fill_policy_set((Ewl_Object *) row, EWL_FLAG_FILL_HFILL);
  ewl_widget_show(row);

  save = ewl_button_new();
  ewl_button_label_set(EWL_BUTTON(save), "Apply");
  ewl_callback_append(save, EWL_CALLBACK_MOUSE_DOWN, cb_save, NULL);
  revert = ewl_button_new();
  ewl_button_label_set(EWL_BUTTON(revert), "Revert");
  ewl_callback_append(revert, EWL_CALLBACK_MOUSE_DOWN, cb_revert, NULL);
  quit = ewl_button_new();
  ewl_button_label_set(EWL_BUTTON(quit), "Close");
  ewl_callback_append(quit, EWL_CALLBACK_MOUSE_DOWN, cb_quit, NULL);

  ewl_container_child_append(EWL_CONTAINER(row), save);
  ewl_container_child_append(EWL_CONTAINER(row), revert);
  ewl_container_child_append(EWL_CONTAINER(row), quit);
  ewl_widget_show(save);
  ewl_widget_show(revert);
  ewl_widget_show(quit);
}

/*****************************************************************************/

Ewl_Widget     *
add_tab(char *name)
{
  Ewl_Widget     *scrollpane, *pane;
  examine_panel  *new_panel;

  new_panel = panels;
  while (new_panel) {
    if (strcmp(new_panel->name, name) == 0)
      return new_panel->container;
    new_panel = new_panel->next;
  }

  scrollpane = ewl_scrollpane_new();
  ewl_container_child_append(EWL_CONTAINER(notebook), scrollpane);
  ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(notebook), scrollpane, name);
  ewl_object_alignment_set(EWL_OBJECT(scrollpane), EWL_FLAG_ALIGN_TOP);
  ewl_widget_show(scrollpane);

  pane = ewl_vbox_new();
  ewl_object_alignment_set(EWL_OBJECT(pane), EWL_FLAG_ALIGN_TOP);
  ewl_object_fill_policy_set(EWL_OBJECT(pane), EWL_FLAG_FILL_VSHRINK |
                                               EWL_FLAG_FILL_HFILL);
  ewl_widget_show(pane);

  ewl_container_child_append(EWL_CONTAINER(scrollpane), pane);

  new_panel = malloc(sizeof(examine_panel));
  new_panel->name = strdup(name);
  new_panel->container = pane;
  new_panel->next = panels;
  panels = new_panel;

  return pane;
}
