#include "main.h"

void theme_set_defaults(void)
{
 int i;

 text_color.r = 255;
 text_color.g = 255;
 text_color.b = 255;
 text_color.a = 70;

 prev = (GAME_PREV **) calloc((total_prevs + 1), sizeof(GAME_PREV **));
 slot = (GAME_SLOT **) calloc((total_slots + 1), sizeof(GAME_SLOT **));

 for (i = 1; i <= total_prevs; i++)
   prev[i] = (GAME_PREV *) malloc(sizeof(GAME_PREV));

 for (i = 1; i <= total_slots; i++)
   slot[i] = (GAME_SLOT *) malloc(sizeof(GAME_SLOT));
 
 prev[1]->coords.x = 40;
 prev[1]->coords.y = 100;
 prev[1]->color.r = 255;
 prev[1]->color.g = 255;
 prev[1]->color.b = 255;
 prev[1]->color.a = 40;
 
 slot[1]->coords.x = 130;
 slot[1]->coords.y = 100;
 slot[1]->color.r = 255;
 slot[1]->color.g = 255;
 slot[1]->color.b = 255;
 slot[1]->color.a = 20;

 slot[2]->coords.x = 290;
 slot[2]->coords.y = 100;
 slot[2]->color.r = 255;
 slot[2]->color.g = 255;
 slot[2]->color.b = 255;
 slot[2]->color.a = 20;

 slot[3]->coords.x = 450;
 slot[3]->coords.y = 100;
 slot[3]->color.r = 255;
 slot[3]->color.g = 255;
 slot[3]->color.b = 255;
 slot[3]->color.a = 20;

 menu_color.r = 229;
 menu_color.g = 229;
 menu_color.b = 229;
 menu_color.a = 100;

 menu = (GAME_MENU **) calloc((total_menus + 1), sizeof(GAME_SLOT **)); 

 for (i = 1; i <= total_menus; i++)
   menu[i] = (GAME_MENU *) malloc(sizeof(GAME_MENU));

 strcpy(menu[1]->font.name, "cinema");
 menu[1]->font.size = 12;
 strcpy(menu[1]->text, "Rehash");
 menu[1]->x = 10;
 menu[1]->color.r = 0;
 menu[1]->color.g = 0;
 menu[1]->color.b = 0;
 menu[1]->color.a = 50;

 strcpy(menu[2]->font.name, "cinema");
 menu[2]->font.size = 12;
 strcpy(menu[2]->text, "Quit");
 menu[2]->x = 145;
 menu[2]->color.r = 0;   
 menu[2]->color.g = 0;   
 menu[2]->color.b = 0;  
 menu[2]->color.a = 50;

 points_color.r = 255;
 points_color.g = 255;    
 points_color.b = 255;  
 points_color.a = 70;

 points[1].icon.x = 40;
 points[1].icon.y = 300;
 points[1].exists = 1;
 points[2].icon.x = 40;
 points[2].icon.y = 350;
 points[2].exists = 1;
 points[3].icon.x = 40;
 points[3].icon.y = 400;
 points[3].exists = 1;
 points[4].icon.x = 240;
 points[4].icon.y = 300;
 points[4].exists = 1;
 points[5].icon.x = 240;
 points[5].icon.y = 350;
 points[5].exists = 1;
 points[6].icon.x = 240;
 points[6].icon.y = 400;
 points[6].exists = 1;
 points[7].icon.x = 440;
 points[7].icon.y = 300;
 points[7].exists = 1;
 points[8].icon.x = 440;
 points[8].icon.y = 350;
 points[8].exists = 1;
 points[9].icon.x = 440;
 points[9].icon.y = 400;
 points[9].exists = 1;

 points[1].text.x = 110;
 points[1].text.y = 325;
 points[2].text.x = 110;
 points[2].text.y = 375;
 points[3].text.x = 110;
 points[3].text.y = 425;
 points[4].text.x = 310;
 points[4].text.y = 325;
 points[5].text.x = 310;
 points[5].text.y = 375;
 points[6].text.x = 310;
 points[6].text.y = 425;
 points[7].text.x = 510;
 points[7].text.y = 325;
 points[8].text.x = 510;
 points[8].text.y = 375;
 points[9].text.x = 510;
 points[9].text.y = 425;

 strcpy(points_font.name, "cinema");
 points_font.size = 12;

 strcpy(text.font.name, "cinema");
 text.font.size = 12;
 text.coords.x = 40;
 text.coords.y = 40;

 strcpy(snd.startup, "");
 strcpy(snd.menu, "");
 strcpy(snd.slot, ""); 
}

int theme_read(int argc, char **argv)
{
 FILE *f;
 char z[512] = "", cmd[128] = "", txt[255] = "", *p;
 static int total_bg = 0, total_icon = 0, 
        total_prevs_check, total_slots_check = 0,
        menu_check = 0, line = 0; 
 int i;

 if (argc == 2)
   {
    f = fopen(argv[1], "r");

    if (!f)
      printf("Cannot open %s.\n", argv[1]);
   }
 else
   {
    sprintf(z, "%s/.sletsrc", getenv("HOME"));
    f = fopen(z, "r");
   }

 if (!f)
   {
    theme_set_defaults();
   }
 else
   {
    while(!feof(f))
      {
       strcpy(z,   "");
       strcpy(cmd, "");
       strcpy(txt, "");

       line++;
       fgets(z, 512, f);
       if (z[0] == '#')
         continue;
       sscanf(z, "%128s", &cmd);
       if (!strlen(cmd))
         continue;
      if (!strcmp(cmd, "WINDOW"))
         {
          sscanf(z, "%128s %d %d %128s", &cmd, &win_w, &win_h, &win_title);
          if (win_title[0] == '"')
            {
             strcpy(win_title, strtok(strstr(z, "\""), "\""));
            }
         }
       else
       if (!strcmp(cmd, "BG"))
         {
          sscanf(z, "%128s %255s", &cmd, &txt);
          if (total_bg > 25)
            continue;
          if (!strlen(txt))
            {
             if (total_bg)
               continue; 
             else
               {
                printf("\n%d: %s-> Not enough arguments.\n", line, z);
                return 0;
               }
            }
          if (txt[0] == '"')
            {
             strcpy(txt, strtok(strstr(z, "\""), "\""));
            }
          if (txt[0] == '@')
            {
             p = txt;
             sprintf(z, "%s/%s", DATA_DIR, p+1);
             strcpy(txt, z);
            }
          strcpy(bg_files[total_bg++], txt);
          strcpy(bg_files[total_bg], "");
         }
       else
       if (!strcmp(cmd, "ICON"))
         {
          sscanf(z, "%128s %255s", &cmd, &txt);
          if (total_icon > 8)
            continue;
          if (txt[0] == '"')
            {
             strcpy(txt, strtok(strstr(z, "\""), "\""));
            }
          if (txt[0] == '@')
            {
             p = txt;
             sprintf(z, "%s/%s", DATA_DIR, p+1);
             strcpy(txt, z);
            }
          strcpy(icon_files[total_icon++], txt);
         }
       else
       if (!strcmp(cmd, "FONT_PATH"))
         {
          sscanf(z, "%128s %512s", &cmd, &font_path);
          if (font_path[0] == '"')
            {
             strcpy(font_path, strtok(strstr(z, "\""), "\""));
            }
          if (font_path[0] == '@')
            {
             p = font_path;
             sprintf(z, "%s/%s", DATA_DIR, p+1);
             strcpy(font_path, z);
            }
         }
       else
       if (!strcmp(cmd, "TEXT_COLOR"))
         {
          sscanf(z, "%128s %d %d %d %d", &cmd, &text_color.r, &text_color.g,
                                         &text_color.b, &text_color.a); 
         }
       else
       if (!strcmp(cmd, "TEXT"))
         {
          sscanf(z, "%128s %255s %d %d %d", &cmd, &text.font.name, &text.font.size,
                                            &text.coords.x, &text.coords.y);
          if (text.font.name[0] == '"')
            {
             strcpy(text.font.name, strtok(strstr(z, "\""), "\""));
            }
         }
       else
       if (!strcmp(cmd, "TOTAL_PREVS"))
         {
          total_prevs_check = 1;
          sscanf(z, "%128s %d", &cmd, &total_prevs);
          prev = (GAME_PREV **) calloc((total_prevs + 1), sizeof(GAME_PREV **));
         }
       else
       if (!strcmp(cmd, "PREV"))
         {
          if (!total_prevs_check)
            {
             printf("\n%d: %s-> PREV command while no TOTAL_PREVS specified yet!\n", line, z);
             return 0;
            }
          sscanf(z, "%128s %d", &cmd, &i);
          if (i > total_prevs)
            {
             printf("\n%d: %s-> Only %d prevs allocated.\n", line, z, total_prevs);
             return 0;
            }
          prev[i] = (GAME_PREV *) malloc(sizeof(GAME_PREV));
          sscanf(z, "%128s %d %d %d %d %d %d %d", &cmd, &i,
                                                  &prev[i]->coords.x, &prev[i]->coords.y,
                                                  &prev[i]->color.r,  &prev[i]->color.g,
                                                  &prev[i]->color.b,  &prev[i]->color.a);
          total_prevs_check++;
         }
       else
       if (!strcmp(cmd, "TOTAL_SLOTS"))
         {
          total_slots_check = 1;
          sscanf(z, "%128s %d", &cmd, &total_slots);
          slot = (GAME_SLOT **) calloc((total_slots + 1), sizeof(GAME_SLOT **));
         }
       else
       if (!strcmp(cmd, "SLOT"))
         {
          if (!total_slots_check)
            {
             printf("\n%d: %s-> SLOT command while no TOTAL_SLOTS specified yet!\n", line, z);
             return 0;
            }          
          sscanf(z, "%128s %d", &cmd, &i);
          if (i > total_slots)
            {
             printf("\n%d: %s-> Only %d slots allocated.\n", line, z, total_slots);
             return 0;
            }
          slot[i] = (GAME_SLOT *) malloc(sizeof(GAME_SLOT));
          sscanf(z, "%128s %d %d %d %d %d %d %d", &cmd, &i,
                                                  &slot[i]->coords.x, &slot[i]->coords.y,
                                                  &slot[i]->color.r,  &slot[i]->color.g,
                                                  &slot[i]->color.b,  &slot[i]->color.a);
          total_slots_check++;
         }
       else
       if (!strcmp(cmd, "MENU_COLOR"))
         {
          sscanf(z, "%128s %d %d %d %d", &cmd, &menu_color.r, &menu_color.g,
                                         &menu_color.b, &menu_color.a);          
         }
       else
       if (!strcmp(cmd, "MENU"))
         {
          menu_check = 1;
          sscanf(z, "%128s %d %d %d %d", &cmd, &total_menus, &menu_w, &menu_h, &menu_end);
          menu = (GAME_MENU **) calloc((total_menus + 1), sizeof(GAME_SLOT **));
         }
       else
       if (!strcmp(cmd, "MENU_ITEM"))
         {
          if (!menu_check)
            {
             printf("\n%d: %s-> MENU_ITEM command while no MENU specified yet!\n", line, z);
             return 0;
            }
          sscanf(z, "%128s %d", &cmd, &i);
          if (i > total_menus)
            {
             printf("\n%d: %s-> Only %d menu-items allocated.\n", line, z, total_menus);
             return 0;
            }
          menu[i] = (GAME_MENU *) malloc(sizeof(GAME_MENU));
          sscanf(z, "%128s %d %255s %d %d %d %d %d %d %255s", &cmd, &i,
                                                              &menu[i]->font.name, &menu[i]->font.size, &menu[i]->x,
                                                              &menu[i]->color.r, &menu[i]->color.g,
                                                              &menu[i]->color.b, &menu[i]->color.a,
                                                              &menu[i]->text);
         if (menu[i]->font.name[0] == '"')
            {
             strcpy(menu[i]->font.name, strtok(strstr(z, "\""), "\""));
            }
         if (menu[i]->text[0] == '"')
            {
             strcpy(menu[i]->text, strtok(strstr(z, "\""), "\""));
            }
          menu_check++;
         }
       else
       if (!strcmp(cmd, "POINTS"))
         {
          sscanf(z, "%128s %255s %d %d %d %d %d", &cmd, 
                                                  &points_font.name, &points_font.size,
                                                  &points_color.r, &points_color.g,
                                                  &points_color.b, &points_color.a);
         }
       else
       if (!strcmp(cmd, "POINTS_ICON"))
         { 
          sscanf(z, "%128s %d", &cmd, &i);
          points[i].exists = 1;
          sscanf(z, "%128s %d %d %d", &cmd, &i, &points[i].icon.x, &points[i].icon.y);
         }
       else
       if (!strcmp(cmd, "POINTS_TEXT"))
         {
          sscanf(z, "%128s %d", &cmd, &i);
          points[i].exists = 1;
          sscanf(z, "%128s %d %d %d", &cmd, &i, &points[i].text.x, &points[i].text.y);
         }
       else
       if (!strcmp(cmd, "SOUND_STARTUP"))
         {
          sscanf(z, "%128s %255s", &cmd, &snd.startup);
          if (snd.startup[0] == '"')
            {
             strcpy(snd.startup, strtok(strstr(z, "\""), "\""));
            }
          if (snd.startup[0] == '@')
            {
             p = snd.startup;
             sprintf(z, "%s/%s", DATA_DIR, p+1);
             strcpy(snd.startup, z);
            }
         }
       else
       if (!strcmp(cmd, "SOUND_MENU"))   
         {
          sscanf(z, "%128s %255s", &cmd, &snd.menu);   
          if (snd.menu[0] == '"')   
            {
             strcpy(snd.menu, strtok(strstr(z, "\""), "\""));   
            }
          if (snd.menu[0] == '@')
            {
             p = snd.menu;
             sprintf(z, "%s/%s", DATA_DIR, p+1);
             strcpy(snd.menu, z);
            }
         }
       else
       if (!strcmp(cmd, "SOUND_SLOT"))
         {
          sscanf(z, "%128s %255s", &cmd, &snd.slot);
          if (snd.slot[0] == '"')
            {
             strcpy(snd.slot, strtok(strstr(z, "\""), "\""));
            }
          if (snd.slot[0] == '@')
            {
             p = snd.slot;
             sprintf(z, "%s/%s", DATA_DIR, p+1);
             strcpy(snd.slot, z);
            }
         }
      }

    if (!total_bg)
      {
       printf("\nNo backgrounds found.\n");
       return 0; 
      } 

    if (!total_prevs_check) 
      {
       printf("\nNo TOTAL_PREVS found.\n");
       return 0;
      }
    else
    if ((total_prevs_check - 1) != total_prevs)
      {
       printf("\nNot enough PREVS found.\n");
       return 0;
      }

    if (!total_slots_check)
      { 
       printf("\nNo TOTAL_SLOTS found.\n");
       return 0;
      }
    else 
    if ((total_slots_check - 1) != total_slots)
     {
      printf("\nNot enough SLOTS found.\n");
      return 0;
     }

    if (!menu_check)
      {
       printf("\nNo MENU found.\n");
       return 0;
      }
    else
    if ((menu_check - 1) != total_menus)
     {
      printf("\nNot enough MENU_ITEMS found.\n");
      return 0;
     }

   }

 return 0;
}
