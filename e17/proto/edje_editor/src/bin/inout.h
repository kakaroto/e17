#include "main.h"

void  LoadEDJ        (char *file);
int   SaveEDJ        (char *file_name);
int   Decompile      (void *data); //data is the name of the file to open
int   pipe_data      (void *data, int ev_type, void *ev);
int   load_pipe_exit (void *data, int ev_type, void *ev);
int   save_pipe_exit (void *data, int ev_type, void *ev);
void  stop_bar       (Etk_Widget *pbar, char *text);
