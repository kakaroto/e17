typedef struct _Demo_Edje Demo_Edje;
typedef struct _Collection Collection;

struct _Demo_Edje
{
    Evas_Object *edje;
    Evas_Object *left;
    Evas_Object *right;
    Evas_Object *top;
    Evas_Object *bottom;
    Evas_Object *title;
    Evas_Object *title_clip;
    Evas_Object *image;
    Evas_Coord   minw, minh;
    Evas_Coord   maxw, maxh;
    Evas_Coord   cx, cy, cw, ch;
    int          hdir;
    int          vdir;
    char         down_top : 1;
    char         down_bottom : 1;
    char 	*name;

    Etk_Bool     first_run;
};

struct _Collection
{
    char         header : 1;
    char         clicked : 1;
    Evas_Coord   maxw;
    char        *file;
    char        *part;
    Evas_Object *text;
    Evas_Object *bg;
    Evas_List   *entries;

    Demo_Edje   *de;
};

Evas_List *visible_elements_get(void);
void bg_setup(Etk_Canvas *canvas);
void canvas_resize_cb(Etk_Object *canvas, const char *property_name, 
	void *data);
void list_entries(const char *file, Etk_Tree *tree, Etk_Canvas *canvas);
Demo_Edje *edje_part_create(Etk_Tree *output, Etk_Canvas *canvas,
	const char *file, char *name);
void edje_part_show(Etk_Canvas *canvas, Demo_Edje *de);
void edje_part_hide(Demo_Edje *de);
void edje_part_resize(Demo_Edje *de);


