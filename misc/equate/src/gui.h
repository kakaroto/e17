#ifndef _EQUATE_GUI_H
#define _EQUATE_GUI_H
typedef enum
{
  BASIC,
  SCI
} mode;

void update_display (char *text);

void draw_interface (mode draw_mode);

#endif
