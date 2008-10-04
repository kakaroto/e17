#define WINDOW_TITLE "Edje Viewer"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef enum _Tree_Search Tree_Search;
typedef struct _Gui Gui;

enum _Tree_Search
{
    TREE_SEARCH_START = 1,
    TREE_SEARCH_NEXT,
    TREE_SEARCH_PREV,
};

struct _Gui
{
   Etk_Widget *win;
   Etk_Widget *tree;
   Etk_Widget *mdi_area;
   Etk_Widget *output;
   Etk_Widget *signal_entry;
   Etk_Widget *source_entry;
   
   Etk_Widget *fm_dialog;
   Etk_Widget *fm_chooser;

   Etk_Popup_Window *popup;
   Etk_Widget *search_entry;

   char *path;
};

void main_window_show(const char *file);
int gui_part_col_sort_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1,
      Etk_Tree_Row *row2, void *data);
