#ifndef ENGRAVE_FONT_H
#define ENGRAVE_FONT_H

typedef struct _Engrave_Font Engrave_Font;
struct _Engrave_Font
{
  char *name; /* alias */
  char *file; /* basename */
  char *path; /* dir path */
};

Engrave_Font * engrave_font_new(char *path, char *name);

#endif

