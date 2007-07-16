
void                earc_create(void);
EArc               *_earc_create_copy(EArc *);

void                earc_load(int);
void                earc_redraw(EArc *);
void                earc_sync(EArc *);
void                earc_destroy(EArc *);
void                earc_free(EArc *);

void                earc_move(EArc *, double, double);
void                earc_scale(EArc *, double, double, double);
void                earc_scale_xy(EArc *, double, double, double, double);
void                earc_rotate(EArc *, double, double, double);

void                earc_clone(EArc *, double, double);
void                earc_array(EArc *, int, int, double, double);
void                earc_array_polar(EArc *, double, double, int, double);

void                earc_mirror_ab(EArc *, double, double);
void                earc_mirror_y(EArc *, double);
void                earc_mirror_x(EArc *, double);

void                earc_trim_ab(EArc *, double, double, int);
void                earc_trim_y(EArc *, double, int);
void                earc_trim_x(EArc *, double, int);

void                earc_delete(EArc *);
void                earc_cut(EArc *);
void                earc_copy(EArc *, int, double, double);
void                earc_paste(CP_Header, int, double, double);
char              **earc_get_info(EArc *);

int                 earc_select_by_click(EArc *, double, double);
void                earc_select_by_rect(EArc *, double, double, double, double);
int                 earc_deselect_by_click(EArc *, double, double);
void                earc_deselect_by_rect(EArc *, double, double, double,
                                          double);
