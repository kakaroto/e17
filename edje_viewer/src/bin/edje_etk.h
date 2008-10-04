typedef struct _Demo_Edje Demo_Edje;
typedef struct _Collection Collection;

struct _Demo_Edje
{
    Etk_Widget          *mdi_window;
    Etk_Widget          *etk_evas;
    Etk_Tree_Col        *tree_col;
    Etk_Tree_Row        *tree_row;
    Evas_Coord	         minw, minh;
    Evas_Coord           maxw, maxh;
    char                *name;
    void                *data;
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
Demo_Edje *edje_part_create(Etk_Tree *output, Etk_Mdi_Area *mdi_area,
	const char *file, char *name);


