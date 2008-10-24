
void                line_create(void);
Line               *_line_create_copy(Line *);

void                line_load(int);
void                line_redraw(Line *);
void                line_sync(Line *);
void                line_destroy(Line *);
void                line_free(Line *);

void                line_move(Line *, double, double);
void                line_scale(Line *, double, double, double);
void                line_scale_xy(Line *, double, double, double, double);
void                line_rotate(Line *, double, double, double);

void                line_clone(Line *, double, double);
void                line_array(Line *, int, int, double, double);
void                line_array_polar(Line *, double, double, int, double);

void                line_mirror_ab(Line *, double, double);
void                line_mirror_y(Line *, double);
void                line_mirror_x(Line *, double);

void                line_trim_ab(Line *, double, double, int);
void                line_trim_y(Line *, double, int);
void                line_trim_x(Line *, double, int);

void                line_delete(Line *);
void                line_cut(Line *);
void                line_copy(Line *, int, double, double);
void                line_paste(CP_Header, int, double, double);

char              **line_get_info(Line *);

int                 line_select_by_click(Line *, double, double);
void                line_select_by_rect(Line *, double, double, double, double);
int                 line_deselect_by_click(Line *, double, double);
void                line_deselect_by_rect(Line *, double, double, double,
                                          double);

void                ghost_line_create(void);
void                ghost_line_redraw(Eina_List*, double, double);
void                ghost_line_destroy(void);

Evas_Object        *_line_item(Evas *e, double w);
void                _line_item_xy(Evas *e, Evas_Object *o, double x1, double y1,
                                  double x2, double y2, double w);
