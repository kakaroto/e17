
/* by Azundris */

/* Modified for ecore_config by HandyAndE */

#include "Ecore_Config.h"

#include <Ewl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "examine_client.h"

int             debug = 1;

void            render_ewl(void);
void            print_usage(void);
Ewl_Widget     *add_tab(char *name);

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

char           *examine_separators = ".\\/\0";

/*****************************************************************************/

void
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
  char           *pipe_name = NULL;

  if (argc <= 1 ||
      (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
    print_usage();
    return 0;
  }

  pipe_name = argv[1];
  E(2, "examine: connecting to %s.\n", pipe_name);

  ecore_init();
  ecore_app_args_set(argc, (const char **) argv);
  ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, ecore_config_ipc_sigexit,
                          &cs);

  ewl_init(&argc, argv);

  main_win = ewl_window_new();
  ewl_window_set_title(EWL_WINDOW(main_win), "Examine Configuration Client");
  ewl_window_set_name(EWL_WINDOW(main_win), "Examine");
  ewl_window_set_class(EWL_WINDOW(main_win), "examine");
  ewl_object_request_size(EWL_OBJECT(main_win), 200, 250);
  ewl_object_set_fill_policy((Ewl_Object *) main_win, EWL_FLAG_FILL_FILL);
  ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW,
                      __destroy_main_window, NULL);

reconnect:
  cc++;
  if ((ret = examine_client_init(pipe_name, &cs)) != ECORE_CONFIG_ERR_SUCC)
    E(0, "examine: %sconnect to %s failed: %d\n", (cc > 1) ? "re" : "",
      pipe_name, ret);
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

void
cb_quit(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_client_exit();
  ewl_main_quit();
  ewl_widget_destroy(main_win);
  /* ewl_shutdown(); ### segs */
}

void
cb_save(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_client_save_list();
  /* sets all props where oldvalue != value */
}

void
cb_revert(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_client_revert_list();
}

void
cb_set_str(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_prop   *change;
  char           *data;

  change = (examine_prop *) user_data;
  data = (char *) ev_data;
  free(change->value.ptr);
  change->value.ptr = strdup(data);
}

void
cb_set_int(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_prop   *change;

  change = (examine_prop *) user_data;
  change->value.val = (int) ewl_spinner_get_value(EWL_SPINNER(w));
}

void
cb_set_float(Ewl_Widget * w, void *ev_data, void *user_data)
{
  examine_prop   *change;

  change = (examine_prop *) user_data;
  change->value.fval = (float) ewl_spinner_get_value(EWL_SPINNER(w));
}

/* UI constructor */

void
draw_tree(examine_prop * prop_item)
{
  Ewl_Widget     *entries[2], *tree_box, *tmp_row;
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

  printf("key is %s\n", prop_item->key);
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
      entries[0] = ewl_text_new(panel_tmp + 1);
    } else {
      tree_box = add_tab("Misc");
      entries[0] = ewl_text_new(panel_name);
    }

    free(key_tmp);
    if (prop_item->type == PT_STR) {
      entries[1] = ewl_entry_new("");
      ewl_callback_append(EWL_ENTRY(entries[1])->text,
                          EWL_CALLBACK_VALUE_CHANGED, cb_set_str, prop_item);
    } else if (prop_item->type == PT_INT) {
      entries[1] = ewl_spinner_new();

      ewl_spinner_set_digits(EWL_SPINNER(entries[1]), 0);
      ewl_spinner_set_step(EWL_SPINNER(entries[1]), 1);
      if (prop_item->bound & BOUND_BOUND) {
        ewl_spinner_set_min_val(EWL_SPINNER(entries[1]), prop_item->min);
        ewl_spinner_set_max_val(EWL_SPINNER(entries[1]), prop_item->max);
      }
      if (prop_item->bound & BOUND_STEPPED)
        ewl_spinner_set_step(EWL_SPINNER(entries[1]), prop_item->step);
      ewl_callback_append(entries[1], EWL_CALLBACK_VALUE_CHANGED, cb_set_int,
                          prop_item);
    } else if (prop_item->type == PT_FLT) {
      entries[1] = ewl_spinner_new();

/*          ewl_spinner_set_digits(EWL_SPINNER(input), 0);
            ewl_spinner_set_step(EWL_SPINNER(input), 1);*/
      if (prop_item->bound & BOUND_BOUND) {
        ewl_spinner_set_min_val(EWL_SPINNER(entries[1]), prop_item->fmin);
        ewl_spinner_set_max_val(EWL_SPINNER(entries[1]), prop_item->fmax);
      }
      if (prop_item->bound & BOUND_STEPPED)
        ewl_spinner_set_step(EWL_SPINNER(entries[1]), prop_item->fstep);
      ewl_callback_append(entries[1], EWL_CALLBACK_VALUE_CHANGED, cb_set_float,
                          prop_item);
    } else if (prop_item->type == PT_RGB) {
      entries[1] = ewl_entry_new("");
      ewl_callback_append(EWL_ENTRY(entries[1])->text,
                          EWL_CALLBACK_VALUE_CHANGED, cb_set_str, prop_item);
    } else
      entries[1] = ewl_entry_new("unknown");
    prop_item->w = entries[1];
    examine_client_get_val(prop_item->key);

    ewl_widget_show(entries[0]);
    ewl_widget_show(entries[1]);

    tmp_row = ewl_hbox_new();
    ewl_widget_show(tmp_row);
    ewl_container_append_child(EWL_CONTAINER(tmp_row), entries[0]);
    ewl_container_append_child(EWL_CONTAINER(tmp_row), entries[1]);
    ewl_container_append_child(EWL_CONTAINER(tree_box), tmp_row);
    prop_item = prop_item->next;
  }
}

void
render_ewl(void)
{
  Ewl_Widget     *main_box, *row, *cell[2], *text[2];
  Ewl_Widget     *save, *revert, *quit;
  char           *headers[2];

  main_box = ewl_vbox_new();
  ewl_container_append_child(EWL_CONTAINER(main_win), main_box);
  ewl_object_set_padding(EWL_OBJECT(main_box), 2, 2, 2, 2);
  ewl_widget_show(main_box);

  notebook = ewl_notebook_new();
  ewl_notebook_set_tabs_position(EWL_NOTEBOOK(notebook), EWL_POSITION_TOP);
  ewl_notebook_set_tabs_alignment(EWL_NOTEBOOK(notebook), EWL_FLAG_ALIGN_LEFT);
  ewl_container_append_child(EWL_CONTAINER(main_box), notebook);
  ewl_widget_show(notebook);

  examine_client_list_props();

  row = ewl_hbox_new();
  ewl_container_append_child(EWL_CONTAINER(main_box), row);
  ewl_object_set_fill_policy((Ewl_Object *) row, EWL_FLAG_FILL_HFILL);
  ewl_widget_show(row);

  save = ewl_button_new("Save");
  ewl_callback_append(save, EWL_CALLBACK_MOUSE_DOWN, cb_save, NULL);
  revert = ewl_button_new("Revert");
  ewl_callback_append(revert, EWL_CALLBACK_MOUSE_DOWN, cb_revert, NULL);
  quit = ewl_button_new("Close");
  ewl_callback_append(quit, EWL_CALLBACK_MOUSE_DOWN, cb_quit, NULL);

  ewl_container_append_child(EWL_CONTAINER(row), save);
  ewl_container_append_child(EWL_CONTAINER(row), revert);
  ewl_container_append_child(EWL_CONTAINER(row), quit);
  ewl_widget_show(save);
  ewl_widget_show(revert);
  ewl_widget_show(quit);
}

/*****************************************************************************/

Ewl_Widget * add_tab(char *name)
{
  Ewl_Widget     *button, *scrollpane, *pane, *headers[2];
  examine_panel  *new_panel;

  new_panel = panels;
  while (new_panel) {
    if (strcmp(new_panel->name, name) == 0)
      return new_panel->container;
    new_panel = new_panel->next;
  }

  button = ewl_text_new(name);
  ewl_widget_show(button);

//  pane = ewl_scrollpane_new(); FIXME: ewl scrollpane does not allow
//  additions after realisation
  pane = ewl_vbox_new();
  ewl_object_set_alignment(EWL_OBJECT(pane), EWL_FLAG_ALIGN_TOP);
  ewl_widget_show(pane);

  ewl_notebook_prepend_page(EWL_NOTEBOOK(notebook), button, pane);

  new_panel = malloc(sizeof(examine_panel));
  new_panel->name = strdup(name);
  new_panel->container = pane;
  new_panel->next = panels;
  panels = new_panel;

  return pane;
}
