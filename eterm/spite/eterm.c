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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eterm.h"
#define BUFFER_LEN 1024

extern void
create_contexts(void)
{
  color = (Color *)malloc(sizeof(Color));
  color->foreground = (char *) malloc(BUFFER_LEN);
  color->background = (char *) malloc(BUFFER_LEN);
  color->cursor = (char *) malloc(BUFFER_LEN);
  color->cursor_text = (char *) malloc(BUFFER_LEN);
  color->pointer = (char *) malloc(BUFFER_LEN);
  attributes = (Attributes *) malloc(sizeof(Attributes));
  attributes->title = (char *) malloc(BUFFER_LEN);
  attributes->name = (char *) malloc(BUFFER_LEN);
  attributes->iconname = (char *) malloc(BUFFER_LEN);
  attributes->font0 = (char *) malloc(BUFFER_LEN);
  attributes->font1 = (char *) malloc(BUFFER_LEN);
  attributes->font2 = (char *) malloc(BUFFER_LEN);
  attributes->font3 = (char *) malloc(BUFFER_LEN);
  attributes->font4 = (char *) malloc(BUFFER_LEN);
  attributes->font5 = (char *) malloc(BUFFER_LEN);
  attributes->font6 = (char *) malloc(BUFFER_LEN);
  attributes->font_bold = (char *) malloc(BUFFER_LEN);
  imageclass = (Imageclass *) malloc(sizeof(Imageclass));
  imageclass->path = (char *) malloc(BUFFER_LEN);
  imageclass->icon = (char *) malloc(BUFFER_LEN);
  imageclass->anim = (char *) malloc(BUFFER_LEN);
  imageclass->image_bg = (Image *) malloc(sizeof(Image));
  imageclass->image_bg->mode = (char *) malloc(BUFFER_LEN);
  imageclass->image_bg->normal = (State *) malloc(sizeof(State));
  imageclass->image_bg->normal->color = (char *) malloc(BUFFER_LEN);
  imageclass->image_bg->normal->file = (char *) malloc(BUFFER_LEN);
  imageclass->image_bg->normal->geom = (char *) malloc(BUFFER_LEN);
  imageclass->image_bg->normal->border = (char *) malloc(BUFFER_LEN);
  imageclass->image_bg->selected = (State *) malloc(sizeof(State));
  imageclass->image_bg->selected->color = (char *) malloc(BUFFER_LEN);
  imageclass->image_bg->selected->file = (char *) malloc(BUFFER_LEN);
  imageclass->image_bg->selected->geom = (char *) malloc(BUFFER_LEN);
  imageclass->image_bg->selected->border = (char *) malloc(BUFFER_LEN);
  imageclass->image_bg->clicked = (State *) malloc(sizeof(State));
  imageclass->image_bg->clicked->color = (char *) malloc(BUFFER_LEN);
  imageclass->image_bg->clicked->file = (char *) malloc(BUFFER_LEN);
  imageclass->image_bg->clicked->geom = (char *) malloc(BUFFER_LEN);
  imageclass->image_bg->clicked->border = (char *) malloc(BUFFER_LEN);
  imageclass->image_trough = (Image *) malloc(sizeof(Image));
  imageclass->image_trough->mode = (char *) malloc(BUFFER_LEN);
  imageclass->image_trough->normal = (State *) malloc(sizeof(State));
  imageclass->image_trough->normal->color = (char *) malloc(BUFFER_LEN);
  imageclass->image_trough->normal->file = (char *) malloc(BUFFER_LEN);
  imageclass->image_trough->normal->geom = (char *) malloc(BUFFER_LEN);
  imageclass->image_trough->normal->border = (char *) malloc(BUFFER_LEN);
  imageclass->image_trough->selected = (State *) malloc(sizeof(State));
  imageclass->image_trough->selected->color = (char *) malloc(BUFFER_LEN);
  imageclass->image_trough->selected->file = (char *) malloc(BUFFER_LEN);
  imageclass->image_trough->selected->geom = (char *) malloc(BUFFER_LEN);
  imageclass->image_trough->selected->border = (char *) malloc(BUFFER_LEN);
  imageclass->image_trough->clicked = (State *) malloc(sizeof(State));
  imageclass->image_trough->clicked->color = (char *) malloc(BUFFER_LEN);
  imageclass->image_trough->clicked->file = (char *) malloc(BUFFER_LEN);
  imageclass->image_trough->clicked->geom = (char *) malloc(BUFFER_LEN);
  imageclass->image_trough->clicked->border = (char *) malloc(BUFFER_LEN);
  imageclass->image_anchor = (Image *) malloc(sizeof(Image));
  imageclass->image_anchor->mode = (char *) malloc(BUFFER_LEN);
  imageclass->image_anchor->normal = (State *) malloc(sizeof(State));
  imageclass->image_anchor->normal->color = (char *) malloc(BUFFER_LEN);
  imageclass->image_anchor->normal->file = (char *) malloc(BUFFER_LEN);
  imageclass->image_anchor->normal->geom = (char *) malloc(BUFFER_LEN);
  imageclass->image_anchor->normal->border = (char *) malloc(BUFFER_LEN);
  imageclass->image_anchor->selected = (State *) malloc(sizeof(State));
  imageclass->image_anchor->selected->color = (char *) malloc(BUFFER_LEN);
  imageclass->image_anchor->selected->file = (char *) malloc(BUFFER_LEN);
  imageclass->image_anchor->selected->geom = (char *) malloc(BUFFER_LEN);
  imageclass->image_anchor->selected->border = (char *) malloc(BUFFER_LEN);
  imageclass->image_anchor->clicked = (State *) malloc(sizeof(State));
  imageclass->image_anchor->clicked->color = (char *) malloc(BUFFER_LEN);
  imageclass->image_anchor->clicked->file = (char *) malloc(BUFFER_LEN);
  imageclass->image_anchor->clicked->geom = (char *) malloc(BUFFER_LEN);
  imageclass->image_anchor->clicked->border = (char *) malloc(BUFFER_LEN);
  imageclass->image_up_arrow = (Image *) malloc(sizeof(Image));
  imageclass->image_up_arrow->mode = (char *) malloc(BUFFER_LEN);
  imageclass->image_up_arrow->normal = (State *) malloc(sizeof(State));
  imageclass->image_up_arrow->normal->color = (char *) malloc(BUFFER_LEN);
  imageclass->image_up_arrow->normal->file = (char *) malloc(BUFFER_LEN);
  imageclass->image_up_arrow->normal->geom = (char *) malloc(BUFFER_LEN);
  imageclass->image_up_arrow->normal->border = (char *) malloc(BUFFER_LEN);
  imageclass->image_up_arrow->selected = (State *) malloc(sizeof(State));
  imageclass->image_up_arrow->selected->color = (char *) malloc(BUFFER_LEN);
  imageclass->image_up_arrow->selected->file = (char *) malloc(BUFFER_LEN);
  imageclass->image_up_arrow->selected->geom = (char *) malloc(BUFFER_LEN);
  imageclass->image_up_arrow->selected->border = (char *) malloc(BUFFER_LEN);
  imageclass->image_up_arrow->clicked = (State *) malloc(sizeof(State));
  imageclass->image_up_arrow->clicked->color = (char *) malloc(BUFFER_LEN);
  imageclass->image_up_arrow->clicked->file = (char *) malloc(BUFFER_LEN);
  imageclass->image_up_arrow->clicked->geom = (char *) malloc(BUFFER_LEN);
  imageclass->image_up_arrow->clicked->border = (char *) malloc(BUFFER_LEN);

  buttonbar = (Buttonbar *)malloc(sizeof(Buttonbar));
  toggles = (Toggles *)malloc(sizeof(Toggles));
  misc = (Misc *)malloc(sizeof(Misc));
  misc->print_pipe = (char *) malloc(BUFFER_LEN);
  misc->save_lines = (char *) malloc(BUFFER_LEN);
  misc->cut_chars =  (char *) malloc(BUFFER_LEN);
  misc->border_width = (char *) malloc(BUFFER_LEN);
  misc->line_space = (char *) malloc(BUFFER_LEN);
  misc->term_name = (char *) malloc(BUFFER_LEN);
  misc->exec = (char *) malloc(BUFFER_LEN);
}

extern void
destroy_contexts(void)
{
  free (color->pointer);
  free (color->cursor_text);
  free (color->cursor);
  free (color->background);
  free (color->foreground);
  free (color);
  free (attributes->font_bold);
  free (attributes->font6);
  free (attributes->font5);
  free (attributes->font4);
  free (attributes->font3);
  free (attributes->font2);
  free (attributes->font1);
  free (attributes->font0);
  free (attributes->iconname);
  free (attributes->name);
  free (attributes->title);
  free (attributes);
  free (toggles);
  free (misc->exec);
  free (misc->term_name);
  free (misc->line_space);
  free (misc->border_width);
  free (misc->cut_chars);
  free (misc->save_lines);
  free (misc->print_pipe);
  free (misc);
}
extern void
print_theme(void){
  printf("<Eterm-0.9>\n");
  printf("begin main\n");
  print_color(); 
  print_attributes();
  get_toggles();
  print_toggles();
  print_misc();
  printf("end main\n");
}

extern void
print_color(void)
{
  printf("   begin color\n");
  printf("      foreground %s\n", color->foreground);
  printf("      background %s\n", color->background);
  printf("      cursor %s\n", color->cursor);
  printf("      cursor_text %s\n", color->cursor_text);
  printf("      pointer %s\n", color->pointer);
  if (color->video){
    printf("      video normal\n");
  }
  else{
    printf("      video reverse\n");
  }
  printf("   end color\n");
}

extern void
print_attributes(void)
{
  printf("   begin attributes\n");
  printf("      geom %dx%d+%d+%d\n", attributes->width, attributes->height, attributes->offX, attributes->offY);
  printf("      title %s\n", attributes->title);
  printf("      name %s\n", attributes->name);
  printf("      iconname %s\n", attributes->iconname);
  /*  printf("font default %s\n", attributes->font_default);*/
  printf("      font 0 %s\n", attributes->font0);
  printf("      font 1 %s\n", attributes->font1);
  printf("      font 2 %s\n", attributes->font2);
  printf("      font 3 %s\n", attributes->font3);
  printf("      font 4 %s\n", attributes->font4);
  printf("      font 5 %s\n", attributes->font5);
  printf("      font 6 %s\n", attributes->font6);
  printf("      font bold %s\n", attributes->font_bold);
  printf("   end attributes\n");
}

extern void
print_toggles(void)
{
  printf("   begin toggles\n");
  printf("      map_alert %d\n", toggles->map_alert);
  printf("      visual_bell %d\n", toggles->visual_bell);
  printf("      login_shell %d\n", toggles->login_shell);
  printf("      scrollbar %d\n", toggles->scrollbar);
  printf("      utmp_logging %d\n", toggles->utmp_logging);
  printf("      select_line %d\n", toggles->select_line);
  printf("      meta8 %d\n", toggles->meta8);
  printf("      iconic %d\n", toggles->iconic);
  printf("      home_on_output %d\n", toggles->home_on_output);
  printf("      home_on_input %d\n", toggles->home_on_input);
  printf("      scrollbar_right %d\n", toggles->scrollbar_right);
  printf("      scrollbar_floating %d\n", toggles->scrollbar_floating);
  printf("      borderless %d\n", toggles->borderless);
  printf("      double_buffer %d\n", toggles->double_buffer);
  printf("   end toggles\n");
}

extern void
print_misc(void)
{
  printf("   begin misc\n");
  printf("      print_pipe %s\n", misc->print_pipe);
  printf("      save_lines %s\n", misc->save_lines);
  printf("      cut_chars %s\n", misc->cut_chars);
  printf("      border_width %s\n", misc->border_width);
  printf("      line_space %s\n", misc->line_space);
  printf("      term_name %s\n", misc->term_name);
  printf("      exec %s\n", misc->exec);
  printf("   end misc\n");
}
