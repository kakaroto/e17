/*
 *      SPITE v0.2 (C) 2000 Michael Lea (mazeone@dcaa.net)
 *
 *              SPITE Provides Independent Theming for Eterm.
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2, or (at your option)
 *      any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program (see the file COPYING); if not, write to the
 *      Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *      Boston, MA  02111-1307, USA
 *
 */

typedef struct {
  char *foreground;
  char *background;
  char *cursor;
  char *cursor_text;
  char *pointer;
  char video;
}Color;

typedef struct {
  int height;
  int width;
  int offX;
  int offY;
  char *title;
  char *name;
  char *iconname;
  char font_default;
  char *font0;
  char *font1;
  char *font2;
  char *font3;
  char *font4;
  char *font5;
  char *font6;
  char *font_bold;
}Attributes;

typedef struct {
  char *color;
  char *file;
  char *geom;
  char *border;
}State;

typedef struct {
  char *mode;
  char allowed;
  State *normal;
  State *selected;
  State *clicked;
}Image;

typedef struct {
  char *path;
  char *icon;
  char *anim;
  Image *image_bg;
  Image *image_trough;
  Image *image_anchor;
  Image *image_thumb;
  Image *image_up_arrow;
  Image *image_down_arrow;
  Image *image_menu;
  Image *image_submenu;
  Image *image_buttonbar;
  Image *image_button;
}Imageclass;

typedef struct {
  char *name;
  char *type;
  char *data;
}Button;

typedef struct {
  char *font;
  Button *buttons;
}Buttonbar;

typedef struct {
  char map_alert;
  char visual_bell;
  char login_shell;
  char scrollbar;
  char utmp_logging;
  char select_line;
  char meta8;
  char iconic;
  char home_on_output;
  char home_on_input;
  char scrollbar_right;
  char scrollbar_floating;
  char borderless;
  char double_buffer;
} Toggles;

typedef struct {
  char *print_pipe;
  char *save_lines;
  char *cut_chars;
  char *border_width;
  char *line_space;
  char *term_name;
  char *exec;
}Misc;

Color *color;
Attributes *attributes;
Imageclass *imageclass;
Buttonbar *buttonbar;
Toggles *toggles;
Misc *misc;

extern void create_contexts(void);
extern void destroy_contexts(void);
extern void print_theme(void);
extern void print_color(void);
extern void print_attributes(void);
extern void print_toggles(void);
extern void get_toggles(void);
extern void print_misc(void);
