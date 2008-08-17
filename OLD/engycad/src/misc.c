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
#include <math.h>
#include <time.h>

/* vars */
Evas_Object        *o_bg,
    *o_text, *o_butt1, *o_butt2, *o_butt3, *o_label1, *o_label2, *o_label3;

/* protos */
int                 parse_as_polar(char *, float, float, float *, float *);
int                 parse_as_decart(char *, float, float, float *, float *);

/******************/

/* logo stuff */
static void
hide_logo(int val, void *data)
{
/*    Evas_Object         *o;

    o = evas_object_get_named(shell->evas, "logo");
    if (val < 32)
      {
          evas_set_color(shell->evas, o, 255, 255, 255, 255 - val * 8);
          ecore_add_event_timer("hide_logo", 0.05, hide_logo, val + 1, NULL);
      }
    else
      {
          evas_hide(shell->evas, o);
      }
      */
}

void
logo_init(void)
{
/*    Evas_Object         o;
    double              x, y, w, h;
    
    o = evas_object_get_named(shell->evas, "logo");
    if (!o)
    {
	char *s;
	char buf[4096];
	int res;
	
	E_DB_STR_GET (shell->rcfile, "/logoimage", s, res);
	ENGY_ASSERT (res);
	if (s && s[0] != '/')
	{
	    snprintf (buf, 4096, "%s/%s", shell->home, s);
	    FREE (s);
	    s = DUP (buf);
	}
	o = evas_add_image_from_file (shell->evas, s);
	FREE (s);
	evas_object_set_name(shell->evas, o, "logo");
	evas_set_layer(shell->evas, o, 20);
	evas_set_pass_events(shell->evas, o, 1);
    }
    evas_get_geometry(shell->evas, o, &x, &y, &w, &h);
    evas_set_color(shell->evas, o, 255, 255, 255, 255);
    evas_move(shell->evas, o, (shell->w - h) / 2, (shell->h - h) / 2);
    evas_show(shell->evas, o);
    ecore_add_event_timer("hide_logo", 3, hide_logo, 0, NULL);
    */
}

/* iconv stuff */
/*
char               *
my_iconv(iconv_t dcd, char *s)
{
    int                 a, b, tmp;
    char               *buf;
    char               *res;
    char               *p1, *p2;

    if ((int)dcd == -1)
        return NULL;
    if (!s)
        return NULL;
    a = strlen(s);
    ENGY_ASSERTS((a > 4000) ? 0 : 1, "long string for iconv");
    b = a + 1;

    buf = (char *)malloc(b);
    memset(buf, 0, b);
    ENGY_ASSERTS(buf, "malloc");
    if (!buf)
        return NULL;

    p1 = s;
    p2 = buf;
    tmp = iconv(dcd, &p1, &a, &p2, &b);
    res = (char *)malloc(strlen(buf) + 1);
    ENGY_ASSERTS(res, "malloc");

    strcpy(res, buf);

    FREE(buf);
    ENGY_ASSERTS(((strlen(res) > 4001) ? 0 : 1), "it's impossible!");
    return res;
}


void
my_evas_init(void)
{
	imlib_context_set_TTF_encoding(shell->charmap);
}

Evas_Object *
my_evas_add_text(Evas *e, char *font, int size, char *text)
{
    Evas_Object         *o_tmp;
    Evas_Object         *res;
    char                buf[4096];
    DATA32             *p;

    long                w, h;
    int                 a, d;

    Imlib_Image         im_prev, im_curr;
    Imlib_Font          fo_prev, fo_curr;

    im_prev = imlib_context_get_image();
    fo_prev = imlib_context_get_font();

    sprintf(buf, "%s/%d", font, size);

    fo_curr = imlib_load_font(buf);

    imlib_context_set_font(fo_curr);

    imlib_get_text_size(text, &w, &h);
    h += 2;
    w += 2;
    if ((!text) || !strlen(text))
      {
          w = 1;
          h = 1;
      }

    im_curr = imlib_create_image(w, h);
    a = imlib_get_maximum_font_ascent();
    d = imlib_get_maximum_font_descent();
    imlib_context_set_image(im_curr);
    imlib_image_set_has_alpha(1);
    imlib_image_clear();

    imlib_text_draw(1, 1, text);
    printf("text draw: %s\n", text);

    p = imlib_image_get_data_for_reading_only();

    res = evas_object_image_add(e);
    evas_object_image_size_set(res, w, h);
    evas_object_image_fill_set(res, 0, 0, w, h);
    evas_object_image_data_copy_set(res, p);
    evas_object_resize(res, w, h);
    evas_object_color_set(res, 255, 255, 255, 255);

    evas_object_data_set(res, "font", (void *)DUP(buf));
    evas_object_data_set(res, "text", (void *)DUP(text));
    evas_object_data_set(res, "w", (void *)w);
    evas_object_data_set(res, "h", (void *)h);

    imlib_context_set_image(im_curr);
    imlib_free_image();
    imlib_free_font();

    imlib_context_set_image(im_prev);
    imlib_context_set_font(fo_prev);

    return res;
}

void
my_evas_text_get_max_ascent_descent(Evas *e,
                                    Evas_Object *o, double *as, double *ds)
{

    
    Imlib_Font          fo_prev, fo_curr;
    char               *s;
    int                 a, d;

    if (!as)
        return;
    if (!ds)
        return;
    if (!e)
        return;
    if (!o)
        return;
    fo_prev = imlib_context_get_font();
    s = evas_object_data_get(o, "font");

    if (!s)
      {
          *as = 0;
          *ds = 0;
          return;
      }

    fo_curr = imlib_load_font(s);
    imlib_context_set_font(fo_curr);
    a = imlib_get_maximum_font_ascent();
    d = imlib_get_maximum_font_descent();
    imlib_free_font();
    imlib_context_set_font(fo_prev);

    *as = (double)a;
    *ds = (double)d;
    
}

int
my_evas_get_text_width(Evas *e, Evas_Object *o)
{
    int                 res = 0;

    if (!e)
        return 0;
    if (!o)
        return 0;
    res = (int)evas_object_data_get(o, "w");
    return res;
}

void
my_evas_set_text(Evas *e, Evas_Object *o, char *text)
{
    Imlib_Image         im_prev, im_curr;
    Imlib_Font          fo_prev, fo_curr;
    DATA32             *p;
    char               *s;
    int                 w, h;

    if (!e)
        return;
    if (!o)
        return;

    s = evas_object_data_get(o, "text");
    if (!s)
        s = DUP("");
    if (!text)
        text = DUP("");
    if (!strcmp(s, text))
        return;

    s = evas_object_data_get(o, "font");
    if (!s)
        return;
    fo_prev = imlib_context_get_font();
    fo_curr = imlib_load_font(s);
    ENGY_ASSERT(fo_curr);
    imlib_context_set_font(fo_curr);

    imlib_get_text_size(text, &w, &h);
    w += 2;
    h += 2;
    if ((!text) || (!strlen(text)))
      {
          w = 1;
          h = 1;
      }

    im_curr = imlib_create_image(w, h);
    imlib_context_set_image(im_curr);
    imlib_image_set_has_alpha(1);
    imlib_image_clear();

    imlib_text_draw(0, 1, text);

    p = imlib_image_get_data_for_reading_only();

    evas_object_image_fill_set(o, 0, 0, w, h);
    evas_object_image_size_set(o, w, h);
    evas_object_image_data_copy_set(o, p);
    evas_object_color_set(o, 255, 255, 255, 255);
    evas_object_data_set(o, "w", (void *)w);
    evas_object_data_set(o, "h", (void *)h);
    evas_object_data_set(o, "text", DUP(text));

    imlib_context_set_image(im_curr);
    imlib_free_image();
    imlib_free_font();
    imlib_context_set_image(im_prev);
    imlib_context_set_font(fo_prev);
}

void
my_evas_resize(Evas *e, Evas_Object *o, double w, double h)
{
    evas_object_resize(o, w, h);
    evas_object_image_fill_set(o, 0, 0, w, h);
}
*/
/* name generator */
char               *
generate_name(void)
{
    char                buf[400];
    int                 i, j;

    struct timeval      tv;

    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);
    i = rand();
    j = rand();
    sprintf(buf, "%x%x", i, j);
    return (char *)DUP(buf);
}

/* string funcs */
char               *
get_rid_of_right_spaces(char *s)
{
    if (!s)
        return;
    while (strlen(s) && s[strlen(s) - 1] == ' ')
      {
          s[strlen(s) - 1] = 0;
      }
    return s;
}

char               *
get_rid_of_left_spaces(char *s)
{
    int                 pos = 0;
    char               *res;

    if (!s)
        return;

    while (s[pos] && s[pos] == ' ')
        pos++;
    res = DUP(s + pos);
    FREE(s);
    return res;
}
char               *
get_rid_of_extra_spaces(char *s)
{
    return get_rid_of_right_spaces(get_rid_of_left_spaces(s));
}

/* get_values */
/* return value:
 * 2 correct and OK
 * 1 incorrect and not parsed
 * 0 exit keyword found
 * --------------- */
int
get_values(char *s, float x1, float y1, float *x, float *y)
{
    int                 res;

    if (!s)
        return 1;
    if (strlen(s) == 0)
      {
          *x = 0.0;
          return 1;
      }
    s = get_rid_of_extra_spaces(s);

    if (!strcmp(s, "done") ||
        !strcmp(s, "cancel") ||
        !strcmp(s, _("done")) || !strcmp(s, _("cancel")))
      {
          FREE(s);
          return 0;
      }

    if (strchr(s, '<'))
      {
          return parse_as_polar(s, x1, y1, x, y);
      }
    else
      {
          return parse_as_decart(s, x1, y1, x, y);
      }
}

int
parse_as_decart(char *s, float x1, float y1, float *x, float *y)
{
    char               *fig;
    int                 res;
    float               lx, ly;

    if (!s)
        return 1;

    if (s[0] == '@')
      {
          fig = s + 1;
      }
    else
      {
          fig = s;
          x1 = 0;
          y1 = 0;
      }

    res = sscanf(fig, "%f %f", &lx, &ly);
    if (res == 2)
      {
          *x = lx + x1;
          *y = ly + y1;
          FREE(s);
          return res;
      }
    else
      {
          *x = lx;
          FREE(s);
          return 1;
      }
}

int
parse_as_polar(char *s, float x1, float y1, float *x, float *y)
{
    float               len = 0.0;
    float               ang = 0.0;
    int                 res1, res2;
    char               *pos1, *pos2;

    if (!s)
        return 1;

    pos2 = (char *)strchr(s, '<');
    if (!pos2)
      {
          FREE(s);
          return 1;
      }
    if (!pos2[1])
      {
          FREE(s);
          return 1;
      }
    pos2[0] = 0;
    pos2++;
    res1 = sscanf(pos2, "%f", &ang);
    if (res1 != 1)
      {
          FREE(s);
          return 1;
      }
    ang = ang / 180 * M_PI;

    pos1 = (char *)strchr(s, '@');
    if (!pos1)
      {
          pos1 = s;
          x1 = 0;
          y1 = 0;
      }
    else
      {
          pos1++;
      }
    if (!pos1[0])
      {
          FREE(s);
          return 1;
      }
    res2 = sscanf(pos1, "%f", &len);
    if (res1 != 1)
      {
          FREE(s);
          return 1;
      }

    *x = len * cos(ang) + x1;
    *y = len * sin(ang) + y1;
    FREE(s);
    return 2;
}

/** aliases **/

Evas_Hash *aliases;

Evas_Bool my_pair_free(Evas_Hash *hash, 
		const char *key, void *data, void *fdata)
{
	FREE(data);
}

void alias_init()
{
	char buf[4096];
	char *al;
	FILE *f;

	sprintf(buf,"%s/%s", shell->home, shell->aliases);
	f = fopen(buf, "r");

	if(!f)
		return;
	while(!feof(f))
	{
		fgets(buf, 4095, f);
		if(!buf[0])
			continue;
		buf[strlen(buf)-1] = '\0';
		al = strchr(buf, '=');
		if(!al)
			continue;
		al[0] = '\0';
		al++;
		aliases = evas_hash_add(aliases, buf, DUP(al));
	}

	fclose(f);
}

void alias_shutdown()
{
	evas_hash_foreach(aliases, my_pair_free, NULL);
	evas_hash_free(aliases);
}

char               *
_alias(char *s)
{
	char *res;
	res = (char*) evas_hash_find(aliases, s);

	if(res)
		return DUP(res);

	return NULL;
}

/* screen-to-world */
/* hope simple reading is threadsafe... */
double
s2w_x(int _x)
{
    double              x;

    if (!drawing)
        return 0.0;
    x = _x / drawing->scale - drawing->x;
    return x;
}

double
s2w_y(int _y)
{
    Evas_Coord          ex, ey, ew, eh;
    double              y;

    if (!shell)
        return 0.0;
    if (!shell->evas)
        return 0.0;
    if (!drawing)
        return 0.0;
    evas_output_viewport_get(shell->evas, &ex, &ey, &ew, &eh);
    y = -(_y - eh - ey) / drawing->scale - drawing->y;
    return y;
}

double
w2s_x(double _x)
{
    double              x;

    if (!drawing)
        return 0.0;
    x = ((_x + drawing->x) * drawing->scale);
    return x;
}
double
w2s_y(double _y)
{
    Evas_Coord          ex, ey, ew, eh;
    double              y;

    if(!shell) return 0.0;
    if(!shell->evas) return 0.0;
    if(!drawing) return 0.0;

    evas_output_viewport_get(shell->evas, &ex, &ey, &ew, &eh);

    y = ((_y + drawing->y) * drawing->scale);
    y = ey + eh - y;
    return y;
}

/* ipc */
void
my_run(char *s)
{
    system(s);
}
/* perfomance testing */
void
timer(void)
{
    static double       t = 0;
    double              tmp;

    tmp = ecore_time_get();
    printf("timer: %f\n", tmp - t);
    t = ecore_time_get();
}



Evas_Object *evas_image_load(char *file)
{
	Evas_Object * res;
	int w, h;

	res = evas_object_image_add(shell->evas);
	evas_object_image_file_set(res, file, NULL);
	evas_object_image_size_get(res, &w, &h);
	evas_object_image_fill_set(res, 0.0, 0.0, (Evas_Coord)w, (Evas_Coord)h);
	evas_object_resize(res, (Evas_Coord)w, (Evas_Coord)h);

	return res;
}
