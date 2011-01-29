/*
 * A simple game for testing
 *
 * Copyright 2009 Thomas Horsten
 * Copyright 2009-2011 Mike McCormack
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <Elementary.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#define DEFAULT_WIDTH 10
#define DEFAULT_HEIGHT 20
#define DEFAULT_BLOCKSIZE 20
#define PREVIEW_BLOCKSIZE 7

#define BLACK 0
#define CYAN 1

Evas_Object *board[DEFAULT_WIDTH * DEFAULT_HEIGHT];

int board_width = DEFAULT_WIDTH;
int board_height = DEFAULT_HEIGHT;
int block_size = DEFAULT_BLOCKSIZE;
int interval = 1000;
int piece_orientation = 0;
int next_piece_type = 0;
int piece_type = 0;
int piece_color = CYAN;
int piece_x = 0;
int piece_y = 0;

bool show_next_piece = true;
int preview_pos_x = (DEFAULT_WIDTH * DEFAULT_BLOCKSIZE)-(5*PREVIEW_BLOCKSIZE)-1;
int preview_pos_y = 1;

bool game_over = false;

int rgb_table[8][3] ={
   { 0x00, 0x00, 0x00 },
   { 0x00, 0xf0, 0xf0 },
   { 0xf0, 0xa0, 0x00 },
   { 0x00, 0x00, 0xf0 },
   { 0xa0, 0x00, 0xf0 },
   { 0x00, 0xf0, 0x00 },
   { 0xf0, 0x00, 0x00 },
   { 0xf0, 0xf0, 0x00 },
};

/*
 * The pieces are stored in specially formatted 32-bit integers.
 *
 * The top 8 bits specify the x and y size of the piece (e.g. 0x41 for
 * a 4*1 piece).  The lower (xsize*ysize) bits are a bitmap of the
 * piece.  This is used instead of a fixed size e.g. 4x4, so that
 * rotation can be done correctly relating to the piece size and
 * prevents "wobbling" when rotating the pieces. When displaying,
 * every piece is mapped (and centered) to a 5x5 area.
 */
const unsigned int piece[] = {
   /* straight */
   0x41000000 | 0x0f,
   /* bent left */
   0x23000000 | (0x3 << 4) | (0x1 << 2) | 0x1,
   /* bent right */
   0x23000000 | (0x3 << 4) | (0x2 << 2) | 0x2,
   /* T shaped */
   0x32000000 | (0x2 << 3) | 0x7,
   /* dogleg left */
   0x32000000 | (0x3 << 3) | 0x6,
   /* dogleg right */
   0x32000000 | (0x6 << 3) | 0x3,
   /* square */
   0x2200000f,
};

typedef void *block;

void set_block_color(block b, unsigned long color)
{
   Evas_Object **eo = b;

   if (!eo)
     return;

   evas_object_color_set(*eo, rgb_table[color][0],
             rgb_table[color][1],
             rgb_table[color][2], 255);
}

unsigned long get_block_color(block bl)
{
   Evas_Object **eo = bl;
   int r = 0, g = 0, b = 0, a = 0;
   int i;

   if (!eo)
     return CYAN;
   evas_object_color_get(*eo, &r, &g, &b, &a);

   for (i=0; i<8; i++)
     if (r == rgb_table[i][0] &&
         g == rgb_table[i][1] &&
         b == rgb_table[i][2])
       return i;

   assert(!r && !b && !g);

   return BLACK;
}

block get_block_ptr(int x, int y)
{
   if (x < 0 || x >= board_width)
     return NULL;
   if (y < 0 || y >= board_height)
     return NULL;
   return &board[y*board_width + x];
}

bool block_is_empty(int x, int y)
{
   block b = get_block_ptr(x, y);
   return get_block_color(b) == BLACK;
}

bool piece_has_block(int type, int orientation, int x, int y)
{
   int size_x = (piece[type] & 0xf0000000) >> 28;
   int size_y = (piece[type] & 0x0f000000) >> 24;
   int new_x, new_y;
   bool has_block = false;

   // Center in 5x5 area
   x = (x - (2-(((orientation & 1)?size_y:size_x)>>1)));
   y = (y - (2-(((orientation & 1)?size_x:size_y)>>1)));
   if ((x<0)||(y<0))
     return false;

   if ((orientation & 1) == 0)
     {
        new_x = x;
        new_y = y;
     }
   else
     {
        new_x = size_x-y-1;
        new_y = x;
     }
   if ((orientation & 2) != 0)
     {
        new_x = size_x-1-new_x;
        new_y = size_y-1-new_y;
     }

   if ((new_x < 0) || (new_x >= size_x) || (new_y < 0) || (new_y >= size_y))
     has_block = false;
   else
     has_block = ((piece[type] & ((1<<(size_x*size_y-1)) >> ((new_x + size_x*new_y)))) != 0);
   return has_block;
}

void set_block(int x, int y, int color)
{
   set_block_color(get_block_ptr(x, y), color);
}

void block_at_cursor(int type, int orientation, int color)
{
   int i, j;
   for (i=0; i<5; i++)
     {
        for (j=0; j<5; j++)
          {
             if (!piece_has_block(type, orientation, i, j))
               continue;
             set_block(piece_x + i, piece_y + j, color);
          }
     }
}

bool block_fits_at(int type, int orientation, int x, int y)
{
   int i, j;
   for (i=0; i<5; i++)
     {
        for (j=0; j<5; j++)
          {
             if (!piece_has_block(type, orientation, i, j))
               continue;
             if (!block_is_empty(x + i, y + j))
               return false;
          }
     }
   return true;
}

bool move_to(int x, int y)
{
   if (!block_fits_at(piece_type, piece_orientation, x, y ))
     return false;
   block_at_cursor(piece_type, piece_orientation, BLACK);
   piece_x = x;
   piece_y = y;
   block_at_cursor(piece_type, piece_orientation, piece_color);
   return true;
}

bool move_down(void)
{
   int new_y = piece_y + 1;
   block_at_cursor(piece_type, piece_orientation, BLACK);
   if (block_fits_at(piece_type, piece_orientation, piece_x, new_y))
     piece_y = new_y;
   block_at_cursor(piece_type, piece_orientation, piece_color);
   return (piece_y == new_y);
}

bool drop_down(void)
{
   int rows_dropped = 0;
   while (move_down())
     rows_dropped++;
   return rows_dropped;
}

bool move_left(void)
{
   int new_x = piece_x - 1;
   block_at_cursor(piece_type, piece_orientation, BLACK);
   if (block_fits_at(piece_type, piece_orientation, new_x, piece_y ))
     piece_x = new_x;
   block_at_cursor(piece_type, piece_orientation, piece_color);
   return piece_x == new_x;
}

bool move_right(void)
{
   int new_x = piece_x + 1;
   block_at_cursor(piece_type, piece_orientation, BLACK);
   if (block_fits_at(piece_type, piece_orientation, new_x, piece_y ))
     piece_x = new_x;
   block_at_cursor(piece_type, piece_orientation, piece_color);
   return piece_x == new_x;
}

bool do_rotate(void)
{
   int new_orientation = (piece_orientation + 1) & 3;
   block_at_cursor(piece_type, piece_orientation, BLACK);
   if (block_fits_at(piece_type, new_orientation, piece_x, piece_y ))
     piece_orientation = new_orientation;
   block_at_cursor(piece_type, piece_orientation, piece_color);
   return piece_orientation == new_orientation;
}

unsigned char get_random_number(void)
{
   struct timeval tv;
   unsigned long ticks;
   static unsigned char random_store;

   gettimeofday(&tv, NULL);
   ticks = tv.tv_usec ^ tv.tv_sec;
   random_store ^= (ticks >> 16) & 0xff;
   random_store ^= (ticks >> 8) & 0xff;
   random_store ^= ticks&0xff;
   return random_store;
}

bool new_block(void)
{
   int new_x, new_y, new_orientation;

   /* check if we can place a new block */
   new_x = board_width/2 - 2;
   new_y = 0;
   new_orientation = 0;
   if (!block_fits_at(next_piece_type, new_orientation, new_x, new_y))
     {
        game_over = true;
        return false;
     }

   /* place the new piece */
   piece_type = next_piece_type;
   piece_color = piece_type+1;
   piece_orientation = new_orientation;
   piece_x = new_x;
   piece_y = new_y;

   /* calculate the next piece */
   next_piece_type = get_random_number()%7;

   block_at_cursor(piece_type, piece_orientation, piece_color);

   return true;
}

void clear_board(void)
{
   int i, j;
   for (i=0; i<board_width; i++)
     {
        for (j=0; j<board_height; j++)
          {
             set_block_color(get_block_ptr(i, j), BLACK);
          }
     }
}

bool row_full(int row)
{
   int i;
   for (i=0; i<board_width; i++)
     {
        if (block_is_empty(i, row))
          return false;
     }
   return true;
}

void move_row(int to, int from)
{
   block from_ptr, to_ptr;
   int i;

   if (from == to)
      return;

   for (i=0; i<board_width; i++)
     {
        from_ptr = get_block_ptr(i, from);
        to_ptr = get_block_ptr(i, to);
        if (!to_ptr)
          continue;
        if (from_ptr)
          set_block_color(to_ptr, get_block_color(from_ptr));
        else
          set_block_color(to_ptr, BLACK);
     }
}

unsigned long erase_rows(void)
{
   int row = board_height - 1, collapsed_rows = 0;
   while (row >= collapsed_rows)
     {
        if (row_full(row))
          collapsed_rows ++;
        else
          row--;
        move_row(row, row - collapsed_rows);
     }
   return collapsed_rows;
}

bool do_space(void)
{
   drop_down();
   erase_rows();
   new_block();
   return true;
}

Eina_Bool my_timer(void *data)
{
   move_down();
   return 1;
}

static void
my_keydown(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *ev = event_info;

   if (!strcmp(ev->key, "space") || !strcmp(ev->key, "Down"))
     do_space();
   else if (!strcmp(ev->key, "Left"))
     move_left();
   else if (!strcmp(ev->key, "Right"))
     move_right();
   else if (!strcmp(ev->key, "Up"))
     do_rotate();
}

static void
my_win_del(void *data, Evas_Object *obj, void *event_info)
{
   /* called when my_win_main is requested to be deleted */
   elm_exit(); /* exit the program's main loop that runs in elm_run() */
}

/* this is your elementary main function - it MUST be called IMMEDIATELY
 * after elm_init() and MUSt be passed argc and argv, and MUST be called
 * elm_main and not be static - must be a visible symbol with EAPI infront */
EAPI int
elm_main(int argc, char **argv)
{
   int i, j;
   Evas_Object *win;
   Evas *ev;
   int rw, rh;

   win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   /* set the title of the window - this is in the titlebar */
   elm_win_title_set(win, "Elementary game");

   /* catch keydown events */
   evas_object_focus_set(win, 1);
   evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN, &my_keydown, NULL);

   evas_object_smart_callback_add(win, "delete,request", my_win_del, NULL);
   /* set an initial window size */
   evas_object_resize(win, 320, 480);
   rw = 320/DEFAULT_WIDTH;
   rh = 480/DEFAULT_HEIGHT;

   ev = evas_object_evas_get(win);
   for (i=0; i<DEFAULT_HEIGHT; i++)
     {
        for (j=0; j<DEFAULT_WIDTH; j++)
          {
             Evas_Object **eo = &board[i * DEFAULT_WIDTH + j];
             *eo = evas_object_rectangle_add(ev);
             evas_object_resize(*eo, rw, rh);
             evas_object_move(*eo, (320*j)/DEFAULT_WIDTH, (480*i)/DEFAULT_HEIGHT);
             evas_object_color_set(*eo, 0, 0, 0, 255);
             evas_object_show(*eo);
          }
     }
   /* show the window */
   evas_object_show(win);

   new_block();
   ecore_timer_add(1, &my_timer, NULL);

   elm_run(); /* and run the program now  and handle all events etc. */
   /* if the mainloop that elm_run() runs exist - we exit the app */
   elm_shutdown(); /* clean up and shut down */
   /* exit code */
   return 0;
}
/* all elementary apps should use this. but it right after elm_main() */
ELM_MAIN()
