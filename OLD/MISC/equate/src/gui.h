#ifndef _EQUATE_GUI_H
#define _EQUATE_GUI_H

/* needed 'cos generated h files suck */
double          yyresult(void);

void            update_display(char *text);

void            equate_gui_init(Equate * equate, int argc, char **arvg);
void            equate_gui_quit(void);
void            equate_gui_update(Equate * equate);
#endif
