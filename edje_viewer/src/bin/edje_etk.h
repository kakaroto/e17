typedef struct _Demo_Edje Demo_Edje;
typedef struct _Collection Collection;

struct _Demo_Edje
{
    Etk_Widget          *mdi_window;
    Etk_Widget          *etk_evas;
    Etk_Tree_Col        *tree_col;
    Etk_Tree_Row        *tree_row;
    Etk_Tree_Row        *part_row;
    Evas_Object         *edje_object;
    Evas_Coord	         minw, minh;
    Evas_Coord           maxw, maxh;
    char                *name;
    void                *data;
};

struct _Collection
{
    char        *file;
    char        *part;

    Demo_Edje   *de;
};

char *ThemeFile;

Evas_List *visible_elements_get(void);
Demo_Edje *edje_part_create(Etk_Tree *output, Etk_Mdi_Area *mdi_area,
	const char *file, char *name);


