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
Eina_List          *pointer_queue = NULL;

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

	p = (int*)evas_object_image_data_get(o, 0);

	xc[0] = ecore_x_cursor_new(shell->win, p, 24, 24, 11, 11);
	xc[1] = ecore_x_cursor_new(shell->win, p+24*24, 24, 24, 11, 11);
	xc[2] = ecore_x_cursor_new(shell->win, p+48*24, 24, 24, 11, 11);
	xc[3] = ecore_x_cursor_new(shell->win, p+72*24, 24, 24, 0, 0);

	evas_object_del(o);
	pointer_push_and_set(10000 + POINTER_NORMAL);
}

void
pointer_move(int x, int y)
{
}

void
pointer_set(long n)
{
    Evas               *e;

    n = n & 15;
    ecore_x_window_cursor_set(shell->win, xc[n]);
}

void
pointer_push_and_set(long n)
{
    pointer_queue = eina_list_append(pointer_queue, (void *)n);
    pointer_set(n);
}

void
pointer_pop(long n)
{
    pointer_queue = eina_list_remove(pointer_queue, (void *)n);
    if (!pointer_queue)
        pointer_push_and_set(POINTER_NORMAL);
    pointer_set((long)pointer_queue->data);
}

long
curr_pointer_get(void)
{
    return curr;
}
