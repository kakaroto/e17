#include "main.h"

void  LoadEDJ     (char *file);
int   SaveEDJ     (char *file_name);
int   Decompile   (void *data); //data is the name of the file to open
int   exe_data    (void *data, int ev_type, void *ev);
int   exe_exit    (void *data, int ev_type, void *ev);
void  stop_bar    (Etk_Widget * pbar);
