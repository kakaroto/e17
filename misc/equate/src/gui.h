#ifndef _EQUATE_GUI_H
#define _EQUATE_GUI_H
typedef enum
{
  BASIC,
  SCI
} mode;

// needed 'cos generated h files suck
double yyresult (void);

void update_display (char *text);

void draw_interface (mode draw_mode);

#endif
