
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

Ewl_Widget     *main_win;
Ewl_Widget     *main_box;

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
  ewl_object_set_minimum_size(EWL_OBJECT(main_win), 200, 100);
  ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW,
                      __destroy_main_window, NULL);

  main_box = ewl_vbox_new();
  ewl_container_append_child(EWL_CONTAINER(main_win), main_box);
  ewl_object_set_padding(EWL_OBJECT(main_box), 2, 2, 2, 2);
  ewl_widget_show(main_box);


reconnect:
  cc++;
  if ((ret = examine_client_init(pipe_name, &cs)) != ECORE_CONFIG_ERR_SUCC)
    E(0, "examine: %sconnect to %s failed: %d\n", (cc > 1) ? "re" : "",
      pipe_name, ret);
  else {
//    ewl_widget_show(main_win);
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

void
render_ewl(void)
{
  Ewl_Widget     *row, *cell[2], *text[2];
  char           *label, *typename, *start, *end;
  char           *prop_list;
  int             tmpi;
  double          tmpd;

  prop_list = examine_client_list_props();
  if (prop_list && (strlen(prop_list) > 0)) {

    row = ewl_grid_new(3, 1);
    cell[0] = ewl_cell_new();
    cell[1] = ewl_cell_new();
    text[0] = ewl_text_new("Property");
    text[1] = ewl_text_new("Value");

    ewl_container_append_child(EWL_CONTAINER(cell[0]), text[0]);
    ewl_container_append_child(EWL_CONTAINER(cell[1]), text[1]);
    ewl_grid_add(EWL_GRID(row), cell[0], 1, 1, 1, 1);
    ewl_grid_add(EWL_GRID(row), cell[1], 2, 3, 1, 1);

    ewl_widget_show(cell[0]);
    ewl_widget_show(cell[1]);
    ewl_widget_show(text[0]);
    ewl_widget_show(text[1]);

    ewl_container_append_child(EWL_CONTAINER(main_box), row);
    ewl_widget_show(row);

    start = prop_list;
    end = prop_list + strlen(prop_list);

    while (start < end) {
      label = start;
      while (*start) {
        if (*start == ':') {
          *start = '\0';
          break;
        }
        start++;
      }

      start++;
      typename = ++start;
      while (*start) {
        if (*start == '\n') {
          *start = '\0';
          break;
        }
        start++;
      }

      if (*label && *typename) {
        row = ewl_grid_new(3, 1);
        cell[0] = ewl_cell_new();
        cell[1] = ewl_cell_new();
        text[0] = ewl_text_new(label);

        if (!strcmp(typename, "string"))
          text[1] = ewl_entry_new(examine_client_get_val(label));
        else if (!strcmp(typename, "integer")) {
          text[1] = ewl_spinner_new();
          ewl_spinner_set_digits(EWL_SPINNER(text[1]), 0);
          ewl_spinner_set_step(EWL_SPINNER(text[1]), 1);
          sscanf(examine_client_get_val(label), "%d", &tmpi);
          ewl_spinner_set_value(EWL_SPINNER(text[1]), tmpi);
        } else if (!strcmp(typename, "float")) {
          text[1] = ewl_spinner_new();
//          ewl_spinner_set_digits(EWL_SPINNER(text[1]), 0);
//          ewl_spinner_set_step(EWL_SPINNER(text[1]), 1);
          sscanf(examine_client_get_val(label), "%lf", &tmpd);
          ewl_spinner_set_value(EWL_SPINNER(text[1]), tmpd);
        } else if (!strcmp(typename, "colour"))
          text[1] = ewl_entry_new(examine_client_get_val(label));
        else
          text[1] = ewl_text_new(typename);

        ewl_container_append_child(EWL_CONTAINER(cell[0]), text[0]);
        ewl_container_append_child(EWL_CONTAINER(cell[1]), text[1]);
        ewl_grid_add(EWL_GRID(row), cell[0], 1, 1, 1, 1);
        ewl_grid_add(EWL_GRID(row), cell[1], 2, 3, 1, 1);

        ewl_widget_show(cell[0]);
        ewl_widget_show(cell[1]);
        ewl_widget_show(text[0]);
        ewl_widget_show(text[1]);

        ewl_container_append_child(EWL_CONTAINER(main_box), row);
        ewl_object_set_minimum_h(EWL_OBJECT(row), 22);
        ewl_widget_show(row);
      }

      start++;
    }
  }
  free(prop_list);
}

/*****************************************************************************/
