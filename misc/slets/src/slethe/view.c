#include "main.h"

int win_w = 640, win_h = 460;
int total_prevs = 1;   
int total_slots = 3;
int total_menus = 2;
int menu_end = 250;
int menu_w = 640;
int menu_h = 20;

char win_title[255] = "Slets";
char bg_files[28][255] =
{
 "./img/sky.png",
 ""
};
char icon_files[10][255] =
{
 "./img/bomb.png",
 "./img/book.png",
 "./img/spider.png",
 "./img/log.png",
 "./img/cdrom.png",
 "./img/clock.png",
 "./img/dynamite.png",
 "./img/floppy.png",
 "./img/flower.png",
 ""
};
char font_path[512] = "./fnt";

VIEW_COLOR  text_color;
VIEW_COLOR  menu_color;
VIEW_COLOR  points_color;
VIEW_PREV   **prev;
VIEW_SLOT   **slot;
VIEW_FONT   points_font;
VIEW_MENU   **menu;
VIEW_POINTS points[10];
VIEW_TEXT   text;
VIEW_SOUNDS snd;

GFX_OBJECT score;


void view_exit(int ret)
{
 int i;

/** FIXME:
 **
 ** Commented to avoid strange malloc errors... 

 free(prev);
 free(slot);

 gfx_done(); 
*/

 exit(ret);
}

int view_init(void)
{
 GFX_OBJECT tmp;
 char z[255];
 int i, j, k, x, y;
 CORE_ROW *tmp_row;

 for (i = 0; strlen(bg_files[i]); i++)
   { 
    tmp = gfx_add_image_from_file(bg_files[i]);
    gfx_resize(tmp, win_w, win_h);
    gfx_show(tmp);
   }

 for (i = 0; strlen(icon_files[i]); i++)
   {
    if (!points[(i+1)].exists)
      continue;
    tmp = gfx_add_image_from_file(icon_files[i]);
    gfx_resize(tmp, 50, 50);
    gfx_move(tmp, points[(i+1)].icon.x, points[(i+1)].icon.y);
    gfx_show(tmp);
   }

 for (i = 0; (j = core_get_points(i)); i++)
   {
    if (!points[(i+1)].exists)
      continue;
    sprintf(z, "%d", j);
    tmp = gfx_add_text(points_font.name, points_font.size, z);
    gfx_set_color(tmp, points_color.r, points_color.g, points_color.b, points_color.a);
    gfx_move(tmp, points[(i+1)].text.x, points[(i+1)].text.y);
    gfx_show(tmp);
   }

 j = 0;
 tmp_row = core_new_row();
 for (i = 1; i <= total_prevs; i++)
   {
    tmp = gfx_add_rectangle();
    gfx_set_color(tmp, prev[i]->color.r, prev[i]->color.g, prev[i]->color.b, prev[i]->color.a);
    gfx_resize(tmp, 50, 150);   
    gfx_move(tmp, prev[i]->coords.x, prev[i]->coords.y);
    gfx_show(tmp);

    prev[i]->r = tmp_row;
    
    prev[i]->o[j++] = gfx_add_image_from_file(icon_files[prev[i]->r->x]);
    gfx_resize(prev[i]->o[(j-1)], 50, 50);
    gfx_move(prev[i]->o[(j-1)], prev[i]->coords.x, prev[i]->coords.y);
    gfx_show(prev[i]->o[(j-1)]);
    prev[i]->o[j++] = gfx_add_image_from_file(icon_files[prev[i]->r->y]);
    gfx_resize(prev[i]->o[(j-1)], 50, 50);
    gfx_move(prev[i]->o[(j-1)], prev[i]->coords.x, (prev[i]->coords.y + 50));
    gfx_show(prev[i]->o[(j-1)]);
    prev[i]->o[j++] = gfx_add_image_from_file(icon_files[prev[i]->r->z]);
    gfx_resize(prev[i]->o[(j-1)], 50, 50);
    gfx_move(prev[i]->o[(j-1)], prev[i]->coords.x, (prev[i]->coords.y + 100));
    gfx_show(prev[i]->o[(j-1)]);
   }

 for (i = 1; i <= total_slots; i++)
   {
    slot[i]->s = core_new_slot();;
  
    tmp = gfx_add_rectangle();
    gfx_set_color(tmp, slot[i]->color.r, slot[i]->color.g, slot[i]->color.b, slot[i]->color.a);
    gfx_resize(tmp, 150, 150);
    gfx_move(tmp, slot[i]->coords.x, slot[i]->coords.y);
    gfx_show(tmp);

    x = 0; 
    y = 0;
    for (j = 1; j < 10; j++)
      {
       slot[i]->o[j] = gfx_add_image_from_file(icon_files[0]);
       gfx_resize(slot[i]->o[j], 50, 50);
       gfx_move(slot[i]->o[j], (slot[i]->coords.x + x), (slot[i]->coords.y + y));
       gfx_hide(slot[i]->o[j]);
       if (!(j % 3))
         {
          y = 0;
          x += 50;
         }
       else
         y += 50;
      }       
   }

 tmp = gfx_add_rectangle();
 gfx_set_color(tmp, menu_color.r, menu_color.g, menu_color.b, menu_color.a);
 gfx_resize(tmp, menu_w, menu_h);
 gfx_move(tmp, 0, 0);
 gfx_show(tmp);

 for (i = 1; i <= total_menus; i++)
   {
    tmp = gfx_add_text(menu[i]->font.name, menu[i]->font.size,
                           menu[i]->text);
    gfx_set_color(tmp, menu[i]->color.r, menu[i]->color.g,
                           menu[i]->color.b, menu[i]->color.a);
    gfx_move(tmp, menu[i]->x, 0);
    gfx_show(tmp);
   }

 sprintf(z, "Total = %d     Score = %d", core_get_total(), core_get_score());
 score = gfx_add_text(text.font.name, text.font.size, z);
 gfx_set_color(score, text_color.r, text_color.g, text_color.b, text_color.a);
 gfx_move(score, text.coords.x, text.coords.y);   
 gfx_show(score);

 gfx_render();

 return 1;
}

void view_main(void)
{
 GFX_EVENT ev;
 extern GFX_ATOM a_win_del;
 int i, x, y;

 for(;;)
   {
    while (gfx_pending())
      {
       gfx_next_event(&ev);
       switch(ev.type)
         {
          case ButtonPress:
            printf("Not implemented yet..\n");
          break;
          case Expose:
            gfx_update_rect(ev.xexpose.x, ev.xexpose.y,
                            ev.xexpose.width, ev.xexpose.height);
            gfx_render();
          break;   
          case ClientMessage:
            if (ev.xclient.format == 32
                && ev.xclient.data.l[0] == (signed) a_win_del)
              {   
               view_exit(0);
              }
            break;
         }
      }
   }
} 
