
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
void            draw_tree(void);
void            print_usage(void);

Ewl_Widget     *main_win;
Ewl_Widget     *tree_box;

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
  ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, ex_ipc_sigexit, &cs);

  ewl_init(&argc, argv);

  main_win = ewl_window_new();
  ewl_window_set_title(EWL_WINDOW(main_win), "Examine Configuration Client");
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
  draw_tree();
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
draw_tree(void)
{
  examine_prop   *prop_item;
  Ewl_Widget     *entries[2];

  ewl_container_reset(EWL_CONTAINER(tree_box));
  prop_item = examine_client_list_props();
  while (prop_item) {
    entries[0] = ewl_text_new(prop_item->key);

    if (prop_item->type == PT_STR) {
      entries[1] = ewl_entry_new(prop_item->value.ptr);
      ewl_callback_append(EWL_ENTRY(entries[1])->text, EWL_CALLBACK_VALUE_CHANGED,
                          cb_set_str, prop_item);
    } else if (prop_item->type == PT_INT) {
      entries[1] = ewl_spinner_new();

      ewl_spinner_set_digits(EWL_SPINNER(entries[1]), 0);
      ewl_spinner_set_step(EWL_SPINNER(entries[1]), 1);
      ewl_spinner_set_value(EWL_SPINNER(entries[1]), prop_item->value.val);
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
      ewl_spinner_set_value(EWL_SPINNER(entries[1]), prop_item->value.fval);
      if (prop_item->bound & BOUND_BOUND) {
        ewl_spinner_set_min_val(EWL_SPINNER(entries[1]), prop_item->fmin);
        ewl_spinner_set_max_val(EWL_SPINNER(entries[1]), prop_item->fmax);
      }
      if (prop_item->bound & BOUND_STEPPED)
        ewl_spinner_set_step(EWL_SPINNER(entries[1]), prop_item->fstep);
      ewl_callback_append(entries[1], EWL_CALLBACK_VALUE_CHANGED, cb_set_float,
                          prop_item);
    } else if (prop_item->type == PT_RGB) {
      entries[1] = ewl_entry_new(prop_item->value.ptr);
      ewl_callback_append(EWL_ENTRY(entries[1])->text, EWL_CALLBACK_VALUE_CHANGED,
                          cb_set_str, prop_item);
    } else
      entries[1] = ewl_text_new("unknown");

    ewl_widget_show(entries[0]);
    ewl_widget_show(entries[1]);
    /* padding to fix ewl layout issue ##### FIXME */
    ewl_object_set_padding(EWL_OBJECT(entries[1]), 0, 11, 0, 0);
    ewl_tree_add_row(EWL_TREE(tree_box), 0, entries);

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

  tree_box = ewl_tree_new(2);
  ewl_container_append_child(EWL_CONTAINER(main_box), tree_box);
  ewl_widget_show(tree_box);
  headers[0] = strdup("Setting");
  headers[1] = strdup("Value");
  ewl_tree_set_headers((Ewl_Tree *) tree_box, headers);
  free(headers[0]);
  free(headers[1]);

  draw_tree();

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
