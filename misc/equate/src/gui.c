#include "Equate.h"

#include <Ewl.h>

Ewl_Widget     *main_win;
Ewl_Widget     *main_box;
Ewl_Widget     *display;

char            tmp[BUFLEN];
char            disp[BUFLEN];

// needed 'cos generated h files suck
double          yyresult(void);

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

static equate_button buttons[] = {
	{2, 1, 1, 1, "/", "/", (void *) calc_append, NULL},
	{2, 2, 1, 1, "*", "*", (void *) calc_append, NULL},
	{2, 3, 1, 1, "-", "-", (void *) calc_append, NULL},
	{2, 4, 1, 1, "+", "+", (void *) calc_append, NULL},
	{3, 1, 1, 1, "7", "7", (void *) calc_append, NULL},
	{3, 2, 1, 1, "8", "8", (void *) calc_append, NULL},
	{3, 3, 1, 1, "9", "9", (void *) calc_append, NULL},
	{3, 4, 1, 1, "(", "(", (void *) calc_append, NULL},
	{4, 1, 1, 1, "4", "4", (void *) calc_append, NULL},
	{4, 2, 1, 1, "5", "5", (void *) calc_append, NULL},
	{4, 3, 1, 1, "6", "6", (void *) calc_append, NULL},
	{4, 4, 1, 1, ")", ")", (void *) calc_append, NULL},
	{5, 1, 1, 1, "1", "1", (void *) calc_append, NULL},
	{5, 2, 1, 1, "2", "2", (void *) calc_append, NULL},
	{5, 3, 1, 1, "3", "3", (void *) calc_append, NULL},
	{5, 4, 2, 1, "=", "=", (void *) calc_exec, NULL},
	{6, 1, 1, 1, "c", "c", (void *) calc_clear, NULL},
	{6, 2, 1, 1, "0", "0", (void *) calc_append, NULL},
	{6, 3, 1, 1, ".", ".", (void *) calc_append, NULL},
};

void
update_display(char *text)
{
	ewl_text_set_text((Ewl_Text *) display, text);
}

void
calc_append(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char            key;
	int             len;

	key = (char) *((char *) user_data);
	len = strlen(tmp);
	tmp[len] = key;
	tmp[len + 1] = '\0';
	len = strlen(disp);
	disp[len] = key;
	disp[len + 1] = '\0';
	update_display(disp);
}

void
calc_op(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char            key;
	double          val;
	int             len;

	if (tmp[0] != '\0') {
		sscanf(tmp, "%lf", &val);
		//equate_parse_val(val);
	}

	tmp[0] = '\0';
	key = (char) *((char *) user_data);
	switch (key) {
/*	case '+':
		equate_parse_add();
		break;
	case '-':
		equate_parse_sub();
		break;
	case '*':
		equate_parse_mult();
		break;
	case '/':
		equate_parse_div();
		break;
	case '(':
		equate_parse_open_brak();
		break;
	case ')':
		equate_parse_close_brak();
		break;*/
	case '=':
		//equate_print ();
		calc_exec();
		break;
	}

	if (key != '=') {
		len = strlen(disp);
		disp[len] = key;
		disp[len + 1] = '\0';
		update_display(disp);
	} else
		disp[0] = '\0';
}

void
calc_exec(void)
{
	char            res[BUFLEN];

	yy_scan_string(tmp);
	yyparse();

	snprintf(res, BUFLEN, "%.10g", yyresult());
	update_display(res);
	tmp[0] = '\0';
	disp[0] = '\0';
}

void
calc_clear(Ewl_Widget * w, void *ev_data, void *user_data)
{
	//equate_clear();
	update_display("0");
	tmp[0] = '\0';
	disp[0] = '\0';
}

void
destroy_main_window(Ewl_Widget * main_win, void *ev_data, void *user_data)
{
	ewl_widget_destroy(main_win);
	ewl_main_quit();
	return;
}

void
key_press(Ewl_Widget * w, void *ev_data, void *user_data)
{
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

	int             bc = sizeof(buttons) / sizeof(equate_button);
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
draw_interface(void)
{
	int             count = sizeof(buttons) / sizeof(equate_button);
	Ewl_Widget     *table;
	Ewl_Widget     *button[count];
	Ewl_Widget     *cell[count];
	Ewl_Widget     *displaycell;

//	equate_init();

	tmp[0] = '\0';
	disp[0] = '\0';

	main_win = ewl_window_new();
	ewl_window_set_title(EWL_WINDOW(main_win), "Equate");
	ewl_object_set_minimum_size(EWL_OBJECT(main_win), 120, 160);
	ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW,
			    destroy_main_window, NULL);

	ewl_callback_append(main_win, EWL_CALLBACK_KEY_DOWN, key_press, NULL);
	ewl_callback_append(main_win, EWL_CALLBACK_KEY_UP, key_un_press, NULL);

	main_box = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(main_win), main_box);
	ewl_object_set_padding(EWL_OBJECT(main_box), 3, 3, 3, 3);



	ewl_object_set_fill_policy(EWL_OBJECT(main_box), EWL_FLAG_FILL_FILL);
	ewl_container_append_child(EWL_CONTAINER(main_win), main_box);

	table = ewl_grid_new(4, 6);
	ewl_container_append_child(EWL_CONTAINER(main_box), table);
	ewl_widget_show(table);

	int             bc = sizeof(buttons) / sizeof(equate_button);
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
