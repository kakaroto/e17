#ifndef _EQUATE_GUI_H
#define _EQUATE_GUI_H
typedef enum {
   DEFAULT,
   BASIC,
   SCI
} Mode;

typedef enum {
   DEF,
   EWL,
   EDJE
} UI;

// needed 'cos generated h files suck
double          yyresult(void);

void            update_display(char *text);

void            draw_ewl(Mode mode);

#endif
