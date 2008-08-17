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
 DATA_DIR "/img/sky.png",
 ""
};
char icon_files[10][255] =
{
 DATA_DIR "/img/bomb.png",
 DATA_DIR "/img/book.png",
 DATA_DIR "/img/spider.png",
 DATA_DIR "/img/log.png",
 DATA_DIR "/img/cdrom.png",
 DATA_DIR "/img/clock.png",
 DATA_DIR "/img/dynamite.png",
 DATA_DIR "/img/floppy.png",
 DATA_DIR "/img/flower.png",
 ""
};
char font_path[512] = DATA_DIR "/fnt";

GAME_COLOR text_color;
GAME_COLOR  menu_color;
GAME_COLOR  points_color;
GAME_PREV   **prev;
GAME_SLOT   **slot;
GAME_FONT   points_font;
GAME_MENU   **menu;
GAME_POINTS points[10];
GAME_TEXT   text;
GAME_SOUNDS snd;

GFX_OBJECT score;


void game_exit(int ret)
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

void game_put_prev(void)
{
 CORE_ROW *tmp_row;
 int i, j;

 j = 0;
 tmp_row = core_new_row();

 for (i = 1; i <= total_prevs; i++)
   {
    prev[i]->r = tmp_row;
    
    gfx_set_image_file(prev[i]->o[j++], icon_files[prev[i]->r->x]);
    gfx_move(prev[i]->o[(j-1)], prev[i]->coords.x, prev[i]->coords.y);
    gfx_show(prev[i]->o[(j-1)]);
    gfx_set_image_file(prev[i]->o[j++], icon_files[prev[i]->r->y]);
    gfx_move(prev[i]->o[(j-1)], prev[i]->coords.x, (prev[i]->coords.y + 50));
    gfx_show(prev[i]->o[(j-1)]);
    gfx_set_image_file(prev[i]->o[j++], icon_files[prev[i]->r->z]); 
    gfx_move(prev[i]->o[(j-1)], prev[i]->coords.x, (prev[i]->coords.y + 100));
    gfx_show(prev[i]->o[(j-1)]);
   } 

 gfx_render();
}

void game_put_score(void)
{
 char z[255];

 sprintf(z, "Total = %d     Score = %d", core_get_total(), core_get_score());
 gfx_set_text(score, z);
}

int game_put_in_slot(int wich)
{
 int i, j;

 if (slot[wich]->s->wich > 3)
   return 0;

 core_put_in_slot(prev[1]->r, slot[wich]->s);
 core_free(prev[1]->r);

 switch ((slot[wich]->s->wich-1))
   {
    case 1: 
      j = 1;
      i = slot[wich]->s->row1->x;
      gfx_set_image_file(slot[wich]->o[j++], icon_files[i]);
      gfx_show(slot[wich]->o[(j-1)]);
      i = slot[wich]->s->row1->y;
      gfx_set_image_file(slot[wich]->o[j++], icon_files[i]);
      gfx_show(slot[wich]->o[(j-1)]);
      i = slot[wich]->s->row1->z;
      gfx_set_image_file(slot[wich]->o[j++], icon_files[i]);
      gfx_show(slot[wich]->o[(j-1)]);
    break;
    case 2: 
      j = 4;
      i = slot[wich]->s->row2->x;
      gfx_set_image_file(slot[wich]->o[j++], icon_files[i]);
      gfx_show(slot[wich]->o[(j-1)]);
      i = slot[wich]->s->row2->y;
      gfx_set_image_file(slot[wich]->o[j++], icon_files[i]);
      gfx_show(slot[wich]->o[(j-1)]);
      i = slot[wich]->s->row2->z;
      gfx_set_image_file(slot[wich]->o[j++], icon_files[i]);
      gfx_show(slot[wich]->o[(j-1)]);
    break;
    case 3: 
      j = 7;
      i = slot[wich]->s->row3->x;
      gfx_set_image_file(slot[wich]->o[j++], icon_files[i]);
      gfx_show(slot[wich]->o[(j-1)]); 
      i = slot[wich]->s->row3->y;
      gfx_set_image_file(slot[wich]->o[j++], icon_files[i]);
      gfx_show(slot[wich]->o[(j-1)]);
      i = slot[wich]->s->row3->z;
      gfx_set_image_file(slot[wich]->o[j++], icon_files[i]);
      gfx_show(slot[wich]->o[(j-1)]);
    break;
   }

 game_put_score();

 gfx_render();

 game_put_prev();

 return 1;
}

void game_rehash(void)
{
 int i, j;

 for (i = 1; i <= total_slots; i++)
   {
    for (j = 1; j < 10; j++)
      {
       gfx_hide(slot[i]->o[j]);
      }
    core_rehash_slot(slot[i]->s);
   }

 core_rehash();

 game_put_score();

 gfx_render();
}

int game_init(void)
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

 if (strlen(snd.startup))
   esd_play_file(NULL, snd.startup, 1);

 return 1;
}

void game_main(void)
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
            for (i = 1; i <= total_slots; i++)
              {
               if ((ev.xbutton.x > slot[i]->coords.x) && (ev.xbutton.x < (slot[i]->coords.x + 150)) &&
                   (ev.xbutton.y > slot[i]->coords.y) && (ev.xbutton.y < (slot[i]->coords.y + 150)) )
                 {
                  if (strlen(snd.slot))
                    esd_play_file(NULL, snd.slot, 1);
                  game_put_in_slot(i);
                  break;
                 }
              }

            for (i = 1; i <= total_menus; i++)
              {
               if (i == 1)
                 x = 0;
               else
                 x = menu[i-1]->x;
               
               if (i == total_menus)
                 y = menu_end;
               else
                 y = menu[i+1]->x;
              
               if ((ev.xbutton.x > x) && (ev.xbutton.x < y) &&
                   (ev.xbutton.y > 0) && (ev.xbutton.y < menu_h) )
                 {
                  if (strlen(snd.menu))
                    esd_play_file(NULL, snd.menu, 1);
                  switch (i)
                    {
                     case 1: 
                       game_rehash();       
                     break;
                     case 2: 
                       game_exit(0);
                     break;
                    }
                  break;
                 }
              }
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
               game_exit(0);
              }
            break;
         }
      }

    /* the blinking.. */
    for (i = 1; i <= total_slots; i++)
      {
       switch ((slot[i]->s->wich-1))
         {
          case 1:
            if ((slot[i]->s->row1->x ==
                 prev[1]->r->x)      ||
                (slot[i]->s->row1->x ==
                 prev[1]->r->y)      )
              {
               gfx_hide(slot[i]->o[1]);
               gfx_render();
               gfx_flush();
               sleep(0.5);
               gfx_show(slot[i]->o[1]);
               gfx_render();
              }
            if ((slot[i]->s->row1->y ==
                 prev[1]->r->y) )
              {
               gfx_hide(slot[i]->o[2]);
               gfx_render();
               gfx_flush();
               sleep(0.5);
               gfx_show(slot[i]->o[2]);
               gfx_render();
              }
            if ((slot[i]->s->row1->z ==
                 prev[1]->r->z)      ||  
                (slot[i]->s->row1->z ==  
                 prev[1]->r->y)      )
              {
               gfx_hide(slot[i]->o[3]);
               gfx_render();
               gfx_flush();
               sleep(0.5);
               gfx_show(slot[i]->o[3]);
               gfx_render();
              }
          break;
          case 2:
            if ((slot[i]->s->row2->x ==
                 prev[1]->r->x)      &&
                (slot[i]->s->row1->x ==
                 prev[1]->r->x)      )  
              {
               gfx_hide(slot[i]->o[1]);
               gfx_hide(slot[i]->o[4]);
               gfx_render();
               gfx_flush();
               sleep(0.5);
               gfx_show(slot[i]->o[1]);
               gfx_show(slot[i]->o[4]);
               gfx_render();
              }
            if ((slot[i]->s->row2->y ==
                 prev[1]->r->y)      &&
                (slot[i]->s->row1->y ==  
                 prev[1]->r->y)      ) 
              {                        
               gfx_hide(slot[i]->o[2]);
               gfx_hide(slot[i]->o[5]);
               gfx_render();
               gfx_flush();
               sleep(0.5);
               gfx_show(slot[i]->o[2]);
               gfx_show(slot[i]->o[5]);
               gfx_render();
              }
            if ((slot[i]->s->row2->z ==
                 prev[1]->r->z)      && 
                (slot[i]->s->row1->z ==   
                 prev[1]->r->z)      ) 
              {
               gfx_hide(slot[i]->o[3]);
               gfx_hide(slot[i]->o[6]);
               gfx_render();
               gfx_flush();
               sleep(0.5);
               gfx_show(slot[i]->o[3]);
               gfx_show(slot[i]->o[6]);
               gfx_render();
              }
            if ((slot[i]->s->row2->y ==  
                 prev[1]->r->z)      &&
                (slot[i]->s->row1->x ==
                 prev[1]->r->z)      )
              { 
               gfx_hide(slot[i]->o[1]);
               gfx_hide(slot[i]->o[5]);
               gfx_render();
               gfx_flush();
               sleep(0.5);
               gfx_show(slot[i]->o[1]);
               gfx_show(slot[i]->o[5]);
               gfx_render();
              }
            if ((slot[i]->s->row2->y ==
                 prev[1]->r->x)      &&
                (slot[i]->s->row1->z ==
                 prev[1]->r->x)      )
              { 
               gfx_hide(slot[i]->o[3]);
               gfx_hide(slot[i]->o[5]);
               gfx_render();
               gfx_flush();
               sleep(0.5);
               gfx_show(slot[i]->o[3]);
               gfx_show(slot[i]->o[5]);
               gfx_render();
              }
          break;
         }
      }
    
    gfx_flush();

   }
}
