#include "Equate.h"

Ewl_Widget     *main_win;
Ewl_Widget     *main_box;
Ewl_Widget     *display;

char            disp[BUFLEN];

typedef struct equate_button {
   int             row;
   int             col;
   int             width;
   int             height;
   char           *text;
   char           *cmd;
   void           *callback;
   Ewl_Widget     *button;
} equate_button;

void            calc_append(Ewl_Widget * w, void *ev_data, void *user_data);
void            calc_clear(Ewl_Widget * w, void *ev_data, void *user_data);
void            calc_op(Ewl_Widget * w, void *ev_data, void *user_data);
void            calc_exec(void);

static int      basic_width = 98;
static int      basic_height = 138;
static int      basic_cols = 4;
static int      basic_rows = 6;
static equate_button basic_buttons[] = {
   {2, 1, 1, 1, "c", "c", (void *) calc_clear, NULL},
   {2, 2, 1, 1, "/", "/", (void *) calc_append, NULL},
   {2, 3, 1, 1, "*", "*", (void *) calc_append, NULL},
   {2, 4, 1, 1, "-", "-", (void *) calc_append, NULL},
   {3, 1, 1, 1, "7", "7", (void *) calc_append, NULL},
   {3, 2, 1, 1, "8", "8", (void *) calc_append, NULL},
   {3, 3, 1, 1, "9", "9", (void *) calc_append, NULL},
   {3, 4, 2, 1, "+", "+", (void *) calc_append, NULL},
   {4, 1, 1, 1, "4", "4", (void *) calc_append, NULL},
   {4, 2, 1, 1, "5", "5", (void *) calc_append, NULL},
   {4, 3, 1, 1, "6", "6", (void *) calc_append, NULL},

   {5, 1, 1, 1, "1", "1", (void *) calc_append, NULL},
   {5, 2, 1, 1, "2", "2", (void *) calc_append, NULL},
   {5, 3, 1, 1, "3", "3", (void *) calc_append, NULL},
   {5, 4, 2, 1, "=", "=", (void *) calc_exec, NULL},
//  {6, 1, 1, 1, "c", "c", (void *) calc_clear, NULL},
   {6, 1, 1, 2, "0", "0", (void *) calc_append, NULL},
   {6, 3, 1, 1, ".", ".", (void *) calc_append, NULL},
};

static int      sci_width = 130;
static int      sci_height = 175;
static int      sci_cols = 4;
static int      sci_rows = 7;
static equate_button sci_buttons[] = {
   {2, 1, 1, 1, "^", "^", (void *) calc_append, NULL},
   {2, 2, 1, 1, "sin", "sin(", (void *) calc_append, NULL},
   {2, 3, 1, 1, "cos", "cos(", (void *) calc_append, NULL},
   {2, 4, 1, 1, "tan", "tan(", (void *) calc_append, NULL},
   {3, 1, 1, 1, "/", "/", (void *) calc_append, NULL},
   {3, 2, 1, 1, "*", "*", (void *) calc_append, NULL},
   {3, 3, 1, 1, "-", "-", (void *) calc_append, NULL},
   {3, 4, 1, 1, "+", "+", (void *) calc_append, NULL},
   {4, 1, 1, 1, "7", "7", (void *) calc_append, NULL},
   {4, 2, 1, 1, "8", "8", (void *) calc_append, NULL},
   {4, 3, 1, 1, "9", "9", (void *) calc_append, NULL},
   {4, 4, 1, 1, "(", "(", (void *) calc_append, NULL},
   {5, 1, 1, 1, "4", "4", (void *) calc_append, NULL},
   {5, 2, 1, 1, "5", "5", (void *) calc_append, NULL},
   {5, 3, 1, 1, "6", "6", (void *) calc_append, NULL},
   {5, 4, 1, 1, ")", ")", (void *) calc_append, NULL},
   {6, 1, 1, 1, "1", "1", (void *) calc_append, NULL},
   {6, 2, 1, 1, "2", "2", (void *) calc_append, NULL},
   {6, 3, 1, 1, "3", "3", (void *) calc_append, NULL},
   {6, 4, 2, 1, "=", "=", (void *) calc_exec, NULL},
   {7, 1, 1, 1, "c", "c", (void *) calc_clear, NULL},
   {7, 2, 1, 1, "0", "0", (void *) calc_append, NULL},
   {7, 3, 1, 1, ".", ".", (void *) calc_append, NULL},
};

static equate_button *buttons;

void
update_display(char *text)
{
   ewl_text_set_text((Ewl_Text *) display, text);
}

void
calc_append(Ewl_Widget * w, void *ev_data, void *user_data)
{
   char           *key;
   int             len, slen;

   key = (char *) user_data;
   slen = strlen(key);

   equate_append(key);

   len = strlen(disp);
   memcpy(&disp[len], key, slen);
   disp[len + slen] = '\0';
   update_display(disp);
}

void
calc_exec(void)
{
   char            res[BUFLEN];

   snprintf(res, BUFLEN, "%.10g", equate_eval());
   update_display(res);
   disp[0] = '\0';
}

void
calc_clear(Ewl_Widget * w, void *ev_data, void *user_data)
{
   equate_clear();
   update_display("0");
   disp[0] = '\0';
}

void
destroy_main_window(Ewl_Widget * main_win, void *ev_data, void *user_data)
{
   ewl_widget_destroy(main_win);
   equate_quit();
   return;
}

void
key_press(Ewl_Widget * w, void *ev_data, void *user_data)
{
   Ecore_X_Event_Key_Down *ev;

   ev = ev_data;

   if (ev->key_compose && !strcmp(ev->key_compose, "q"))
      equate_quit();
   else
      do_key(ev_data, EWL_CALLBACK_MOUSE_DOWN);
}

void
key_un_press(Ewl_Widget * w, void *ev_data, void *user_data)
{
   do_key(ev_data, EWL_CALLBACK_MOUSE_UP);
}

int
do_key(void *ev_data, int action)
{
   Ecore_X_Event_Key_Down *ev;

   ev = ev_data;

   if (ev->key_compose)
      E(6, "Key pressed: %s\n", ev->key_compose);

   int             bc;

   if (buttons == sci_buttons)
      bc = sizeof(sci_buttons);
   else
      bc = sizeof(basic_buttons);
   bc /= sizeof(equate_button);

   equate_button  *but = buttons;

   while (bc-- > 0) {
      if (ev->key_compose)
         if (!strcmp(ev->key_compose, but->cmd)) {
            E(4, "Pressing button %s\n", but->text);

            ewl_callback_call(but->button, action);

            break;
         }
      but++;
   }

   return 0;
}

void
init_gui(Equate * equate, int argc, char **argv)
{
   if (equate) {
      switch (equate->conf.type) {
      case EDJE:
         //fprintf(stderr, "%s\n", equate->conf.path);
         if (ecore_init()) {
            ecore_app_args_set(argc, (const char **) argv);
            equate_edje_init(equate);
            ecore_shutdown();
         }
         break;
         /*
          * case DEF:
          * case EWL:
          */
      default:
         ewl_init(&argc, argv);
         draw_ewl(equate->conf.mode);
         break;
      }
   }
}

void
draw_ewl(Mode draw_mode)
{
   int             count;
   int             width, height, cols, rows;

   if (draw_mode == SCI) {
      buttons = sci_buttons;
      count = sizeof(sci_buttons);
      width = sci_width;
      height = sci_height;
      cols = sci_cols;
      rows = sci_rows;
   } else {
      buttons = basic_buttons;
      count = sizeof(basic_buttons);
      width = basic_width;
      height = basic_height;
      cols = basic_cols;
      rows = basic_rows;
   }

   count /= sizeof(equate_button);

   Ewl_Widget     *table;
   Ewl_Widget     *button[count];
   Ewl_Widget     *cell[count];
   Ewl_Widget     *displaycell;

   disp[0] = '\0';

   main_win = ewl_window_new();
   ewl_window_set_title(EWL_WINDOW(main_win), "Equate");
   ewl_object_set_minimum_size(EWL_OBJECT(main_win), width, height);
   ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW,
                       destroy_main_window, NULL);

   ewl_callback_append(main_win, EWL_CALLBACK_KEY_DOWN, key_press, NULL);
   ewl_callback_append(main_win, EWL_CALLBACK_KEY_UP, key_un_press, NULL);

   main_box = ewl_vbox_new();
   ewl_container_append_child(EWL_CONTAINER(main_win), main_box);
   ewl_object_set_padding(EWL_OBJECT(main_box), 3, 3, 3, 3);



   ewl_object_set_fill_policy(EWL_OBJECT(main_box), EWL_FLAG_FILL_FILL);
   ewl_container_append_child(EWL_CONTAINER(main_win), main_box);

   table = ewl_grid_new(cols, rows);
   ewl_container_append_child(EWL_CONTAINER(main_box), table);
   ewl_widget_show(table);

   int             bc = count;
   equate_button  *but = buttons;

   displaycell = ewl_cell_new();
   display = ewl_text_new("0");
   ewl_object_set_alignment(EWL_OBJECT(display), EWL_FLAG_ALIGN_LEFT);

   ewl_container_append_child(EWL_CONTAINER(displaycell), display);
   ewl_grid_add(EWL_GRID(table), displaycell, 1, 1, 1, 1);
   /* kinda thought the end col should be 4, but 1 works better... */

   ewl_widget_show(display);
   ewl_widget_show(displaycell);

   while (bc-- > 0) {
      cell[bc] = ewl_cell_new();
      button[bc] = ewl_button_new(but->text);
      but->button = button[bc];

      ewl_callback_append(button[bc], EWL_CALLBACK_MOUSE_DOWN,
                          but->callback, but->cmd);


      ewl_container_append_child(EWL_CONTAINER(cell[bc]), button[bc]);
      ewl_box_set_homogeneous(EWL_BOX(button[bc]), TRUE);
      ewl_object_set_alignment(EWL_OBJECT
                               (EWL_BUTTON(button[bc])->label_object),
                               EWL_FLAG_ALIGN_CENTER);
      ewl_grid_add(EWL_GRID(table), cell[bc], but->col,
                   but->col + but->height - 1, but->row,
                   but->row + but->width - 1);
      ewl_widget_show(button[bc]);
      ewl_widget_show(cell[bc]);

      but++;
   }


   ewl_widget_configure(table);

   ewl_widget_show(main_box);
   ewl_widget_show(main_win);

   ewl_main();

   return;
}
