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
   
   Etk_Widget *fm_dialog;
   Etk_Widget *fm_chooser;

   char *file;
};

void main_window_show(char *file);
