#include "main.h"

int theme_read(char *file)
{
 FILE *f;
 char z[512] = "", cmd[128] = "", txt[255] = "";
 static int total_bg = 0, total_icon = 0, 
        total_prevs_check, total_slots_check = 0,
        menu_check = 0, line = 0; 
 int i;

 f = fopen(file, "r");

 if (!f)
   {
    printf("Cannot open %s.\n", file);
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
          prev = (VIEW_PREV **) calloc((total_prevs + 1), sizeof(VIEW_PREV **));
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
          prev[i] = (VIEW_PREV *) malloc(sizeof(VIEW_PREV));
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
          slot = (VIEW_SLOT **) calloc((total_slots + 1), sizeof(VIEW_SLOT **));
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
          slot[i] = (VIEW_SLOT *) malloc(sizeof(VIEW_SLOT));
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
          menu = (VIEW_MENU **) calloc((total_menus + 1), sizeof(VIEW_SLOT **));
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
          menu[i] = (VIEW_MENU *) malloc(sizeof(VIEW_MENU));
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
         }
       else
       if (!strcmp(cmd, "SOUND_MENU"))   
         {
          sscanf(z, "%128s %255s", &cmd, &snd.menu);   
          if (snd.menu[0] == '"')   
            {
             strcpy(snd.menu, strtok(strstr(z, "\""), "\""));   
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
