#include <signal.h>
#include <limits.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>

#include "Ewl.h"
#include "config.h"

#define DEBUG 1
#define BUFLEN 100

#define E(lvl,fmt,args...) do { if(DEBUG>=(lvl)) fprintf(stderr,fmt, args); } while(0)



Ewl_Widget *main_win;
Ewl_Widget *main_box;
Ewl_Widget *display;

int calc[2];
char *buffer;

typedef struct equate_button
{
  int row;
  int col;
  int width;
  int height;
  char *text;
  char *cmd;
  Ewl_Widget *button;
} equate_button;

static equate_button buttons[] = {
  {2, 1, 1, 1, "/", "/"},
  {2, 2, 1, 1, "*", "*"},
  {2, 3, 1, 1, "-", "-"},
  {2, 4, 1, 1, "+", "+"},
  {3, 1, 1, 1, "7", "7"},
  {3, 2, 1, 1, "8", "8"},
  {3, 3, 1, 1, "9", "9"},
  {3, 4, 1, 1, "(", "("},
  {4, 1, 1, 1, "4", "4"},
  {4, 2, 1, 1, "5", "5"},
  {4, 3, 1, 1, "6", "6"},
  {4, 4, 1, 1, ")", ")"},
  {5, 1, 1, 1, "1", "1"},
  {5, 2, 1, 1, "2", "2"},
  {5, 3, 1, 1, "3", "3"},
  {5, 4, 2, 1, "=", "="},
  {6, 2, 1, 1, "0", "0"},
  {6, 3, 1, 1, ".", "."},
};

void
err_msg (const char *str)
{
  fprintf (stderr, "%s\n", str);
  exit (1);
}
static void
sig_pipe (int signo)
{
  err_msg ("SIGPIPE caught\n");
}

void
update_display (char *text)
{
  ewl_text_set_text ((Ewl_Text *) display, text);
  E (2, "Evaluated to '%s'.\n", text);
}

void
calc_append (Ewl_Widget * w, void *ev_data, void *user_data)
{
  int n = strlen (buffer);
  int m = strlen ((char *) user_data);

  E (4, "Execing %s ...\n", (char *) user_data);
  memcpy (&buffer[n], (char *) user_data, m);
  buffer[n + m] = 0;
  E (5, "appended to: %s\n", buffer);

  update_display (buffer);
}

void
calc_exec (Ewl_Widget * w, void *ev_data, void *user_data)
{
  int n = strlen (buffer), i;
  char line[PATH_MAX];

  if (n <= 0)
    return;

  buffer[n] = '\n';
  buffer[++n] = 0;

  E (2, "Evaluating %d chars:%s", n, buffer);

  for (i = 0; i < n; i++)
    E (9, "(char %d is %d)\n", i, buffer[i]);

  write (calc[0], buffer, n);

  n = strlen (line);
  if ((n = read (calc[0], line, PATH_MAX)) < 0)
    {
      err_msg ("parent: read error from pipe");
    }
  if (n == 0)
    {
      err_msg ("parent: child closed pipe");
      return;
    }
  line[n - 1] = 0;		/* cut off the traing CR while delimiting the string */

  update_display (line);
  buffer[0] = 0;
}

void
draw_interface (void)
{
  int count = sizeof (buttons) / sizeof (equate_button);
  Ewl_Widget *table;
  Ewl_Widget *button[count];
  Ewl_Widget *cell[count];
  Ewl_Widget *displaycell;

  ewl_object_set_fill_policy (EWL_OBJECT (main_box), EWL_FLAG_FILL_FILL);
  ewl_container_append_child (EWL_CONTAINER (main_win), main_box);

  table = ewl_grid_new (4, 6);
  ewl_container_append_child (EWL_CONTAINER (main_box), table);
  ewl_widget_show (table);

  int bc = sizeof (buttons) / sizeof (equate_button);
  equate_button *but = buttons;

  displaycell = ewl_cell_new ();
  display = ewl_text_new ("0.0");
  ewl_object_set_alignment (EWL_OBJECT (display), EWL_FLAG_ALIGN_LEFT);

  ewl_container_append_child (EWL_CONTAINER (displaycell), display);
  ewl_grid_add (EWL_GRID (table), displaycell, 1, 1, 1, 1);
  /* kinda thought the end col should be 4, but 1 works better... */

  ewl_widget_show (display);
  ewl_widget_show (displaycell);

  while (bc-- > 0)
    {
      cell[bc] = ewl_cell_new ();
      button[bc] = ewl_button_new (but->text);
      but->button = button[bc];

      if (strcmp ("=", but->cmd))
	ewl_callback_append (button[bc], EWL_CALLBACK_MOUSE_DOWN, (void *)calc_append,
			     but->cmd);
      else
	ewl_callback_append (button[bc], EWL_CALLBACK_MOUSE_DOWN, (void *)calc_exec,
			     but->cmd);


      ewl_container_append_child (EWL_CONTAINER (cell[bc]), button[bc]);
      ewl_box_set_homogeneous (EWL_BOX (button[bc]), TRUE);
      ewl_object_set_alignment (EWL_OBJECT
				(EWL_BUTTON (button[bc])->label_object),
				EWL_FLAG_ALIGN_CENTER);
      ewl_grid_add (EWL_GRID (table), cell[bc], but->col,
		    but->col + but->height - 1, but->row,
		    but->row + but->width - 1);
      ewl_widget_show (button[bc]);
      ewl_widget_show (cell[bc]);

      but++;
    }


  ewl_widget_configure (table);
  return;
}

void
destroy_main_window (Ewl_Widget * main_win, void *ev_data, void *user_data)
{
  ewl_widget_destroy (main_win);
  ewl_main_quit ();
  return;
}

void
key_press (Ewl_Widget * w, void *ev_data, void *user_data)
{
  do_key (ev_data, EWL_CALLBACK_MOUSE_DOWN);
}

void
key_un_press (Ewl_Widget * w, void *ev_data, void *user_data)
{
  do_key (ev_data, EWL_CALLBACK_MOUSE_UP);
}

int
do_key (void *ev_data, int action)
{
  Ecore_X_Event_Key_Down *ev;
  ev = ev_data;

  if (ev->key_compose)
    E (6, "Key pressed: %s\n", ev->key_compose);

  int bc = sizeof (buttons) / sizeof (equate_button);
  equate_button *but = buttons;
  while (bc-- > 0)
    {
      if (ev->key_compose)
	if (!strcmp (ev->key_compose, but->cmd))
	  {
	    E (4, "Pressing button %s\n", but->text);

	    ewl_callback_call (but->button, action);

	    break;
	  }
      but++;
    }

  return 0;
}

int
main (int argc, char *argv[], char *env[])
{

  int n;
  pid_t pid;
  char line[PATH_MAX];

  if (signal (SIGPIPE, sig_pipe) == SIG_ERR)
    err_msg ("Signal Error");
  else if (socketpair (PF_LOCAL, SOCK_STREAM, 0, calc) < 0)
    err_msg ("Pipe Error");
  else if ((pid = fork ()) < 0)
    {
      err_msg ("Fork Failure");
    }
  else if (pid > 0)
    {
      close (calc[1]);
      /* now we continue as normal and load the ewl widgets etc */
    }
  else
    {
      close (calc[0]);
      if (calc[1] != STDIN_FILENO)
	{
	  if (dup2 (calc[1], STDIN_FILENO) != STDIN_FILENO)
	    {
	      err_msg ("Dup2 error to stdin");
	    }
	}
      if (calc[1] != STDOUT_FILENO)
	{
	  if (dup2 (calc[1], STDOUT_FILENO) != STDOUT_FILENO)
	    {
	      err_msg ("Dup2 error to stdout");
	    }
	}
      if (execl (BC_LOCATION, "bc", "-l", NULL) < 0)
	err_msg ("Execl error");
      exit (0);
    }

  buffer = malloc (BUFLEN);
  buffer[0] = 0;

  ewl_init (&argc, argv);

  main_win = ewl_window_new ();
  ewl_window_set_title (EWL_WINDOW (main_win), "Equate");
  ewl_object_set_minimum_size (EWL_OBJECT (main_win), 120, 160);
  ewl_callback_append (main_win, EWL_CALLBACK_DELETE_WINDOW,
		       destroy_main_window, NULL);

  ewl_callback_append (main_win, EWL_CALLBACK_KEY_DOWN, key_press, NULL);
  ewl_callback_append (main_win, EWL_CALLBACK_KEY_UP, key_un_press, NULL);

  main_box = ewl_vbox_new ();
  ewl_container_append_child (EWL_CONTAINER (main_win), main_box);
  ewl_object_set_padding (EWL_OBJECT (main_box), 3, 3, 3, 3);


  draw_interface ();
  ewl_widget_show (main_box);
  ewl_widget_show (main_win);



  ewl_main ();

  return 0;
}
