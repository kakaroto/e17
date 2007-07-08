/*
 * Copyright (C) 2002, Yury Hudobin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "engy.h"

/* vars */
//Evas_Object        *op[4][6];
Ecore_X_Cursor xc[4];
Pointer             curr;
int                 pointer_x, pointer_y;
Evas_List          *pointer_queue = NULL;

void
pointer_init(void)
{


	int *p;
	char *s;
	int i;

	Evas_Object *o;
        E_DB_STR_GET(shell->rcfile, "/pointer-pointer", s, i);
	ENGY_ASSERT(i);
        if( s[0] !='/'){
	    char buf[4096];
	    snprintf(buf, 4096,"%s/%s", shell->home,s);
	    o = evas_image_load(buf);
	    FREE(s);
	} else {
	    o = evas_image_load(s);
	    FREE(s);
	}

//	o = evas_image_load("engy.png");

	p = (int*)evas_object_image_data_get(o, 0);

	xc[0] = ecore_x_cursor_new(shell->win, p, 24, 24, 11, 11);
	xc[1] = ecore_x_cursor_new(shell->win, p+24*24, 24, 24, 11, 11);
	xc[2] = ecore_x_cursor_new(shell->win, p+48*24, 24, 24, 11, 11);
	xc[3] = ecore_x_cursor_new(shell->win, p+72*24, 24, 24, 0, 0);

	evas_object_del(o);
/*
    Evas               *e;
    int                 i, j;
    char               *s;

    e = shell->evas;

    memset(op, 0, sizeof(op));

    for (i = 0; i < 6; i++)
        op[0][i] = evas_object_line_add(e);
    for (i = 0; i < 6; i++)
        evas_object_color_set(op[0][i], 120, 120, 120, 255);

    for (i = 0; i < 2; i++)
        op[2][i] = evas_object_line_add(e);
    for (i = 0; i < 2; i++)
        evas_object_color_set(op[2][i], 120, 120, 120, 255);

    E_DB_STR_GET(shell->rcfile, "/pointer-pointer", s, i);
    ENGY_ASSERT(i);
    if( s[0] !='/'){
	char buf[4096];
	snprintf(buf, 4096,"%s/%s", shell->home,s);
	op[1][0] = evas_image_load(buf);
	FREE(s);
    } else {
	op[1][0] = evas_image_load(s);
	FREE(s);
    }
    E_DB_STR_GET(shell->rcfile, "/pointer-hand", s, i);
    ENGY_ASSERT(i);
    if( s[0] !='/'){
	char buf[4096];
	snprintf(buf, 4096,"%s/%s", shell->home,s);
	op[3][0] = evas_image_load(buf);
	FREE(s);
    } else {
	op[3][0] = evas_image_load(s);
	FREE(s);
    }
    
    for (i = 0; i < 6; i++)
        for (j = 0; j < 4; j++)
          {
              evas_object_layer_set(op[j][i], 30);
              evas_object_pass_events_set(op[j][i], 1);
          }
	  */
    pointer_push_and_set(10000 + POINTER_NORMAL);
}

void
pointer_move(int x, int y)
{
	/*
    Evas               *e;

    e = shell->evas;
    pointer_x = x;
    pointer_y = y;
    switch (curr)
      {
      case POINTER_NORMAL:
          evas_object_line_xy_set(op[0][0], x - 20, y, x + 20, y);
          evas_object_line_xy_set(op[0][1], x, y - 20, x, y + 20);
          evas_object_line_xy_set(op[0][2], x - 4, y - 4, x + 4, y - 4);
          evas_object_line_xy_set(op[0][3], x + 4, y - 4, x + 4, y + 4);
          evas_object_line_xy_set(op[0][4], x + 4, y + 4, x - 4, y + 4);
          evas_object_line_xy_set(op[0][5], x - 4, y + 4, x - 4, y - 4);
          break;
      case POINTER_CROSS:
          evas_object_line_xy_set(op[2][0], x - 20, y, x + 20, y);
          evas_object_line_xy_set(op[2][1], x, y - 20, x, y + 20);
//		    evas_object_line_xy_set(op[2][2], x+4, y+4, x-4, y+4);
//		    evas_object_line_xy_set(op[2][3], x-4, y+4, x-4, y-4);
          break;
      case POINTER_HAND:
          evas_object_move(op[3][0], x - 16, y - 16);
          break;
      case POINTER_ARROW:
          evas_object_move(op[1][0], x, y);
          break;
      }*/
}

void
pointer_set(int n)
{
    Evas               *e;
    int                 i, j;

    n = n & 15;
/*
    e = shell->evas;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 6; j++)
            evas_object_hide(op[i][j]);

    for (i = 0; i < 6; i++)
        evas_object_show(op[n][i]);
    curr = n;
    pointer_move(pointer_x, pointer_y);
    */
    ecore_x_window_cursor_set(shell->win, xc[n]);
}

void
pointer_push_and_set(int n)
{
    pointer_queue = evas_list_append(pointer_queue, (void *)n);
    pointer_set(n);
}

void
pointer_pop(int n)
{
    pointer_queue = evas_list_remove(pointer_queue, (void *)n);
    if (!pointer_queue)
        pointer_push_and_set(POINTER_NORMAL);
    pointer_set((int)pointer_queue->data);
}

int
curr_pointer_get(void)
{
    return curr;
}
