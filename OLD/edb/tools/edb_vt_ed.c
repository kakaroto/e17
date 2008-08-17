#include "../src/Edb.h"
#include <signal.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define  YELLOW_ON_BLUE 10
#define  RED_ON_BLUE    11
#define  BLACK_ON_BLUE  12
#define  WHITE_ON_BLUE  13
#define  GREEN_ON_BLUE  14

#define  MARKED         15

char* exit_reason = 0;

WINDOW*    app_win;
WINDOW*    scrl_win;
E_DB_File* db;
char**     keys;
int        keys_num;
int        marked = 0;
int        first = 0;

static void the_end(int sig);
static int sort_compare(const void *v1, const void *v2);

void init_screeen();
void usage();
void draw_item( int row, int item );
void draw_items();
void draw_edit_item();

char* strpad( char* str , int len);

static void the_end(int sig)
{
   endwin();
   /* do your non-curses wrapup here */
   if(exit_reason != 0)
     {
       printf("%s",exit_reason);
     }
}

int
main(int argc, char **argv)
{
  int i;
  char *dbfile = NULL;
  
  for(i=1; i < argc; i++)
    {
      if ( !strcmp(argv[i], "-h") ||
           !strcmp(argv[i], "-help") ||
           !strcmp(argv[i], "--h") ||
           !strcmp(argv[i], "--help") )
      {
        usage();
      }
      else
      { 
        dbfile = argv[i];
      }
    }

  if( dbfile == NULL )
    {
      printf("\nNo file specified!");
      usage();
    }
  
  db = e_db_open(dbfile);

  if (!db)
    {
      printf("Database file %s cannot be opened!\n", dbfile);
      exit(-1);
    }
  
  keys = e_db_dump_key_list(dbfile, &keys_num);
  qsort(keys, keys_num, sizeof(char *), sort_compare);
  
  signal(SIGINT, the_end);
   
  app_win = initscr();

  if(COLS < 80 || LINES < 24)
    {
      strcpy(exit_reason, "Needs a terminal with a resolution of at least 80x24\n");
      the_end(0);
      e_db_flush();
      exit(0);
    }

  nonl();
  cbreak();
  noecho();
  curs_set(0);
  if (has_colors())
    {
      start_color();
      
      init_pair(COLOR_BLACK,   COLOR_BLACK,   COLOR_BLACK);
      init_pair(COLOR_GREEN,   COLOR_GREEN,   COLOR_BLACK);
      init_pair(COLOR_RED,     COLOR_RED,     COLOR_BLACK);
      init_pair(COLOR_CYAN,    COLOR_CYAN,    COLOR_BLACK);
      init_pair(COLOR_WHITE,   COLOR_WHITE,   COLOR_BLACK);
      init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
      init_pair(COLOR_BLUE,    COLOR_BLUE,    COLOR_BLACK);
      init_pair(COLOR_YELLOW,  COLOR_YELLOW,  COLOR_BLACK);
      
      init_pair(YELLOW_ON_BLUE, COLOR_YELLOW, COLOR_BLUE);
      init_pair(RED_ON_BLUE,    COLOR_RED,    COLOR_BLUE);
      init_pair(BLACK_ON_BLUE,  COLOR_BLACK,  COLOR_BLUE);
      init_pair(WHITE_ON_BLUE,  COLOR_WHITE,  COLOR_BLUE);
      init_pair(GREEN_ON_BLUE,  COLOR_GREEN,  COLOR_BLUE);
      init_pair(MARKED,         COLOR_BLACK,  COLOR_GREEN);
      
    }
  
  init_screeen();
  
  for (;;)
  {
    int c;
    char str[40];

    draw_items();
    draw_edit_item();
    
    refresh();
    wrefresh(app_win);
    wrefresh(scrl_win);
    
    c = getch();
    
    if( c == 113 ) break; 

    switch(c)
    {
      case 107:
        marked--;
        break;
      case 106:
        marked++;
        break;
    }
    

    if( marked >= keys_num ) marked = keys_num - 1;
    if( marked < 0 )marked = 0;

    if( first > marked ) first = marked;
    if( first+9 < marked ) first = marked - 9;
    
    sprintf(str,"%8d %8d %8d %8d", c, keys_num, first, marked );
    mvwprintw(app_win, 23, 0, str);
  }
  exit_reason = 0; 
  the_end(0);

  e_db_flush();
 
  return 0;
}

void draw_item( int row, int item )
{
  char buffer[80] = "";
  char tdata[1024]  = "";
  char* type = NULL;
  char* t;
  

  if(item >= keys_num) return;
  
  if( item == marked )
    {
      wcolor_set(scrl_win, MARKED, NULL);
    }
  else
    {
      wcolor_set(scrl_win, WHITE_ON_BLUE, NULL);
    }

  type = e_db_type_get(db, keys[item]);
  if (type) t = type;
  else t = "?";
  strcat(buffer, t);
  strpad(buffer, 6);
  if( !strcmp(t,"int") )
    {
      int data;
      if (e_db_int_get(db, keys[item], &data))
        {
          sprintf(tdata, "%i", data);
        }
    }
  else if( !strcmp(t, "float") )
    {
      float data;
      if (e_db_float_get(db, keys[item], &data))
        {
          sprintf(tdata, "%f", data);
        }
    }
  else if( !strcmp(t, "str") )
    {
      char* data;
      char* ptr;
      if ( (data = e_db_str_get(db, keys[item])) )
        {
          ptr = data;
          while( *ptr != 0 )
          {
            if(*ptr == '\n' ) *ptr = 1;
            *ptr++;
          }
          strncpy(tdata, data, 32);
          if( strlen(data) > 32 ) strcat(tdata,"...");
          free(data);
        }
      
    }
  else 
    {
      sprintf(tdata, "Unknown type or binary data.");
    }
  if (type) free(type);
  
  strcat(buffer, keys[item]);
  strpad(buffer,43);

  strcat(buffer,tdata);
  strpad(buffer,79);

  mvwprintw(scrl_win, row, 0, buffer );

}

char* strpad( char* str , int len)
{
  str[len-1] = 0;
  while( strlen(str) < len )
  {
    str[strlen(str)+1] = 0;
    str[strlen(str)] = 32;
  }
}

void init_screeen()
{
  /* Creates two windows.  */
  
  /* 1. Main window */
  app_win = newwin(24,80, (LINES-24)/2, (COLS-80)/2);
  keypad(app_win, TRUE);
  wbkgd(app_win, (chtype)(' ' + (WHITE_ON_BLUE << 8)));
  
  /* 2. Data window */
  scrl_win = newwin(10,79, ((LINES-24)/2)+3, (COLS-80)/2);
  keypad(scrl_win, TRUE);
  wbkgd(scrl_win, (chtype)(' ' + (YELLOW_ON_BLUE << 8)));
  
  /* Draws the top of the editor */
  wcolor_set(app_win, WHITE_ON_BLUE, NULL);
  mvwprintw(app_win, 0, 34, "E DB Editor");
  mvwprintw(app_win, 1,0, "TYPE  "
            "KEY                                  "
            "VALUE                                ");
  mvwprintw(app_win, 2,0,  "----------------------------------------"
                           "----------------------------------------");
  mvwprintw(app_win, 13,0, "----------------------------------------"
                           "----------------------------------------");
}

void usage()
{
  printf("\nUsage: edb_vt_ed database_file.db\n\n");
  exit(0);
}

void draw_items()
{
  int   i;
    
  for(i=0; i < 10; i++)
    {
      draw_item(i, i+first);
    }
}

static int sort_compare(const void *v1, const void *v2)
{
   return strcmp(*(char **)v1, *(char **)v2);
}

void draw_edit_item()
{
  char buffer[80] = "";
  char tdata[1024]  = "";
  char* type = NULL;
  int t;
  
  wcolor_set(app_win, YELLOW_ON_BLUE, NULL);
  mvwprintw(app_win, 14,0, "Name:");

  type = e_db_type_get(db, keys[marked]);

  if(type)
    {
      if( !strcmp( type, "int") )
        {  
          int data;
          if (e_db_int_get(db, keys[marked], &data))
            {
              sprintf(tdata, "%i", data);
            }
          t = 0;
        }
      else if( !strcmp( type, "str") ) 
        {
          char* data;
//          char* ptr;
          if ( (data = e_db_str_get(db, keys[marked])) )
            {
//              ptr = data;
//              while( *ptr != 0 )
//                {
//                  if(*ptr == '\n' ) *ptr = 1;
//                  *ptr++;
//               }
              sprintf(tdata, "%s", data);
              free(data);
            }
          t = 1; 
        }
      else if( !strcmp( type, "float") ) 
        { 
          float data;
          if (e_db_float_get(db, keys[marked], &data))
            {
              sprintf(tdata, "%f", data);
            }
          t = 2; 
        }
    }
  else
  {
    t = 3;
  }

  strcpy(buffer,"Integer ( )");
  strpad(buffer,20);

  strcat(buffer,"String ( )");
  strpad(buffer,40);
  
  strcat(buffer,"Float ( )");
  strpad(buffer,60);

  strcat(buffer,"Binary ( )");
  strpad(buffer,80);

  mvwprintw(app_win, 15,0, buffer);

  mvwprintw(app_win, 16,0, "Data:");

  wcolor_set(app_win, WHITE_ON_BLUE, NULL);

  sprintf(buffer,"%s",keys[marked]);
  strpad(buffer,74);
  mvwprintw(app_win, 14,6, buffer);

  strpad(tdata,1023);
  mvwprintw(app_win, 16, 6, tdata);

  switch(t)
  {
    case 0:
      mvwprintw(app_win, 15,9, "*");
      mvwprintw(app_win, 15,28, " ");
      mvwprintw(app_win, 15,47, " ");
      mvwprintw(app_win, 15,68, " ");
      break;
    case 1:
      mvwprintw(app_win, 15,9, " ");
      mvwprintw(app_win, 15,28, "*");
      mvwprintw(app_win, 15,47, " ");
      mvwprintw(app_win, 15,68, " ");
      break;
    case 2:
      mvwprintw(app_win, 15,9, " ");
      mvwprintw(app_win, 15,28, " ");
      mvwprintw(app_win, 15,47, "*");
      mvwprintw(app_win, 15,68, " ");
      break;
    case 3:
      mvwprintw(app_win, 15,9, " ");
      mvwprintw(app_win, 15,28, " ");
      mvwprintw(app_win, 15,47, " ");
      mvwprintw(app_win, 15,68, "*");
      break;
    default:
      mvwprintw(app_win, 15,9, " ");
      mvwprintw(app_win, 15,28, " ");
      mvwprintw(app_win, 15,47, " ");
      mvwprintw(app_win, 15,68, " ");
      break;
  }

/*  if( !strcmp(t,"int") )
    {
      int data;
      if (e_db_int_get(db, keys[item], &data))
        {
          sprintf(tdata, "%i", data);
        }
    }
  else if( !strcmp(t, "float") )
    {
      float data;
      if (e_db_float_get(db, keys[item], &data))
        {
          sprintf(tdata, "%f", data);
        }
    }
  else if( !strcmp(t, "str") )
    {
      char* data;
      char* ptr;
      if ( (data = e_db_str_get(db, keys[item])) )
        {
          ptr = data;
          while( *ptr != 0 )
          {
            if(*ptr == '\n' ) *ptr = 1;
            *ptr++;
          }
          sprintf(tdata, "%s", data);
        }
      free(data);
    }
  else 
    {
      sprintf(tdata, "Unknown type or binary data.");
    }
  if (type) free(type);
  
  strcat(buffer, keys[item]);
  strpad(buffer,43);

  strcat(buffer,tdata);
  strpad(buffer,79);*/
}
