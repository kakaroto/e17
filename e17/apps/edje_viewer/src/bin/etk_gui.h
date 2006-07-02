#define WINDOW_TITLE "Edje Viewer"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct _Gui Gui;

struct _Gui
{
   Etk_Widget *win;
   Etk_Widget *tree;
   Etk_Widget *canvas;
   Etk_Widget *status;
   Etk_Widget *output;
   Etk_Widget *signal_entry;
   Etk_Widget *source_entry;
   
   Etk_Widget *fm_dialog;
   Etk_Widget *fm_chooser;

   char *path;
};

void main_window_show(char *file);
