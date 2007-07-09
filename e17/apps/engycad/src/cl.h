

int engy_cl_init(void);

Evas_Object * cl_new(int opt);


char *my_iconv(iconv_t, const char *);

void cl_text_set(Evas_Object * _o, const char * str);
void cl_text_get(Evas_Object * _o, char ** str);
void cl_cursor_pos_set(Evas_Object * _o, int _pos);
int  cl_cursor_pos_get(Evas_Object * _o);
void cl_cursor_pos_fwd(Evas_Object * _o);
void cl_cursor_pos_bwd(Evas_Object * _o);
void cl_cursor_insert(Evas_Object * _o, const char * _new_text);
void cl_cursor_del(Evas_Object * _o);
void cl_cursor_bsp(Evas_Object * _o);
void cl_readlen_set(Evas_Object * _o, int _len);
void cl_readlen_get(Evas_Object * _o, int *_len);
void cl_refresh(Evas_Object * _o);

void cl_configure(Evas_Coord w, Evas_Coord h);
void cl_hint_set(Evas_Object *_o, const char *str);

void cl_handle_key(Evas_Object *oparent, void *event_info);

void cl_shutdown(void);
