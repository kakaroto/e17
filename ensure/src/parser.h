struct enwin;
struct enobj;

Eina_Bool child_data(void *data ensure_unused, Ecore_Fd_Handler *hdlr);
int       parser_save_window(struct enwin *w, FILE *outfile);
int       parser_save_object(struct enobj *o, FILE *outfile);
int       parser_readfile(struct ensure *ensure, const char *file);
